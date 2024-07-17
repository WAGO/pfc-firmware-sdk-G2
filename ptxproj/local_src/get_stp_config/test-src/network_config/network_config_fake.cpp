// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include <stp.hpp>

using wago::stp::lib::status;

namespace wago::stp::network_config {

status get_bridge_interfaces(const ::std::string& bridge_name, ::std::vector<::std::string>& interfaces) {
  (void)bridge_name;
  status s;

  interfaces.clear();

  if (bridge_name == "one_interface") {
    interfaces.emplace_back("X1");
  } else if (bridge_name == "two_interfaces") {
    interfaces.emplace_back("X1");
    interfaces.emplace_back("X2");
  }

  return s;
}

}  // namespace wago::stp::network_config