// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconf {

class IBridgeConfigurator {
 public:
  IBridgeConfigurator()          = default;
  virtual ~IBridgeConfigurator() = default;

  IBridgeConfigurator(const IBridgeConfigurator&)            = default;
  IBridgeConfigurator& operator=(const IBridgeConfigurator&) = default;
  IBridgeConfigurator(IBridgeConfigurator&&)                 = default;
  IBridgeConfigurator& operator=(IBridgeConfigurator&&)      = default;

  virtual Status Configure(const BridgeConfig& config) const = 0;
  virtual Interfaces GetBridgeAssignedInterfaces() const = 0;
};

} /* namespace netconf */
