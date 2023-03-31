// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IBridgeManager.hpp"

#include <memory>

#include "BridgeConfigurator.hpp"
#include "INetDevManager.hpp"
#include "IMacDistributor.hpp"
#include "IBridgeInformation.hpp"

namespace netconf {

class BridgeManager : public IBridgeManager, public IBridgeInformation{
 public:
  BridgeManager(INetDevManager& netdev_manager, IMacDistributor& mac_distributor);
  ~BridgeManager() override = default;

  BridgeManager(const BridgeManager&) = delete;
  BridgeManager& operator=(const BridgeManager&) = delete;
  BridgeManager(const BridgeManager&&) = delete;
  BridgeManager& operator=(const BridgeManager&&) = delete;

  Interfaces GetBridgeAssignedInterfaces() const override;
  Status ApplyBridgeConfiguration(BridgeConfig& config) const override;

 private:
  INetDevManager& netdev_manager_;
  IMacDistributor& mac_distributor_;
  BridgeConfigurator bridge_configurator_;

};

} /* namespace netconf */
