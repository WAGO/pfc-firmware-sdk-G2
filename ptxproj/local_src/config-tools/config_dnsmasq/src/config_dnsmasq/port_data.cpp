//------------------------------------------------------------------------------
/// Copyright (c) 2020-2024 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Class representation of a port configuration
///
///  \author   MSc : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include "port_data.hpp"

#include <Types.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <optional>

#include "error_handling.hpp"
#include "utilities.hpp"

namespace configdnsmasq {

namespace {

std::optional<netconf::InterfaceStatus> GetInterfaceStatus(const netconf::InterfaceStatuses& statuses, const std::string& name) {
  auto it = find_if(statuses.cbegin(), statuses.cend(), [& name](const netconf::InterfaceStatus& status){
    return status.interface_.GetName() == name;
  });

  if(it != statuses.cend()) {
    return *it;
  }
  return std::nullopt;
}

}

port_data::port_data()
    :
    port_name_ { "" },
    state_ { "" },
    link_state_ { netconf::LinkState::UNKNOWN },
    type_ { "" },
    ip_addr_ { "0.0.0.0" },
    netmask_ { "0.0.0.0" },
    ip_addr_bin_ { 0 },
    netmask_bin_ { 0 },
    network_bin_ { 0 } {
}

void port_data::setState(const std::string &bridge, const napi::BridgeConfig &bridgeConfig,
                        napi::InterfaceConfigs interfaceConfig) {
  bool bridgeInterfacesDown = areAllBridgeInterfacesDown(bridge, bridgeConfig, interfaceConfig);
  state_ = bridgeInterfacesDown ? "disabled" : "enabled";
}

void port_data::setLinkState(const std::string &bridge, const napi::BridgeConfig &bridgeConfig,
                            const netconf::InterfaceStatuses& interfaceStatuses) {
  bool bridgeInterfacesDown = areAllBridgeInterfacesLowerDown(bridge, bridgeConfig, interfaceStatuses);
  link_state_ = bridgeInterfacesDown ? netconf::LinkState::DOWN : netconf::LinkState::UP;
}

void port_data::setIpAddress(const std::string &address) {
  ip_addr_ = address;
  auto status = conv_ip_ascii2bin(ip_addr_bin_, address);
  erh_assert(SUCCESS == status, INVALID_PARAMETER,
      boost::format("The ip address %s is invalid and cannot be converted to a binary representation") % address);

  network_bin_ = ip_addr_bin_ & netmask_bin_;
}

void port_data::setNetmask(const std::string &netmask) {
  netmask_ = netmask;
  auto status = conv_ip_ascii2bin(netmask_bin_, netmask);
  erh_assert(SUCCESS == status, INVALID_PARAMETER,
      boost::format("The netmask %s is invalid and cannot be converted to a binary representation") % netmask);

  network_bin_ = ip_addr_bin_ & netmask_bin_;
}

void port_data::setType(const netconf::IPConfig &ip_config) {
  switch (ip_config.source_) {
    case netconf::IPSource::STATIC:
      type_ = "static";
      break;
    case netconf::IPSource::DHCP:
      type_ = "dhcp";
      break;
    case netconf::IPSource::BOOTP:
      type_ = "bootp";
      break;
    default:
      type_ = "unknown";
      break;
  }
}

// We do not check lower-down state here.
bool port_data::areAllBridgeInterfacesDown(
  const std::string &bridge_name,
  const napi::BridgeConfig& bridge_config,
  napi::InterfaceConfigs& interface_config) {

  auto interface_names = bridge_config.GetBridgeInterfaces(bridge_name);
  auto up = find_if(interface_names.begin(), interface_names.end(), [& interface_config](const ::std::string& name) {
    auto optional_interface = interface_config.GetInterfaceConfig(name);
    return optional_interface ? optional_interface->state_ == netconf::InterfaceState::UP : false;
  });

  return up == interface_names.end();
}

bool port_data::areAllBridgeInterfacesLowerDown(
  const std::string &bridge_name,
  const napi::BridgeConfig& bridge_config,
  const netconf::InterfaceStatuses& interface_statuses) {

  auto interface_names = bridge_config.GetBridgeInterfaces(bridge_name);
  auto up = find_if(interface_names.begin(), interface_names.end(), [& interface_statuses](const ::std::string& name) {
    auto optional_interface = GetInterfaceStatus(interface_statuses, name);
    return optional_interface ? optional_interface->link_state_ == netconf::LinkState::UP : false;
  });

  return up == interface_names.end();
}

bool port_data::has_valid_ip_config() {
    return (!ip_addr_.empty()
        && ip_addr_ != "0.0.0.0"
        && !netmask_.empty()
        && netmask_ != "0.0.0.0" );
}

} // namespace configdnsmasq
