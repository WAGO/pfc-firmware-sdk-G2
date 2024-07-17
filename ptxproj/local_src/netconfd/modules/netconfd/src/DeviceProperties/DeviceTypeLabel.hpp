// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IDeviceTypeLabel.hpp"
#include "CommandExecutor.hpp"
#include "MacAddress.hpp"

namespace netconf {

class DeviceTypeLabel : public IDeviceTypeLabel {
 public:
  explicit DeviceTypeLabel(CommandExecutor& command_executer);
  ~DeviceTypeLabel() override = default;

  DeviceTypeLabel(const DeviceTypeLabel&) = delete;
  DeviceTypeLabel& operator=(const DeviceTypeLabel&) = delete;
  DeviceTypeLabel(DeviceTypeLabel&&) = delete;
  DeviceTypeLabel& operator=(DeviceTypeLabel&&) = delete;

  ::std::string GetOrderNumber() const override;
  MacAddress GetMac() const override;
  uint32_t GetMacCount() const override;

 private:

  CommandExecutor& command_executer_;

  ::std::string order_number_;
  MacAddress mac_;
  uint32_t mac_count_;

};

}
