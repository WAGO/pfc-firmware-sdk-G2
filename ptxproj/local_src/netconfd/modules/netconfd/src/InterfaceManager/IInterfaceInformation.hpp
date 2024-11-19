// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconf {

class IInterfaceInformation {
 public:
  IInterfaceInformation() = default;
  virtual ~IInterfaceInformation() = default;

  IInterfaceInformation(const IInterfaceInformation&) = delete;
  IInterfaceInformation& operator=(const IInterfaceInformation&) = delete;
  IInterfaceInformation(const IInterfaceInformation&&) = delete;
  IInterfaceInformation& operator=(const IInterfaceInformation&&) = delete;

  virtual InterfaceConfigs const& GetPortConfigs() = 0;
  virtual Status GetCurrentPortStatuses(InterfaceStatuses& itf_statuses) = 0;
  virtual InterfaceInformations GetInterfaceInformations() = 0;

};

}
