// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeManager.hpp"

#include "BridgeConfigValidator.hpp"
#include "Logger.hpp"

namespace netconf {

using namespace std::literals;

namespace {

bool ContainsBridge(BridgeConfig &config, const Interface& itf) {
  return config.count(itf) > 0;
}

bool ContainsPort(BridgeConfig &config, const Interface& itf) {
  return ::std::any_of(config.begin(), config.end(), [&](auto &bridge) {
    return ::std::count(bridge.second.begin(), bridge.second.end(), itf);
  }
  );
}

bool ContainsNetdevInterfaceAsBridgeOrPort(BridgeConfig& config, NetDevPtr& netdev){
  return ContainsBridge(config, netdev->GetInterface()) or ContainsPort(config, netdev->GetInterface());
}

}

BridgeManager::BridgeManager(INetDevManager &netdev_manager, IMacDistributor &mac_distributor)
    : netdev_manager_ { netdev_manager },
      mac_distributor_ { mac_distributor },
      bridge_configurator_(netdev_manager) {
}

Status BridgeManager::ApplyBridgeConfiguration(BridgeConfig &config) const {
  Status status = BridgeConfigValidator::Validate(config,
                                                  netdev_manager_.GetInterfacesByDeviceType( { DeviceType::Port }));
  LOG_DEBUG("IsValid status: " << status.ToString());
  if (status.IsNotOk()) {
    return status;
  }

  status = bridge_configurator_.Configure(config);
  LOG_DEBUG("Configure status: " << status.ToString());

  auto netdevs = netdev_manager_.GetNetDevs();

  //It should only be assigned MACs to the Bridges and Ports that are configured here.
  //Other bridges in the system e.g. docker0 bridges should be ignored!
  NetDevs configured_netdevs;
  for (auto &netdev : netdevs) {
    if (ContainsNetdevInterfaceAsBridgeOrPort(config, netdev)) {
      configured_netdevs.emplace_back(netdev);
    }
    mac_distributor_.AssignMacs(configured_netdevs);
  }
  return status;

}

Interfaces BridgeManager::GetBridgeAssignedInterfaces() const {
  return bridge_configurator_.GetBridgeAssignedInterfaces();
}

} /* namespace netconf */
