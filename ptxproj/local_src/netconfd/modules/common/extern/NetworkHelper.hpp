// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>
#include "BaseTypes.hpp"

namespace netconf {

::std::string GetPrefix(const ::std::string& netmask);

Netmask GetNetmask(const uint32_t& prefix);

Address GetBroadcast(const Address& address, const Netmask& netmask);

}
