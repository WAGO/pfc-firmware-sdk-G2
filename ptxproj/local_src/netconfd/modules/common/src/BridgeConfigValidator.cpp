// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeConfigValidator.hpp"

#include <net/if.h>
#include <algorithm>

#include "CollectionUtils.hpp"

namespace netconf {

static Status CheckBridgeName(const ::std::string &bridge_name) {
  // Hint: A check for duplicated bridges is not required, because we use a map that
  // does not allow duplicate keys.

  // Check bridge name length
  if (bridge_name.size() > IFNAMSIZ - 1) {
    return Status{ StatusCode::BRIDGE_NAME_INVALID, bridge_name };
  }
  // Checks bridge contains blanks
  if (::std::string::npos != bridge_name.find(' ')) {
    return Status{ StatusCode::BRIDGE_NAME_INVALID, bridge_name };
  }

  return {};
}

static Status CheckBridgeInterfaces(const Interfaces &port_interfaces, const Interfaces &available_interfaces,
                                    Interfaces &former_bridge_interfaces) {
  for (auto &port_interface : port_interfaces) {
    // Check if interface is available in the system
    if (IsNotIncluded(port_interface, available_interfaces)) {
      return Status { StatusCode::INTERFACE_NOT_EXISTING, port_interface.GetName() };
    }

    // Check if interfaces are uniquely assigned to bridge(s)
    if (IsNotIncluded(port_interface, former_bridge_interfaces)) {
      former_bridge_interfaces.push_back(port_interface);
    } else {
      return Status { StatusCode::ENTRY_DUPLICATE, port_interface.GetName() };
    }
  }
  return {};
}

Status BridgeConfigValidator::Validate(BridgeConfig const &config, const Interfaces &available_interfaces) {
  Status status;
  Interfaces found_interfaces;
  if (config.empty()) {
    status.Set(StatusCode::JSON_INCOMPLETE);
  } else {
    for (auto& [bridge_name, port_interfaces] : config) {
      status = CheckBridgeName(bridge_name.GetName());
      if (status.IsNotOk()) {
        break;
      }
      status = CheckBridgeInterfaces(port_interfaces, available_interfaces, found_interfaces);
      if (status.IsNotOk()) {
        break;
      }
    }
    //check if at least one port has been added to a bridge. Otherwise the device can no longer be reached.
    if(found_interfaces.empty()){
      status.Set(StatusCode::JSON_INCOMPLETE);
    }
  }
  return status;
}

} /* namespace netconf */
