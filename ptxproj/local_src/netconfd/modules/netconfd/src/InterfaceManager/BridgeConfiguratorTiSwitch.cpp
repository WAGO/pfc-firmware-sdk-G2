// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeConfiguratorTiSwitch.hpp"

#include <algorithm>
#include <cstddef>

#include "CollectionUtils.hpp"
#include "CommandExecutor.hpp"
#include "Logger.hpp"

namespace netconf {

BridgeConfiguratorTiSwitch::BridgeConfiguratorTiSwitch(INetDevManager &netdev_manager, IBridgeChangeEvent &bridge_change_event)
    : netdev_manager_{netdev_manager}, bridge_change_event_{bridge_change_event} {
      LogDebug("use pfc300 specific BridgeConfigurator");
}

Status BridgeConfiguratorTiSwitch::RemoveAllActualBridgesThatAreNotNeeded(BridgeConfig const &config,
                                                                  Interfaces &current_bridges) const {
  Status status;
  current_bridges.clear();

  Interfaces bridges = netdev_manager_.GetBridgesWithAssignetPort();

  for (const auto &bridge : bridges) {
    if (config.find(bridge) == config.end()) {
      bridge_change_event_.OnBridgeRemove(bridge);
      netdev_manager_.SetDown(bridge);
      netdev_manager_.Delete(bridge);
    } else {
      current_bridges.push_back(bridge);
    }
  }
  return status;
}

Status BridgeConfiguratorTiSwitch::RemoveAllActualBridgeInterfacesThatAreNotNeeded(BridgeConfig const &config,
                                                                           Interfaces const &current_bridges) const {
  Status status;
  for (const auto &actual_bridge : current_bridges) {
    Interfaces actual_port_interfaces = netdev_manager_.GetPorts(actual_bridge);
    Interfaces const &port_interfaces = config.at(actual_bridge);
    for (const auto &port_interface : actual_port_interfaces) {
      if (IsNotIncluded(port_interface, port_interfaces)) {
        netdev_manager_.BridgePortLeave(port_interface);
        bridge_change_event_.OnBridgeAddOrPortChange(actual_bridge, port_interfaces);
      }
    }
  }
  return status;
}

Status BridgeConfiguratorTiSwitch::AddMissingInterfacesToActualBridges(BridgeConfig const &config,
                                                               Interfaces const &current_bridges) const {
  Status status;
  for (const auto &bridge_interface : current_bridges) {
    Interfaces actual_interfaces      = netdev_manager_.GetPorts(bridge_interface);
    Interfaces const &port_interfaces = config.at(bridge_interface);

    for (const auto &port_interface : port_interfaces) {
      if (IsNotIncluded(port_interface, actual_interfaces)) {
        status = netdev_manager_.BridgePortJoin(port_interface, bridge_interface);
        if (status.IsNotOk()) {
          break;
        }
      }
    }
  }
  return status;
}

Status BridgeConfiguratorTiSwitch::AddMissingBridgesAndTheirInterfaces(BridgeConfig const &config,
                                                               Interfaces &current_bridges) const {
  Status status;
  for (const auto &[bridge, port_interfaces] : config) {
    auto bridge_interface = bridge;
    if (IsNotIncluded(bridge_interface, current_bridges)) {
      status = netdev_manager_.AddInterface(bridge_interface);
      current_bridges.emplace_back(bridge_interface);
      if (status.IsNotOk()) {
        break;
      }
      for (const auto &port_interface : port_interfaces) {
        status = netdev_manager_.BridgePortJoin(port_interface, bridge_interface);
        if (status.IsNotOk()) {
          break;
        }
      }
      bridge_change_event_.OnBridgeAddOrPortChange(bridge_interface, port_interfaces);
    }
  }
  return status;
}

Status BridgeConfiguratorTiSwitch::SetAllBridgesUp(Interfaces const &bridges) const {
  Status status;
  for (auto &bridge : bridges) {
    netdev_manager_.SetUp(bridge);
  }
  return status;
}

namespace {
  void PFC300QickfixChangeSwitchMode(INetDevManager& netdev_manager, const BridgeConfig& config){

    netdev_manager.BridgePortLeave(Interface::CreatePort("ethX1"));
    netdev_manager.BridgePortLeave(Interface::CreatePort("ethX2"));

    // INFO Ports müssen up sein damit sie beim hinzufügen der bridge in die untagged vid1 liste aufgenommen werden
    netdev_manager.SetUp(Interface::CreatePort("ethX1"));
    netdev_manager.SetUp(Interface::CreatePort("ethX2"));

    if(config.size() == 1){
      ::std::system("/usr/sbin/devlink dev param set platform/8000000.ethernet name switch_mode value true cmode runtime");
    } else {
      ::std::system("/usr/sbin/devlink dev param set platform/8000000.ethernet name switch_mode value false cmode runtime");
    }
  }
}

Status BridgeConfiguratorTiSwitch::Configure(const BridgeConfig &config) const {
  Interfaces current_bridges;
  Status status = RemoveAllActualBridgesThatAreNotNeeded(config, current_bridges);
  LOG_STATUS(status);

  if (status.IsOk()) {
    status = RemoveAllActualBridgeInterfacesThatAreNotNeeded(config, current_bridges);
    LOG_STATUS(status);
  }

  PFC300QickfixChangeSwitchMode(netdev_manager_,config);

  if (status.IsOk()) {
    status = AddMissingInterfacesToActualBridges(config, current_bridges);
    LOG_STATUS(status);
  }
  if (status.IsOk()) {
    status = AddMissingBridgesAndTheirInterfaces(config, current_bridges);
    LOG_STATUS(status);
  }

  if (status.IsOk()) {
    status = SetAllBridgesUp(current_bridges);
    LOG_STATUS(status);
  }
  return status;
}

Interfaces BridgeConfiguratorTiSwitch::GetBridgeAssignedInterfaces() const {
  Interfaces interfaces;
  Interfaces bridges = netdev_manager_.GetInterfacesByDeviceType({DeviceType::Bridge});
  for (const auto &bridge : bridges) {
    interfaces = Concatenate(interfaces, netdev_manager_.GetPorts(bridge));
  }
  return interfaces;
}

}  // namespace netconf

//---- End of source file ------------------------------------------------------
