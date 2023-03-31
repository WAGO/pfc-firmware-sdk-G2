// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IBridgeConfigurator.hpp"
#include "INetDevManager.hpp"
#include "Types.hpp"
#include <memory>


//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

namespace netconf {

class BridgeConfigurator : public IBridgeConfigurator {

 public:
  explicit BridgeConfigurator(INetDevManager& netdev_manager);
  ~BridgeConfigurator() override = default;

  BridgeConfigurator(const BridgeConfigurator&) = delete;
  BridgeConfigurator& operator=(const BridgeConfigurator&) = delete;
  BridgeConfigurator(const BridgeConfigurator&&) = delete;
  BridgeConfigurator& operator=(const BridgeConfigurator&&) = delete;

  Status Configure(const BridgeConfig& config) const override;
  Interfaces GetBridgeAssignedInterfaces() const;

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

};

}  // namespace netconf
