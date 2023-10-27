// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "NetworkConfiguratorSettings.hpp"
#include "IPersistenceProvider.hpp"
#include "IEventManager.hpp"
#include "IDipSwitch.hpp"
#include "InterfaceConfigManager.hpp"
#include "IBridgeManager.hpp"
#include "IIPManager.hpp"
#include "JsonConverter.hpp"
#include "IHostnameWillChange.hpp"

namespace netconf {

class IInterfaceMonitor;
class IInterfaceInformation;
class ITaskQueue;
class INetDevManager;

class NetworkConfigBrain {
 public:
  NetworkConfigBrain(IBridgeManager &bridge_manager, IBridgeInformation &itf_info, IIPManager &ip_manager,
                     IEventManager &event_manager, IPersistenceProvider &persistence_provider, IDipSwitch &ip_dip_switch,
                     InterfaceConfigManager &interface_config_manager, INetDevManager &netdev_manager,
                     IHostnameWillChange &hostname_manager);

  virtual ~NetworkConfigBrain() = default;

  NetworkConfigBrain(const NetworkConfigBrain&) = delete;
  NetworkConfigBrain& operator=(const NetworkConfigBrain&) = delete;
  NetworkConfigBrain(const NetworkConfigBrain&&) = delete;
  NetworkConfigBrain& operator=(const NetworkConfigBrain&&) = delete;

  void Start(StartWithPortstate startwithportstate);

  ::std::string SetBridgeConfig(::std::string const &config);
  ::std::string GetBridgeConfig(::std::string& config) const;
  ::std::string GetInterfaceInformation(::std::string& config) const;
  ::std::string SetInterfaceConfig(::std::string const &config);
  ::std::string GetInterfaceConfig(::std::string& config) const;
  ::std::string GetInterfaceStatuses(::std::string& config) const;

  ::std::string GetDipSwitchConfig(::std::string& config) const;
  ::std::string GetAllIPConfigs(::std::string& config) const;
  ::std::string GetCurrentIPConfigs(::std::string& config) const;
  ::std::string SetDipSwitchConfig(const ::std::string& config);
  ::std::string SetAllIPConfigs(const ::std::string& config);
  ::std::string SetIPConfig(const ::std::string& config);

  ::std::string GetBackupParamterCount() const;
  ::std::string Backup(const ::std::string &file_path, const ::std::string &targetversion) const;
  ::std::string Restore(const std::string &file_path);

  ::std::string SetTemporaryFixIp();
  
  ::std::string ReceiveDynamicIPEvent(const ::std::string& event);
  ::std::string ReceiveReloadHostConfEvent();

  ::std::string AddInterface(const ::std::string& data);
  ::std::string DeleteInterface(const ::std::string& data);

 private:
  bool HasToApplyDipSwitchConfig() const;
  Status ApplyConfig(BridgeConfig &bridge_config, const Interfaces& interfaces, const IPConfigs &ip_configs,
                     const DipSwitchIpConfig &dip_ip_config) const;
  Status ApplyFallbackConfig();
  Status StopDynamicIPClientsOfBridgesToBeRemoved(const BridgeConfig &product_config) const;
  void RemoveUnsupportedPorts(BridgeConfig &config, Interfaces &supported_interfaces) const;
  Status RemoveUnnecessaryInterfaces(const Interfaces &current_interfaces, const Interfaces &interfaces) const;
  Status ConfigureInterfaces(const Interfaces& interfaces, const Interfaces& current_interfaces) const;
  Status AddMissingInterfaces(const Interfaces &current_interfaces, const Interfaces &interfaces) const;
  void FilterRequiredIpConfigs(IPConfigs &ip_configs, const Interfaces& interfaces) const;
  IPConfigs FilterRequiredIPConfigs(const IPConfigs &ip_configs, const BridgeConfig &product_config) const;
  bool HasToBePersisted(const IPConfig &ip_config) const;

  Status ContainOnlyInterfacesOfTypePort(const InterfaceConfigs &interface_configs) const;

  void ResetBridgeConfigToDefault(Interfaces product_interfaces, BridgeConfig &config);
  void ResetIpConfigsToDefault(IPConfigs &configs);
  void ResetDIPSwitchIPConfigToDefault(DipSwitchIpConfig &config);

  void GetValidIpConfigsSubset(const IPConfigs &configs, IPConfigs &subset);


  Status PersistIpConfiguration(const IPConfigs &config);
  JsonConverter jc;
  IBridgeManager &bridge_manager_;
  IBridgeInformation &bridge_information_;
  InterfaceConfigManager &interface_config_manager_;
  IIPManager &ip_manager_;
  IEventManager &event_manager_;
  IPersistenceProvider &persistence_provider_;
  INetDevManager &netdev_manager_;
  IDipSwitch &ip_dip_switch_;
  IHostnameWillChange &hostname_will_change_;

};



} /* namespace netconf */
