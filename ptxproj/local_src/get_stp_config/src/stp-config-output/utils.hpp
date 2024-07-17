// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stp.hpp>

namespace wago::stp::get_stp_config {

void remove_eth_prefix_from_ports(lib::stp_info& config);
void remove_eth_prefix_from_ports(lib::stp_config& info);
void remove_eth_prefix_from_ports(::std::vector<::std::string>& ports);

}  // namespace wago::stp::get_stp_config