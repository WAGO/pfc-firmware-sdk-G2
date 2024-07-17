// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "network_config.hpp"

#include <BridgeConfig.hpp>
#include <stp.hpp>
#include "utils.hpp"

using wago::stp::lib::status;

namespace napi = ::netconf::api;

namespace wago::stp::network_config {

status get_bridge_interfaces(const ::std::string& bridge_name, ::std::vector<::std::string>& interfaces) {
  status s;
  napi::BridgeConfig bridge_config;

  auto error = napi::GetBridgeConfig(bridge_config);
  if (error.IsNotOk()) {
    s = status{wago::stp::lib::status_code::SYSTEM_CALL_ERROR, error.ToString()};
  }

  if (s.ok()) {
    interfaces = bridge_config.GetBridgeInterfaces(bridge_name);
    ::wago::stp::get_stp_config::remove_eth_prefix_from_ports(interfaces);
  }

  return s;
}

}  // namespace wago::stp::network_config