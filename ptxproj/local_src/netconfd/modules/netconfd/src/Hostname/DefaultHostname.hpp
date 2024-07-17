// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include "MacAddress.hpp"

namespace netconf {

::std::string GetDefaultHostname(const MacAddress& mac_address);

//only for internal testing
::std::string GetDefaultHostname(const ::std::string& board_variant, const ::std::string& base_mac);

} /* namespace netconf */
