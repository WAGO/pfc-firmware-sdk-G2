// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"
#include "IBridgeChangeEvent.hpp"

namespace netconf {

class IBridgeManager {
 public:
  IBridgeManager() = default;
  virtual ~IBridgeManager() = default;

  IBridgeManager(const IBridgeManager&) = delete;
  IBridgeManager& operator=(const IBridgeManager&) = delete;
  IBridgeManager(const IBridgeManager&&) = delete;
  IBridgeManager& operator=(const IBridgeManager&&) = delete;

  virtual Status ApplyBridgeConfiguration(BridgeConfig& config) const = 0;

};

} /* namespace netconf */
