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
///  \brief    Reading of network information (ethernet, ip and interfaces).
///
///  \author   MSc, WAGO GmbH & Co. KG.
///  \author   MOe, WAGO GmbH & Co. KG.
//------------------------------------------------------------------------------

#ifndef SRC_CONFIG_DNSMASQ_NETWORK_CONFIG_HPP_
#define SRC_CONFIG_DNSMASQ_NETWORK_CONFIG_HPP_

#include <string>
#include <vector>

#include "defines.hpp"
#include "ip_configuration.hpp"

namespace configdnsmasq {

void netcfg_read_settings(ip_configuration &data, std::vector<std::string> &legal_ports, const prgconf_t &prgconf,
                          bool debugmode);

void generate_legal_ports(const napi::InterfaceConfigs &interface_configs, ::std::vector<::std::string> &legal_ports);
void parse_config_parameter(ip_configuration &ipConfig, std::vector<std::string> &legal_ports, bool debugmode,
                            const napi::BridgeConfig& bridge_config, const napi::IPConfigs& ip_configs,
                            const napi::InterfaceConfigs& interface_configs,
                            const netconf::InterfaceStatuses& interface_statuses);

}  // namespace configdnsmasq

#endif /* SRC_CONFIG_DNSMASQ_NETWORK_CONFIG_HPP_ */
