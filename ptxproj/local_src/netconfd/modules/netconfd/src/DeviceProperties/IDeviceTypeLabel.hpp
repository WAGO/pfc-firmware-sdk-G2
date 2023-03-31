// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include <cstdint>

#include "Types.hpp"

namespace netconf {

class IDeviceTypeLabel {
 public:

  IDeviceTypeLabel() = default;
  virtual ~IDeviceTypeLabel() = default;
  IDeviceTypeLabel(const IDeviceTypeLabel&) = delete;
  IDeviceTypeLabel& operator=(const IDeviceTypeLabel&) = delete;
  IDeviceTypeLabel(IDeviceTypeLabel&&) = delete;
  IDeviceTypeLabel& operator=(IDeviceTypeLabel&&) = delete;

  virtual ::std::string GetOrderNumber() const = 0;
  virtual MacAddress GetMac() const = 0;
  virtual uint32_t GetMacCount() const = 0;

};

}
