// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <cstdint>
#include <string>
#include "IIPEvent.hpp"

namespace netconf {

class IIPMonitor {
 public:
  IIPMonitor() = default;
  virtual ~IIPMonitor() = default;

  IIPMonitor(const IIPMonitor &other) = delete;
  IIPMonitor& operator=(const IIPMonitor &other) = delete;
  IIPMonitor(IIPMonitor &&other) = delete;
  IIPMonitor& operator=(IIPMonitor &&other) = delete;

  virtual Address GetIPAddress(int if_index)= 0;
  virtual Netmask GetNetmask(int if_index) = 0;

  virtual void RegisterEventHandler(IIPEvent& event_handler) = 0;
  virtual void UnregisterEventHandler(IIPEvent& event_handler) = 0;
};

}
