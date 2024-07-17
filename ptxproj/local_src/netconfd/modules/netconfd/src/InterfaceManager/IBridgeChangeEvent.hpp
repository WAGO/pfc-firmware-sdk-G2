// SPDX-License-Identifier: GPL-2.0-or-later

#include "BaseTypes.hpp"
#include "Status.hpp"
#include "Types.hpp"

#pragma once

namespace netconf {

class IBridgeChangeEvent {
 public:
  IBridgeChangeEvent()          = default;
  virtual ~IBridgeChangeEvent() = default;

  IBridgeChangeEvent(const IBridgeChangeEvent&)             = delete;
  IBridgeChangeEvent& operator=(const IBridgeChangeEvent&)  = delete;
  IBridgeChangeEvent(const IBridgeChangeEvent&&)            = delete;
  IBridgeChangeEvent& operator=(const IBridgeChangeEvent&&) = delete;

  virtual void OnBridgeRemove(const Interface& bridge) const = 0;
  virtual void OnBridgeAddOrPortChange(const Interface& bridge, const Interfaces& ports) const = 0;
};

}  // namespace netconf
