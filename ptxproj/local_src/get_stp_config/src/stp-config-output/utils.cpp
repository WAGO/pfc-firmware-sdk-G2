// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "utils.hpp"

namespace {

void remove_eth_prefix(::std::string& port_name) {
  if (port_name.rfind("ethX", 0) == 0) {
    port_name = port_name.substr(3);
  }
}

}  // namespace

namespace wago::stp::get_stp_config {

void remove_eth_prefix_from_ports(lib::stp_config& config) {
  for (auto& port : config.port_configs) {
    remove_eth_prefix(port.port);
  }
}

void remove_eth_prefix_from_ports(lib::stp_info& info) {
  for (auto& port : info.ports) {
    remove_eth_prefix(port.port);
  }
}

void remove_eth_prefix_from_ports(::std::vector<::std::string>& ports) {
  for (auto& port : ports) {
    remove_eth_prefix(port);
  }
}

}  // namespace wago::stp::get_stp_config