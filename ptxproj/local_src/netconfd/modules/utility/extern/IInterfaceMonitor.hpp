// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include "LinkInfo.hpp"
#include "IInterfaceEvent.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <optional>

namespace netconf {

class IInterfaceMonitor {
 public:
  IInterfaceMonitor() = default;
  virtual ~IInterfaceMonitor() = default;

  IInterfaceMonitor(const IInterfaceMonitor &other) = delete;
  IInterfaceMonitor& operator=(const IInterfaceMonitor &other) = delete;
  IInterfaceMonitor(IInterfaceMonitor &&other) = delete;
  IInterfaceMonitor& operator=(IInterfaceMonitor &&other) = delete;

  virtual void RegisterEventHandler(IInterfaceEvent& event_handler) = 0;
  virtual void UnregisterEventHandler(IInterfaceEvent& event_handler) = 0;

  virtual ::std::uint32_t GetIffFlags(::std::uint32_t if_index) = 0;
  virtual ::std::int32_t GetAddressFamily(::std::uint32_t if_index) = 0;

};


}
