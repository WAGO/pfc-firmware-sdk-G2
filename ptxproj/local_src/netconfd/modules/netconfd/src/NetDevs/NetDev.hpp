// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <linux/if.h>

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "BaseTypes.hpp"
#include "DeviceType.hpp"
#include "IInterfaceMonitor.hpp"
#include "Logger.hpp"
#include "NetworkInterfaceConstants.hpp"

namespace netconf {

class NetDev;

using NetDevPtr = ::std::shared_ptr<NetDev>;
using NetDevs   = ::std::vector<NetDevPtr>;

class NetDev {
 public:
  NetDev() = delete;
  explicit NetDev(LinkInfo attributes);

  Interface GetInterface() const;
  int GetIndex() const;
  int GetParentIndex() const;
  DeviceType GetDeviceType() const;
  MacAddress GetMac() const;
  ::std::string GetName() const;
  eth::InterfaceLinkState GetLinkState() const;

  void SetLinkInfo(LinkInfo attributes);
  LinkInfo GetLinkInfo();

  bool IsLinkStateUp() const;

 private:
  LinkInfo attributes_;
};

}  // namespace netconf
