// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeManager.hpp"
#include <memory>

#include "BridgeConfigValidator.hpp"
#include "BridgeConfigurator.hpp"
#include "BridgeConfiguratorTiSwitch.hpp"
#include "Logger.hpp"
#include "NetDev.hpp"

namespace netconf {

namespace{

bool ContainsBridge(BridgeConfig &config, const Interface &itf) {
  return config.count(itf) > 0;
}

bool ContainsPort(BridgeConfig &config, const Interface &itf) {
  return ::std::any_of(config.begin(), config.end(),
                       [&](auto &bridge) { return ::std::count(bridge.second.begin(), bridge.second.end(), itf); });
}

bool ContainsNetdevInterfaceAsBridgeOrPort(BridgeConfig &config, NetDevPtr &netdev) {
  return ContainsBridge(config, netdev->GetInterface()) or ContainsPort(config, netdev->GetInterface());
}

NetDevs GetConfiguredBridgeNetdevs(BridgeConfig &config, NetDevs &netdevs){
  NetDevs configured_netdevs;
  for (auto &netdev : netdevs) {
    if (ContainsNetdevInterfaceAsBridgeOrPort(config, netdev)) {
      configured_netdevs.emplace_back(netdev);
    }
  }
  return configured_netdevs;
}

}

using namespace std::literals;



BridgeManager::BridgeManager(INetDevManager &netdev_manager, IMacDistributor &mac_distributor,
                             IBridgeChangeEvent &bridge_change_event, const ::std::string& order_number)
    : netdev_manager_{netdev_manager},
      mac_distributor_{mac_distributor} {
        if(order_number == "750-8302"){
          bridge_configurator_ = ::std::make_shared<BridgeConfiguratorTiSwitch>(netdev_manager, bridge_change_event);
        }else{
          bridge_configurator_ = ::std::make_shared<BridgeConfigurator>(netdev_manager, bridge_change_event);
        }
}

Status BridgeManager::ApplyBridgeConfiguration(BridgeConfig &config) const {
  Status status =
      BridgeConfigValidator::Validate(config, netdev_manager_.GetInterfacesByDeviceType({DeviceType::Port}));
  LOG_DEBUG("IsValid status: " << status.ToString());
  if (status.IsNotOk()) {
    return status;
  }

  status = bridge_configurator_->Configure(config);
  LOG_DEBUG("Configure status: " << status.ToString());


  // It should only be assigned MACs to the Bridges and Ports that are configured here.
  // Other bridges in the system e.g. docker0 bridges should be ignored!
  auto netdevs = netdev_manager_.GetNetDevs();
  NetDevs configured_netdevs = GetConfiguredBridgeNetdevs(config, netdevs);
  mac_distributor_.AssignMacs(configured_netdevs);

  return status;
}

Interfaces BridgeManager::GetBridgeAssignedInterfaces() const {
  return bridge_configurator_->GetBridgeAssignedInterfaces();
}

} /* namespace netconf */
