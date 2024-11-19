// SPDX-License-Identifier: GPL-2x.0-or-later

#include "IPManager.hpp"

#include <unistd.h>

#include <algorithm>

#include "CollectionUtils.hpp"
#include "DynamicIPEventAction.hpp"
#include "FixIP.hpp"
#include "IpAddressManipulator.hpp"
#include "IpConfigHelper.hpp"
#include "Logger.hpp"
#include "TypesHelper.hpp"

namespace netconf {

namespace {

DynamicIPType IPSourceToDynamicIPClient(IPSource source) {
  DynamicIPType type = DynamicIPType::UNDEFINED;
  if (source == IPSource::DHCP) {
    type = DynamicIPType::DHCP;
  }
  if (source == IPSource::BOOTP) {
    type = DynamicIPType::BOOTP;
  }
  return type;
}

::std::string ChangesToString(const ::std::string& info_name, const ::std::string& old_value, const ::std::string& new_value){
  if(old_value != new_value){
    return "[" + info_name + ": " + old_value + " -> " + new_value + "] ";
  }
  return "";
}

}  // namespace

IPManager::IPManager(IEventManager &event_manager, IPersistenceProvider &persistence_provider,
                     INetDevManager &netdev_manager, IDipSwitch &ip_dip_switch,
                     IInterfaceInformation &interface_information, IDynamicIPClientAdministrator &dyn_ip_client,
                     IIPController &ip_controller, ::std::shared_ptr<IIPMonitor> &ip_monitor,
                     IHostnameManager &hostname_manager)
    : event_manager_{event_manager},
      persistence_provider_{persistence_provider},
      ip_dip_switch_{ip_dip_switch},
      netdev_manager_{netdev_manager},
      interface_information_{interface_information},
      dyn_ip_client_admin_{dyn_ip_client},
      ip_controller_{ip_controller},
      ip_monitor_{ip_monitor},
      hostname_manager_{hostname_manager} {
  netdev_manager_.RegisterForNetDevConstructionEvents(*this);
  ip_monitor_->RegisterEventHandler(*this);
  hostname_manager_.RegisterIPManager(*this);

  auto netdevs = netdev_manager.GetNetDevs(SupportedDeviceTypes());
  std::for_each(netdevs.begin(), netdevs.end(), [this](auto &netdevptr) { this->OnNetDevCreated(netdevptr); });
}

IPLinkPtr IPManager::GetIPLinkByInterface(const Interface &interface) const {
  auto matching_interface = [&](::std::shared_ptr<IPLink> ip_link) { return interface == ip_link->GetInterface(); };

  auto ip_link = ::std::find_if(ip_links_.begin(), ip_links_.end(), matching_interface);
  if (ip_link != ip_links_.end()) {
    return *ip_link;
  }
  return nullptr;
}

IPLinkPtr IPManager::CreateOrGet(const Interface &interface) {
  auto ip_link = GetIPLinkByInterface(interface);
  if (ip_link) {
    return ip_link;
  }

  auto netdev   = netdev_manager_.GetByInterface(interface);
  auto if_index = netdev->GetIndex();

  ip_link = ::std::make_shared<IPLink>(if_index, interface, ip_monitor_->GetIPAddress(if_index),
                                       ip_monitor_->GetNetmask(if_index), netdev->GetLinkState());

  ip_links_.push_back(ip_link);

  return ip_link;
}

void IPManager::ConfigureDynamicIPClient(IPLinkPtr &link) {
  auto client = dyn_ip_client_admin_.GetClient(link->GetInterface());
  if (client) {
    // 1) if the desired client exists with same client-id we do nothing and return.
    if (client->GetType() == IPSourceToDynamicIPClient(link->GetSource()) && client->GetClientID() == link->GetClientID()) {
        return;
    }
    // 2) terminate client if of different IpSource or client-id mismatch.
    if (client->GetType() != IPSourceToDynamicIPClient(link->GetSource()) || client->GetClientID() != link->GetClientID()) {
      hostname_manager_.OnLeaseFileRemove(link->GetInterface());
      dyn_ip_client_admin_.DeleteClient(link->GetInterface());
      ip_controller_.Flush(link->GetInterface().GetName());
    }
  }

  auto source = link->GetSource();
  if (source == IPSource::DHCP) {
    ip_controller_.Flush(link->GetInterface().GetName());
    dyn_ip_client_admin_.AddClient(DynamicIPType::DHCP, link->GetInterface(), hostname_manager_.GetHostname(), link->GetClientID());
  }
  if (source == IPSource::BOOTP) {
    ip_controller_.Flush(link->GetInterface().GetName());
    dyn_ip_client_admin_.AddClient(DynamicIPType::BOOTP, link->GetInterface(), hostname_manager_.GetHostname(), "");
  }
}

Status IPManager::ConfigureIP(const IPLinkPtr &link) const {
  Status status;

  ::std::optional<IPConfig> ip_config = link->GetIPConfig();
  if(ip_config.has_value()){
    if (IPConfig::SourceIsAnyOf(ip_config.value(), IPSource::STATIC, IPSource::FIXIP)) {
      if (link->GetLinkState() == eth::InterfaceLinkState::Up) {
        status = ip_controller_.Configure(ip_config.value());
      }
    }
    if (IPConfig::SourceIsAnyOf(ip_config.value(), IPSource::NONE, IPSource::EXTERNAL)) {
      ip_controller_.Flush(ip_config.value().interface_);
    }
  }

  return status;
}

Status IPManager::Configure(const IPConfigs &configs) {
  Status status;

  for (auto &ip_config : configs) {
    NetDevPtr netdev = netdev_manager_.GetByName(ip_config.interface_);
    if(netdev){
      auto ip_link = CreateOrGet(netdev->GetInterface());
      ip_link->SetIPConfig(ip_config);
      gratuitous_arp_.EnableGratuitousArp(ip_link);
      ConfigureDynamicIPClient(ip_link);
      ConfigureIP(ip_link);
    }
  }

  return status;
}

IPConfigs IPManager::GetIPConfigs() const {
  IPConfigs ip_configs;
  ip_configs.reserve(ip_links_.size());

  for (auto &link : ip_links_) {
    auto ip = link->GetIPConfig();
    if(ip){
      ip_configs.emplace_back(ip.value());
    } else {
      ip_configs.emplace_back(link->GetInterface().GetName());
    }
  }
  return ip_configs;
}

IPConfigs IPManager::GetCurrentIPConfigs() const {
  IPConfigs ip_configs;
  ip_configs.reserve(ip_links_.size());
  for (auto &link : ip_links_) {
    ip_configs.emplace_back(link->GetCurrentIPConfig());
  }

  return ip_configs;
}

Status IPManager::ValidateIPConfigs(const IPConfigs &configs, const Interfaces &interfaces) const {
  return IPValidator::ValidateIPConfigs(configs, interfaces);
}

void IPManager::OnNetDevCreated(NetDevPtr netdev) {
  CreateOrGet(netdev->GetInterface());
}

void IPManager::OnNetDevRemoved(NetDevPtr netdev) {
  auto it_link = ::std::find_if(ip_links_.begin(), ip_links_.end(),
                                [&](auto &link) { return netdev->GetInterface() == link->GetInterface(); });

  if (it_link != ip_links_.end()) {
    hostname_manager_.OnLeaseFileRemove((*it_link)->GetInterface());
    dyn_ip_client_admin_.DeleteClient((*it_link)->GetInterface());
    ip_links_.erase(it_link);
  }

  IPConfigs configs;
  Status status = persistence_provider_.Read(configs);

  auto it = ::std::find_if(configs.begin(), configs.end(),
                           [&](IPConfig config) { return netdev->GetInterface().GetName() == config.interface_; });
  if (it != configs.end()) {
    configs.erase(it);
  }

  if (status.IsOk()) {
    status = persistence_provider_.Write(configs);
  }
}

void IPManager::OnLinkChange(NetDevPtr netdev) {
  IPLinkPtr link = CreateOrGet(netdev->GetInterface());
  link->SetLinkState(netdev->GetLinkState());

  ::std::optional<IPConfig> config = link->GetIPConfig();

  if (config.has_value() and config.value().source_ == IPSource::EXTERNAL) {
    return;
  }

  if (config.has_value()) {
    if (link->GetLinkState() == eth::InterfaceLinkState::Up) {
      gratuitous_arp_.EnableGratuitousArp(link);

      auto client = dyn_ip_client_admin_.GetClient(link->GetInterface());
      if (client) {
        client->Renew();
      }
      ConfigureIP(link);
    } else {
      auto client = dyn_ip_client_admin_.GetClient(link->GetInterface());
      if (client) {
        client->Release();
      }
      ip_controller_.Flush(link->GetInterface().GetName());
    }
  }

  // Port
  if (netdev->GetDeviceType() == DeviceType::Port) {
    NetDevPtr br_netdev = netdev_manager_.GetParent(netdev);
    if (br_netdev) {
      auto client = dyn_ip_client_admin_.GetClient(br_netdev->GetInterface());
      if (client) {
        client->Renew();
      }
      if(link->GetLinkState() == eth::InterfaceLinkState::Up){
        auto br_link = GetIPLinkByInterface(br_netdev->GetInterface());
        gratuitous_arp_.SendGratuitousArpOnPort(netdev, br_netdev, br_link->GetAddress());
      }
    }
  }

}

void IPManager::OnBridgePortsChange(NetDevPtr netdev) {
  auto link = CreateOrGet(netdev->GetInterface());

  ::std::optional<IPConfig> config = link->GetIPConfig();
  if (config.has_value() and IPConfig::SourceIsAnyOf(config.value(), IPSource::BOOTP, IPSource::DHCP)) {
    gratuitous_arp_.EnableGratuitousArp(link);

    auto client = dyn_ip_client_admin_.GetClient(link->GetInterface());
    if (client) {
      client->Renew();
    }
  }
}

void IPManager::OnAddressChange(ChangeType change_type, int index, Address address, Netmask netmask) {

  auto find_interface_index_and_set_ip =
      [&](::std::shared_ptr<IPLink> link) {
        if (link->GetIfIndex() == index) {
          LOG_DEBUG(link->GetInterface().GetName() << " action: " << ToString(change_type) << " "
                    << ChangesToString("address", link->GetCurrentIPConfig().address_, address)<< " "
                    << ChangesToString("netmask", link->GetCurrentIPConfig().netmask_, netmask));
          if (change_type == ChangeType::Delete) {
            link->SetAddress(ZeroIP, ZeroNetmask);
          } else {
            link->SetAddress(address, netmask);
            auto netdev = netdev_manager_.GetByIfIndex(link->GetIfIndex());
            gratuitous_arp_.SendGratuitousArpOnBridge(netdev, address);
          }

          hostname_manager_.OnInterfaceIPChange();
          event_manager_.NotifyNetworkChanges(EventLayer::IP_CHANGE_FILES, link->GetInterface());
          event_manager_.NotifyNetworkChanges(EventLayer::EVENT_FOLDER);
          return true;
        }
        return false;
      };

  ::std::find_if(ip_links_.begin(), ip_links_.end(), find_interface_index_and_set_ip);
}

void IPManager::OnDynamicIPEvent(const Interface &interface, DynamicIPEventAction action) {
  auto client = dyn_ip_client_admin_.GetClient(interface);
  if (client) {
    switch (action) {
      case DynamicIPEventAction::BOUND: {
        /* bound: This argument is used when udhcpc moves from an unbound, to a bound state. All of the paramaters are
         set in enviromental variables, The script should configure the interface, and set any other relavent parameters
         (default gateway, dns server, etc). */

        client->UpdateContentFromLease();
        ip_controller_.Configure(interface.GetName(), client->GetAddressFromLease(), client->GetNetmaskFromLease());
      } break;
      case DynamicIPEventAction::RELEASE: {
        /* deconfig: This argument is used when udhcpc starts, and  when a leases is lost. The script should put the
         interface in an up, but deconfigured state, ie: ifconfig $interface 0.0.0.0.*/

        ip_controller_.Flush(interface.GetName());

        /* 06.06.2024 The following attempts to workaround a problem where udhcpc is seamingly stuck in the released
         * state. */
        auto ip_link = GetIPLinkByInterface(interface);
        if (ip_link && ip_link->GetLinkState() == eth::InterfaceLinkState::Up) {
          auto ip_config = ip_link->GetIPConfig();
          if (ip_config) {
            if (IPConfig::SourceIsAnyOf(ip_config.value(), IPSource::BOOTP, IPSource::DHCP)) {
              client->Renew();
            }
          }
        }

        break;
      }
      case DynamicIPEventAction::RENEW:
        /* renew: This argument is used when a DHCP lease is renewed. All of the paramaters are set in enviromental
         variables. This argument is used when the interface is already configured, so the IP address, will not change,
         however, the other DHCP paramaters, such as the default gateway, subnet mask, and dns server may change. */
        client->UpdateContentFromLease();
        ip_controller_.Configure(interface.GetName(), client->GetAddressFromLease(), client->GetNetmaskFromLease());
        break;
      case DynamicIPEventAction::NAK:
        /* nak: This argument is used with udhcpc receives a NAK message. The script with the deconfig argument will be
         called directly afterwards, so no changes to the network interface are neccessary. This hook is provided for
         purely informational purposes (the message option may contain a reason for the NAK)*/

        break;
      default:
        LogError("IPManager::OnDynamicIPEvent IP Event: found unknown action");
        break;
    }
    hostname_manager_.OnLeaseFileChange(interface);
  }
}

void IPManager::OnHostnameChanged() {
  dyn_ip_client_admin_.RestartAllClients(hostname_manager_.GetHostname());
}

bool IPManager::HasToBePersisted(const IPConfig &ip_config) const {
  auto netdev = netdev_manager_.GetByName(ip_config.interface_);
  if (netdev && IsIncluded(netdev->GetDeviceType(), PersistedDeviceTypes())) {
    return (ip_config.source_ != IPSource::UNKNOWN);
  }
  return false;
}

Status IPManager::Persist(const IPConfigs &config) {
  IPConfigs new_config;
  persistence_provider_.Read(new_config);

  for (auto &config_entry : config) {
    if (HasToBePersisted(config_entry)) {
      AddIPConfig(config_entry, new_config);
    }
  }

  return persistence_provider_.Write(new_config);
}

Status IPManager::ApplyTempFixIpConfiguration() {
  BridgeConfig config;
  Status status = persistence_provider_.Read(config);
  if(status.IsNotOk()){
    return status;
  }

  IPConfigs ip_configs;
  for (const auto &bridge : config) {
    ip_configs.emplace_back(bridge.first.GetName(), IPSource::NONE, ZeroIP, ZeroNetmask);
  };

  SetFixedIps(ip_configs);

  return Configure(ip_configs);
}

Status IPManager::ApplyIpConfiguration(const IPConfigs &config) {
  DipSwitchIpConfig dip_switch_ip_config;
  persistence_provider_.Read(dip_switch_ip_config);

  auto persisted_config = config;
  ComplementNetmasks(persisted_config);
  // Apply config combined with DIP setting and persist IP-Configuration
  auto applied_config = persisted_config;
  ModifyIpConfigByDipSwitch(applied_config, dip_switch_ip_config, ip_dip_switch_.GetState());
  Status status = Apply(applied_config);
  if (status.IsOk()) {
    status = Persist(persisted_config);
  }

  return status;
}

Status IPManager::ApplyDipConfiguration(const DipSwitchIpConfig &dip_switch_ip_config) {
  IPLinkPtr ip_link = GetIPLinkByInterface(Interface::CreateBridge(DIP_SWITCH_BRIDGE));
  IPConfigs applied_ip_config{ip_link->GetIPConfig().value_or(IPConfig{})};
  auto persisted_dip_switch_config = dip_switch_ip_config;
  persisted_dip_switch_config.address_ = IpAddressManipulator::ChangeLastAddressOctet(dip_switch_ip_config.address_, 0);
  ModifyIpConfigByDipSwitch(applied_ip_config, persisted_dip_switch_config, ip_dip_switch_.GetState());

  // Apply ip but do not persist ip configuration.
  // Persist DIP switch IP configuration only.
  Status status = Apply(applied_ip_config);

  if (status.IsOk() && ip_dip_switch_.GetMode() != DipSwitchMode::HW_NOT_AVAILABLE) {
    status = persistence_provider_.Write(persisted_dip_switch_config);
  }

  return status;
}

Status IPManager::ApplyIpConfiguration(const IPConfigs &ip_configs, const DipSwitchIpConfig &dip_switch_ip_config) {
  /*
   * TODO: This Apply is only used for the restore process, it should be renamed.
   * It also should not use the validation path which tests the config against the actual IPs in the system.
   */
  IPConfigs persisted_ipconfig = ip_configs;
  ComplementNetmasks(persisted_ipconfig);

  auto applied_config = persisted_ipconfig;
  ModifyIpConfigByDipSwitch(applied_config, dip_switch_ip_config, ip_dip_switch_.GetState());

  auto itDipIpConfig = GetIpConfigByInterface(applied_config, Interface::CreateBridge(DIP_SWITCH_BRIDGE));
  if (itDipIpConfig != applied_config.end()) {
    DeactivateIpConfigsWithConflicsTo(*itDipIpConfig, applied_config);
  }
  Status status = Apply(applied_config);

  if (status.IsOk()) {
    status = Persist(persisted_ipconfig);
  }

  if (status.IsOk() && ip_dip_switch_.GetMode() != DipSwitchMode::HW_NOT_AVAILABLE) {
    status = persistence_provider_.Write(dip_switch_ip_config);
  }

  return status;
}

Status IPManager::Apply(const IPConfigs &config) {
  Status status = IPValidator::Validate(config, GetIPConfigs(), interface_information_.GetInterfaceInformations());

  if (status.IsOk()) {
    status = Configure(config);
  }

  return status;
}

} /* namespace netconf */
