// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IBridgeConfigurator.hpp"
#include "INetDevManager.hpp"
#include "Types.hpp"
#include <memory>

#include "IBridgeChangeEvent.hpp"

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

namespace netconf {

class BridgeConfiguratorTiSwitch: public IBridgeConfigurator{

 public:
  explicit BridgeConfiguratorTiSwitch(INetDevManager& netdev_manager, IBridgeChangeEvent& bridge_change_event);
  ~BridgeConfiguratorTiSwitch() = default;

  BridgeConfiguratorTiSwitch(const BridgeConfiguratorTiSwitch&) = delete;
  BridgeConfiguratorTiSwitch& operator=(const BridgeConfiguratorTiSwitch&) = delete;
  BridgeConfiguratorTiSwitch(const BridgeConfiguratorTiSwitch&&) = delete;
  BridgeConfiguratorTiSwitch& operator=(const BridgeConfiguratorTiSwitch&&) = delete;

  Status Configure(const BridgeConfig& config) const override;
  Interfaces GetBridgeAssignedInterfaces() const override;

 private:

  Status RemoveAllActualBridgesThatAreNotNeeded(BridgeConfig const &config,
                                                Interfaces &current_bridges) const;
  Status RemoveAllActualBridgeInterfacesThatAreNotNeeded(BridgeConfig const& config_os,
                                                         Interfaces const& current_bridges) const;
  Status AddMissingInterfacesToActualBridges(BridgeConfig const& config_os,
                                             Interfaces const& current_bridges) const;
  Status AddMissingBridgesAndTheirInterfaces(BridgeConfig const& config_os,
                                             Interfaces& current_bridges) const;
  Status SetAllBridgesUp(Interfaces const &bridges) const;

  INetDevManager& netdev_manager_;
  IBridgeChangeEvent& bridge_change_event_;

};

}  // namespace netconf
