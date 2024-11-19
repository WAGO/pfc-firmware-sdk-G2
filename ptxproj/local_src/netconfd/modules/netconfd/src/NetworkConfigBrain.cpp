// SPDX-License-Identifier: GPL-2.0-or-later

#include "NetworkConfigBrain.hpp"

#include "IInterfaceMonitor.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <string>

#include "CollectionUtils.hpp"
#include "EthernetInterface.hpp"
#include "IBridgeInformation.hpp"
#include "InterfaceConfigManager.hpp"
#include "IpAddressManipulator.hpp"
#include "IpConfigHelper.hpp"
#include "JsonConverter.hpp"
#include "LinkMode.hpp"
#include "Logger.hpp"
#include "NetDevManager.hpp"
#include "Status.hpp"
#include "TypesHelper.hpp"
#include "InterfaceValidator.hpp"

namespace netconf {

namespace {

Interfaces GetBridgeInterfaces(const BridgeConfig &bc) {
  Interfaces itf;
  for (auto &b : bc) {
    itf.emplace_back(b.first);
  }
  return itf;
}

void RemoveInterfaceByName(const Interface &interface, Interfaces &interfaces) {
  interfaces.erase(::std::remove_if(interfaces.begin(), interfaces.end(),
                                    [&interface](const auto &i){return interface.GetName() == i.GetName();}),
                   interfaces.end());
}

void RemoveEmptyBridges(BridgeConfig &config) {
  LOG_DEBUG("");
  auto it = config.cbegin();
  while (it != config.cend()) {
    if (it->second.empty()) {
      it = config.erase(it);
    } else {
      it++;
    }
  }
}

void RemoveVlanInterfacesWithInvalidLink(Interfaces &interfaces, INetDevManager &netdev_manager) {

  auto linkDoesNotExist = [&](Interface& interface){
    if(interface.GetProperties().count(InterfaceProperty::Link) > 0){
      ::std::string name = ::boost::get<::std::string>(interface.GetProperties().at(InterfaceProperty::Link));
      NetDevPtr link = netdev_manager.GetByName(name);
      if(not link){
        return true;
      }
    }
    return false;
  };

  interfaces.erase(::std::remove_if(interfaces.begin(), interfaces.end(), linkDoesNotExist), interfaces.end());
}

void ResetInvalidIPSource(IPConfigs &ip_configs, const Interfaces &interfaces) {
  for(const auto& itf : interfaces) {
    auto ip_config = GetIpConfigByInterface(ip_configs, itf);

    if(ip_config != ip_configs.end() && !hasValidIPSourceForDeviceType(*ip_config, itf.GetType())) {
      LogWarning("Found invalid IP source " + IPSourceToString(ip_config->source_) + " for interface: " 
        + ip_config->interface_ + ". Changing IP source to None.");
      
      ip_config->source_ = IPSource::NONE;
    }
  }
}

}  // namespace

using namespace ::std::literals;

NetworkConfigBrain::NetworkConfigBrain(IBridgeManager &bridge_manager, IBridgeInformation &itf_info,
                                       IIPManager &ip_manager, IEventManager &event_manager,
                                       IPersistenceProvider &persistence_provider, IDipSwitch &ip_dip_switch,
                                       InterfaceConfigManager &interface_config_manager, INetDevManager &netdev_manager,
                                       IHostnameWillChange &hostname_manager)

    : bridge_manager_{bridge_manager},
      bridge_information_{itf_info},
      interface_config_manager_{interface_config_manager},
      ip_manager_{ip_manager},
      event_manager_{event_manager},
      persistence_provider_{persistence_provider},
      netdev_manager_{netdev_manager},
      ip_dip_switch_{ip_dip_switch},
      hostname_will_change_{hostname_manager} {
}

void NetworkConfigBrain::Start(StartWithPortstate startwithportstate) {
  Status statusBridgeConfig;
  Status statusIPConfig;
  Status statusDIPSwitchIPConfig;
  Status status;

  BridgeConfig bridge_config;
  Interfaces interfaces;
  IPConfigs ip_configs;
  DipSwitchIpConfig dip_switch_ip_config;

  auto port_interfaces = netdev_manager_.GetInterfacesByDeviceType({DeviceType::Port});
  if (port_interfaces.empty()) {
    LogError("Start: Failed to determine available product interfaces.");
    return;
  }

  LOG_DEBUG("Read Persistence");
  statusBridgeConfig = persistence_provider_.Read(bridge_config);
  persistence_provider_.Read(interfaces);
  statusIPConfig          = persistence_provider_.Read(ip_configs);
  statusDIPSwitchIPConfig = persistence_provider_.Read(dip_switch_ip_config);

  LOG_DEBUG("validate bridge config");
  if (statusBridgeConfig.IsOk()) {
    RemoveUnsupportedPorts(bridge_config, port_interfaces);
    RemoveEmptyBridges(bridge_config);
  } else {
    ResetBridgeConfigToDefault(port_interfaces, bridge_config);
    LogWarning("Start: " + statusBridgeConfig.ToString());
  }

  if (statusIPConfig.IsNotOk()) {
    LOG_DEBUG("Start: Reset IP configuration.");
    ResetIpConfigsToDefault(ip_configs);
  }

  auto bridges                         = GetBridgeInterfaces(bridge_config);
  Interfaces ip_addressable_interfaces = interfaces;
  ip_addressable_interfaces.insert(ip_addressable_interfaces.end(), bridges.begin(), bridges.end());

  FilterRequiredIpConfigs(ip_configs, ip_addressable_interfaces);
  ResetInvalidIPSource(ip_configs, ip_addressable_interfaces);
  RemoveUnnecessaryIPParameter(ip_configs);

  if (ip_configs.empty()) {
    LogWarning("Start: No IP config left on startup!");
  }

  status = ip_manager_.ValidateIPConfigs(ip_configs, ip_addressable_interfaces);
  if (status.IsNotOk()) {
    LogWarning("Start: IP validation failed. Reset IP configuration.");
    ResetIpConfigsToDefault(ip_configs);
    status = Status{};
  }

  // At this point bridge and IP configs should be valid.

  if (statusDIPSwitchIPConfig.IsNotOk()) {
    LOG_DEBUG("Start: Reset DIP switch IP configuration.");
    ResetDIPSwitchIPConfigToDefault(dip_switch_ip_config);
  }

  LOG_DEBUG("write persistence");
  persistence_provider_.Write(bridge_config);
  persistence_provider_.Write(interfaces);
  persistence_provider_.Write(ip_configs);
  persistence_provider_.Write(dip_switch_ip_config);

  if (status.IsOk()) {
    LOG_DEBUG("apply config");
    status = ApplyConfig(bridge_config, interfaces, ip_configs, dip_switch_ip_config);
  }

  interface_config_manager_.InitializePorts(static_cast<InterfaceState>(startwithportstate));

  LOG_DEBUG("trigger event folder");
  if (status.IsOk()) {
    event_manager_.NotifyNetworkChanges(EventLayer::EVENT_FOLDER);
    event_manager_.ProcessPendingEvents();
  }

  if (status.IsNotOk()) {
    LogError("Start: " + status.ToString());
  }
}

void NetworkConfigBrain::ResetBridgeConfigToDefault(Interfaces product_interfaces, BridgeConfig &config) {
  LOG_DEBUG("");
  config = {{Interface::CreateBridge("br0"), product_interfaces}};
}

void NetworkConfigBrain::ResetIpConfigsToDefault(IPConfigs &configs) {
  configs = {{"br0", IPSource::DHCP, ZeroIP, ZeroNetmask}};
}

void NetworkConfigBrain::ResetDIPSwitchIPConfigToDefault(DipSwitchIpConfig &config) {
  config = {"192.168.1.0", "255.255.255.0"};
}


Status NetworkConfigBrain::RemoveUnnecessaryInterfaces(const Interfaces &current_interfaces, const Interfaces &interfaces) const {

  Interfaces interfaces_to_delete{};
  copy_if(current_interfaces.begin(), current_interfaces.end(), ::std::back_inserter(interfaces_to_delete),
          [&](const auto& persisted_interface){return IsNotIncluded(persisted_interface, interfaces);} );

  return netdev_manager_.Delete(interfaces_to_delete);
}

Status NetworkConfigBrain::AddMissingInterfaces(const Interfaces &current_interfaces, const Interfaces &interfaces) const {

  Interfaces interfaces_to_add{};
  copy_if(interfaces.begin(), interfaces.end(), ::std::back_inserter(interfaces_to_add),
            [&](const auto& interface){return IsNotIncluded(interface, current_interfaces);} );

  return netdev_manager_.AddInterfaces(interfaces_to_add);
}

Status NetworkConfigBrain::ConfigureInterfaces(const Interfaces& interfaces, const Interfaces& current_interfaces) const {
  Status status = RemoveUnnecessaryInterfaces(current_interfaces, interfaces);
  if (status.IsOk()) {
    status = AddMissingInterfaces(current_interfaces, interfaces);
  }
  return status;
}

void NetworkConfigBrain::FilterRequiredIpConfigs(IPConfigs &ip_configs, const Interfaces &interfaces) const {
  ::std::vector<::std::string> interfaces_to_remove;
  for (auto &ip_config : ip_configs) {
    auto it = ::std::find_if(interfaces.begin(), interfaces.end(), [&](const Interface& itf){
      return itf.GetName() == ip_config.interface_;
    });
    if (it == interfaces.end()) {
      interfaces_to_remove.push_back(ip_config.interface_);
    }
  }

  for (auto itf : interfaces_to_remove) {
    ip_configs.erase(::std::remove_if(ip_configs.begin(), ip_configs.end(),
                                      [&](IPConfig &ip_config) { return ip_config.interface_ == itf; }),
                     ip_configs.end());
  }
}

void NetworkConfigBrain::RemoveUnsupportedPorts(BridgeConfig &config, Interfaces &supported_interfaces) const {
  LOG_DEBUG("");
  for (auto &bridge_entry : config) {
    Interfaces unsupported_interfaces;

    for (auto &interface : bridge_entry.second) {
      if (IsNotIncluded(interface, supported_interfaces)) {
        unsupported_interfaces.push_back(interface);
      }
    }

    for (auto &interface : unsupported_interfaces) {
      RemoveByValue(interface, bridge_entry.second);
    }
  }
}

Status NetworkConfigBrain::ApplyConfig(BridgeConfig &bridge_config, const Interfaces &interfaces,
                                       const IPConfigs &ip_configs, const DipSwitchIpConfig &dip_ip_config) const {
  Status status = bridge_manager_.ApplyBridgeConfiguration(bridge_config);

  if (status.IsOk()) {
    LOG_DEBUG("AddInterfaces");
    status = netdev_manager_.AddInterfaces(interfaces);
    LOG_STATUS(status);
  }

  if (status.IsOk()) {
    LOG_DEBUG("ApplyIpConfiguration");
    status = ip_manager_.ApplyIpConfiguration(ip_configs, dip_ip_config);
    LOG_STATUS(status);
  }

  return status;
}

::std::string NetworkConfigBrain::SetBridgeConfig(::std::string const &config) {
  BridgeConfig bridge_config;
  Status status = jc.FromJsonString(config, bridge_config);

  RemoveEmptyBridges(bridge_config);

  BridgeConfig current_config;
  if (status.IsOk()) {
    persistence_provider_.Read(current_config);

    if (IsEqual(bridge_config, current_config)) {
      LOG_DEBUG("New bridge configuration is equal to current. Do nothing.");
      return jc.ToJsonString(Status{});
    }
  }

  if (status.IsOk()) {
    LOG_DEBUG("ApplyBridgeConfiguration");
    status = bridge_manager_.ApplyBridgeConfiguration(bridge_config);
    LOG_STATUS(status);
  }

  if (status.IsOk()) {
    auto const &interface_configs = interface_config_manager_.GetPortConfigs();
    auto interface_config_status  = interface_config_manager_.Configure(interface_configs);
    if (interface_config_status.IsNotOk()) {
      LogError("Set Bridge Config: Re-apply interface config error: " + status.ToString());
    }
  }

  Status persistence_status;
  if (status.IsOk()) {
    persistence_status = persistence_provider_.Write(bridge_config);

    if(persistence_status.IsOk()){
      //INFO: we have to check whether link references have been omitted due to a new bridge configuration
      //and remove the affected virtual interfaces.
      Interfaces interfaces;
      persistence_provider_.Read(interfaces);

      RemoveVlanInterfacesWithInvalidLink(interfaces, netdev_manager_);

      persistence_status = persistence_provider_.Write(interfaces);
    }
  }

  if (status.IsOk()) {
    event_manager_.NotifyNetworkChanges(EventLayer::EVENT_FOLDER);
    event_manager_.ProcessPendingEvents();
  }

  if (persistence_status.IsNotOk()) {
    status.Set(StatusCode::PERSISTENCE_WRITE);
  }

  if (status.IsNotOk()) {
    LogError("Set Bridge Config: " + status.ToString());
  }

  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::GetBridgeConfig(::std::string &config) const {
  BridgeConfig bc;
  Status s = persistence_provider_.Read(bc);
  if(s.IsNotOk()){
    return jc.ToJsonString(s);
  }

  config = jc.ToJsonString(bc);
  return jc.ToJsonString(Status{});
}

::std::string NetworkConfigBrain::GetInterfaceInformation(::std::string &config) const {
  auto iis = interface_config_manager_.GetInterfaceInformations();
  config   = jc.ToJsonString(iis);

  return jc.ToJsonString(Status{});
}

Status NetworkConfigBrain::ContainOnlyInterfacesOfTypePort(const InterfaceConfigs &interface_configs) const {

  Status status;
  auto port_interfaces = netdev_manager_.GetInterfacesByDeviceType({DeviceType::Port});

  for(const auto &itf_config : interface_configs) {
    bool is_of_type_port = false;
    for(const auto &port : port_interfaces) {
      if(itf_config.interface_.GetName() == port.GetName()) {
        is_of_type_port = true;
        break;
      }
    }
    if(not is_of_type_port) {
      status.Set(StatusCode::GENERIC_ERROR);
      status.Append("Device \"" + itf_config.interface_.GetName() + "\" is not of type Port." );
    }
  }

  return status;
}

::std::string NetworkConfigBrain::SetInterfaceConfig(const ::std::string &config) {
  InterfaceConfigs port_configs;

  auto status = jc.FromJsonString(config, port_configs);

  if (status.IsOk()) {
    status = ContainOnlyInterfacesOfTypePort(port_configs);
  }

  if (status.IsOk()) {
    status = interface_config_manager_.Configure(port_configs);
  }

  if (status.IsOk()) {
    event_manager_.ProcessPendingEvents();
  }

  if (status.IsNotOk()) {
    LogError("SetInterfaceConfig: " + status.ToString());
  }

  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::GetInterfaceConfig(::std::string &config) const {
  auto port_configs = interface_config_manager_.GetPortConfigs();

  config = jc.ToJsonString(port_configs);

  return jc.ToJsonString(Status{});
}

::std::string NetworkConfigBrain::GetInterfaceStatuses(::std::string &config) const {
  InterfaceStatuses itf_statuses;
  Status s = interface_config_manager_.GetCurrentPortStatuses(itf_statuses);

  config = jc.ToJsonString(itf_statuses);

  return jc.ToJsonString(s);
}

::std::string NetworkConfigBrain::GetAllIPConfigs(::std::string &config) const {
  IPConfigs configs = ip_manager_.GetIPConfigs();

  config = jc.ToJsonString(configs);

  return jc.ToJsonString(Status{});
}

::std::string NetworkConfigBrain::GetCurrentIPConfigs(::std::string &config) const {
  IPConfigs configs = ip_manager_.GetCurrentIPConfigs();

  config = jc.ToJsonString(configs);
  return jc.ToJsonString(Status{});
}

::std::string NetworkConfigBrain::SetAllIPConfigs(::std::string const &json_config) {
  IPConfigs ip_configs;

  Status status = jc.FromJsonString(json_config, ip_configs);

  RemoveUnnecessaryIPParameter(ip_configs);

  if (status.IsOk()) {
    status = ip_manager_.ApplyIpConfiguration(ip_configs);
  }

  if (status.IsNotOk()) {
    LogError("SetAllIPConfigs: " + status.ToString());
  }

  event_manager_.ProcessPendingEvents();

  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::SetIPConfig(::std::string const &config) {
  return SetAllIPConfigs(config);
}

::std::string NetworkConfigBrain::GetBackupParamterCount() const {
  return ::std::to_string(persistence_provider_.GetBackupParameterCount());
}

::std::string NetworkConfigBrain::Backup(const ::std::string &file_path, const ::std::string &targetversion) const {
  Status status = persistence_provider_.Backup(file_path, targetversion);

  if (status.IsNotOk()) {
    LogError("Backup: " + status.ToString());
  }

  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::Restore(const std::string &file_path) {
  BridgeConfig bridge_config;
  IPConfigs ip_configs;
  InterfaceConfigs interface_configs;
  DipSwitchIpConfig dip_switch_ip_config;
  Interfaces interfaces;

  BridgeConfig persisted_bridge_config;
  IPConfigs persisted_ip_configs;
  Interfaces persisted_interfaces;
  Status status = persistence_provider_.Read(persisted_bridge_config, persisted_ip_configs, persisted_interfaces);
  if (status.IsOk()) {
    status = persistence_provider_.Restore(file_path, bridge_config, ip_configs, interface_configs,
                                           dip_switch_ip_config, interfaces);
  }

  if (status.IsOk()) {
    interface_config_manager_.Configure(interface_configs);

    RemoveEmptyBridges(bridge_config);
    status = bridge_manager_.ApplyBridgeConfiguration(bridge_config);
    if (status.IsNotOk()) {
      LogError("Failed to restore bridge configuration from file:"s + file_path);
    }

    auto current_virtual_interfaces = netdev_manager_.GetInterfacesByDeviceType(VirtualInterfaceDeviceTypes());
    status = ConfigureInterfaces(interfaces, current_virtual_interfaces);
    if(status.IsNotOk()){
      LogError("Failed to restore interface configuration from file:"s + file_path);
    }

    current_virtual_interfaces = netdev_manager_.GetInterfacesByDeviceType(VirtualInterfaceDeviceTypes());
    ResetInvalidIPSource(ip_configs, current_virtual_interfaces);

    /* Clean out ip configs that are not available,
     * the restore data might contain unnecessary ip data from former netconfd releases. */
    RemoveUnnecessaryIPParameter(ip_configs);
    CleanWithRespectToSystem(ip_configs, netdev_manager_.GetNetDevs(),
                             bridge_information_.GetBridgeAssignedInterfaces());
    status = ip_manager_.ApplyIpConfiguration(ip_configs, dip_switch_ip_config);
    if (status.IsNotOk()) {
      LogError("Failed to restore ip configuration from file:"s + file_path);
    }

    // Try rollback if status is not ok
    if (status.IsNotOk()) {
      status = ApplyConfig(persisted_bridge_config, persisted_interfaces, persisted_ip_configs, dip_switch_ip_config);
      if (status.IsNotOk()) {
        LogError("Failed to rollback after restore error: " + status.ToString());
      }
    }
  }

  if (status.IsOk()) {
    event_manager_.ProcessPendingEvents();
  }

  if (status.IsNotOk()) {
    LogError("Restore configurations: " + status.ToString());
  }

  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::SetTemporaryFixIp() {
  auto status = ip_manager_.ApplyTempFixIpConfiguration();

  if (status.IsOk()) {
    event_manager_.ProcessPendingEvents();
  } else {
    LogError("Set temp fix IP: " + status.ToString());
  }
  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::GetDipSwitchConfig(::std::string &config) const {
  DipSwitchIpConfig dip_switch_ip_config;
  Status status = persistence_provider_.Read(dip_switch_ip_config);

  auto dip_switch_config = DipSwitchConfig(dip_switch_ip_config, ip_dip_switch_.GetMode(), ip_dip_switch_.GetValue());

  config = jc.ToJsonString(dip_switch_config);

  return jc.ToJsonString(Status{});
}

::std::string NetworkConfigBrain::SetDipSwitchConfig(const ::std::string &config) {
  if (ip_dip_switch_.GetMode() == DipSwitchMode::HW_NOT_AVAILABLE) {
    return jc.ToJsonString(Status{StatusCode::DIP_NOT_AVAILABLE});
  }

  DipSwitchIpConfig dip_switch_ip_config;

  auto status = jc.FromJsonString(config, dip_switch_ip_config);

  if (status.IsOk()) {
    status = ip_manager_.ApplyDipConfiguration(dip_switch_ip_config);
  }

  if (status.IsOk()) {
    event_manager_.ProcessPendingEvents();
  }

  if (status.IsNotOk()) {
    LogError("DIP Switch: " + status.ToString());
  }

  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::ReceiveDynamicIPEvent(const ::std::string &event) {
  ::std::string interface;
  DynamicIPEventAction action;
  auto status = jc.FromJsonString(event, interface, action);

  if (status.IsOk()) {
    auto netdev = netdev_manager_.GetByName(interface);
    if (netdev) {
      ip_manager_.OnDynamicIPEvent(netdev->GetInterface(), action);
    } else {
      LogWarning("Received dynamic IP event [" +  ::std::to_string(static_cast<int>(action))  +  "] for unknown interface: " + interface);
      status = Status{StatusCode::INTERFACE_NOT_EXISTING, interface};
    }
  }

  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::ReceiveReloadHostConfEvent() {
  hostname_will_change_.OnReloadHostConf();

  event_manager_.NotifyNetworkChanges(EventLayer::EVENT_FOLDER);
  event_manager_.ProcessPendingEvents();

  return jc.ToJsonString(Status{});
}

::std::string NetworkConfigBrain::AddInterface(const ::std::string &data) {
  Interface new_interface;
  Interfaces persisted_interfaces;
  persistence_provider_.Read(persisted_interfaces);

  auto current_interfaces = netdev_manager_.GetInterfaces();
  Status status = jc.FromJsonString(data, new_interface);

  if (status.IsOk()) {
    status = Validate(new_interface, current_interfaces, persisted_interfaces);

    if (status.IsOk()) {
      status = netdev_manager_.AddInterface(new_interface);
    }

    if (status.IsOk()) {
      persisted_interfaces.emplace_back(new_interface);
      status = persistence_provider_.Write(persisted_interfaces);
    }
  }

  LOG_STATUS(status);
  return jc.ToJsonString(status);
}

::std::string NetworkConfigBrain::DeleteInterface(const ::std::string &data) {
  auto current_interfaces = netdev_manager_.GetInterfacesByDeviceType(VirtualInterfaceDeviceTypes());

  Interface interface_to_delete;
  Status status = jc.FromJsonString(data, interface_to_delete);

  if(status.IsOk()) {
    status = ValidateName(interface_to_delete);
  }

  if (status.IsOk()) {

    auto it = ::std::find_if(begin(current_interfaces), end(current_interfaces),
                             [&](const auto &i) { return interface_to_delete.GetName() == i.GetName(); });

    if (it != end(current_interfaces)) {
      status = netdev_manager_.Delete(*it);
    } else {
      LOG_DEBUG("INTERFACE_NOT_EXISTING");
      status.Set(StatusCode::INTERFACE_NOT_EXISTING);
      status.Append(interface_to_delete.GetName());
    }
  }

  if (status.IsOk()) {
    Interfaces persisted_interfaces;
    persistence_provider_.Read(persisted_interfaces);

    RemoveInterfaceByName(interface_to_delete, persisted_interfaces);

    RemoveVlanInterfacesWithInvalidLink(persisted_interfaces, netdev_manager_);

    status = persistence_provider_.Write(persisted_interfaces);
  }
  LOG_STATUS(status);
  return jc.ToJsonString(status);
}

} /* namespace netconf */
