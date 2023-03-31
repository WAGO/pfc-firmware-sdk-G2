// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>
#include <vector>

#include "BaseTypes.hpp"
#include "IInterfaceMonitor.hpp"

namespace netconf {

class LinkHelper {
 public:
  static Interface LinkToInterface(const LinkInfo &link_info);
};

} /* namespace netconf */
