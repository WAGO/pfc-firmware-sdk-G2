// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>

#include "BridgeConfigurator.hpp"
#include "IBridgeInformation.hpp"
#include "IBridgeManager.hpp"
#include "IMacDistributor.hpp"
#include "INetDevManager.hpp"

namespace netconf {

class BridgeManager : public IBridgeManager, public IBridgeInformation {
 public:
  BridgeManager(INetDevManager& netdev_manager, IMacDistributor& mac_distributor,
                IBridgeChangeEvent& bridge_change_event, const ::std::string& order_number);
  ~BridgeManager() override = default;

  BridgeManager(const BridgeManager&)             = delete;
  BridgeManager& operator=(const BridgeManager&)  = delete;
  BridgeManager(const BridgeManager&&)            = delete;
  BridgeManager& operator=(const BridgeManager&&) = delete;

  Interfaces GetBridgeAssignedInterfaces() const override;
  Status ApplyBridgeConfiguration(BridgeConfig& config) const override;

 private:
  INetDevManager& netdev_manager_;
  IMacDistributor& mac_distributor_;
  ::std::shared_ptr<IBridgeConfigurator> bridge_configurator_;
};

} /* namespace netconf */
