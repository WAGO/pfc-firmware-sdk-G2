// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stp.hpp>
#include <string>
#include <vector>

namespace wago::stp::network_config {

wago::stp::lib::status get_bridge_interfaces(const ::std::string &bridge_name,
                                             ::std::vector<::std::string> &interfaces);

}  // namespace wago::stp::network_config