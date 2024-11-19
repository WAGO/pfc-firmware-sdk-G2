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

enum program_arguments_provisioning { COMPLETELY_PROVIDED, PARTIALLY_PROVIDED, NOT_PROVIDED };

struct network_config {
  napi::BridgeConfig bridge_config_;
  napi::IPConfigs ip_configs_;
  napi::InterfaceConfigs interface_configs_;
  netconf::InterfaceStatuses interface_statuses_;
};

program_arguments_provisioning is_network_config_provided_via_program_arguments(const prgconf_t& prgconf);
void read_network_config(ip_configuration &data, std::vector<std::string> &legal_ports, const prgconf_t &prgconf,
                         bool debugmode);

void generate_legal_ports(const napi::InterfaceConfigs &interface_configs, ::std::vector<::std::string> &legal_ports);
void parse_config_parameter(configdnsmasq::ip_configuration& ipConfig, std::vector<std::string>& legal_ports,
                            bool debugmode, const network_config& config);

}  // namespace configdnsmasq

#endif /* SRC_CONFIG_DNSMASQ_NETWORK_CONFIG_HPP_ */
