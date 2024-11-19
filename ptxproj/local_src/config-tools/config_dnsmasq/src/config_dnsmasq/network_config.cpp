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
#include "network_config.hpp"

#include <boost/algorithm/string.hpp>
#include <ct_liblog.h>

#include <BridgeConfig.hpp>
#include <IPConfig.hpp>
#include <InterfaceConfig.hpp>

#include "error_handling.hpp"
#include "utilities.hpp"

namespace napi = ::netconf::api;

namespace configdnsmasq {
namespace {

::std::string get_hostname(bool debugmode) {
  if (debugmode) {
    return "debughostname";
  }

  ::std::array<char, 256> buffer{};
  int status = gethostname(buffer.data(), sizeof(buffer));
  if (status != SUCCESS) {
    erh_set_error(SYSTEM_CALL_ERROR, "gethostname failed.", true);
    return "";
  }

  return ::std::string{buffer.data()};
}

void read_network_config_from_program_arguments(const prgconf_t& prgconf, network_config& config) {
  ::netconf::Status status;
  status = napi::MakeBridgeConfig(prgconf.bridge_config, config.bridge_config_);
  erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

  status = napi::MakeIPConfigs(prgconf.ip_config, config.ip_configs_);
  erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

  status = napi::MakeInterfaceConfigs(prgconf.interface_config, config.interface_configs_);
  erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

  status = napi::MakeInterfaceStatuses(prgconf.interface_statuses, config.interface_statuses_);
  erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());
}

void read_network_config_via_netconfd(network_config& config) {
  ::netconf::Status status = napi::GetBridgeConfig(config.bridge_config_);
  erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

  status = napi::GetCurrentIPConfigs(config.ip_configs_);
  erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

  status = napi::GetInterfaceConfigs(config.interface_configs_);
  erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

  status = napi::GetInterfaceStatuses(config.interface_statuses_);
  erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());
}

}  // Anonymous namespace

program_arguments_provisioning is_network_config_provided_via_program_arguments(const prgconf_t& prgconf) {
  ::std::vector<::std::string> empty_config_names{};

  if (prgconf.bridge_config.empty()) {
    empty_config_names.emplace_back("bridge-config");
  }
  if (prgconf.ip_config.empty()) {
    empty_config_names.emplace_back("ip-config");
  }
  if (prgconf.interface_config.empty()) {
    empty_config_names.emplace_back("interface-config");
  }
  if (prgconf.interface_statuses.empty()) {
    empty_config_names.emplace_back("interface-statuses");
  }

  if (empty_config_names.empty()) {
    return COMPLETELY_PROVIDED;
  }
  if (empty_config_names.size() == 4) {
    return NOT_PROVIDED;
  }

  ::std::string missing_configs = boost::algorithm::join(empty_config_names, ", ");
  erh_log("Network config is only provided partially via program arguments. Missing configs: " + missing_configs);

  return PARTIALLY_PROVIDED;
}

void read_network_config(ip_configuration& data, std::vector<std::string>& legal_ports, const prgconf_t& prgconf,
                          bool debugmode) {
  network_config config;

  auto program_args_provisioning = is_network_config_provided_via_program_arguments(prgconf);

  if (program_args_provisioning == COMPLETELY_PROVIDED) {
    read_network_config_from_program_arguments(prgconf, config);
  } else if (program_args_provisioning == NOT_PROVIDED) {
    read_network_config_via_netconfd(config);
  } else {
    erh_set_error(eStatusCode::MISSING_PARAMETER, "Exiting due to missing network configs.");
  }

  generate_legal_ports(config.interface_configs_, legal_ports);

  parse_config_parameter(data, legal_ports, debugmode, config);
}

void generate_legal_ports(const napi::InterfaceConfigs& interface_configs, ::std::vector<::std::string>& legal_ports) {
  const auto& configs = interface_configs.GetConfig();
  auto count          = ::std::count_if(configs.cbegin(), configs.cend(), [](const auto& c) {
    return c.interface_.GetLabel().find('X') == 0;  // starts with X, e.g. X1, X12
  });

  for (int i = 0; i < count; i++) {
    legal_ports.push_back("br" + ::std::to_string(i));
  }
}

void parse_config_parameter(configdnsmasq::ip_configuration& ipConfig, std::vector<std::string>& legal_ports,
                            bool debugmode, const network_config& config) {
  ipConfig.host_name = get_hostname(debugmode);

  auto bridges = config.bridge_config_.GetBridges();

  ::std::string bridge_list_str = boost::algorithm::join(bridges, ",");
  erh_assert(!bridge_list_str.empty(), eStatusCode::INVALID_PARAMETER, "Error: No bridges found.", true);

  // Read port specific parameters, IP-Address, Netmask.
  for (const auto& bridge : bridges) {
    auto it = ::std::find(legal_ports.begin(), legal_ports.end(), bridge);
    if (it == legal_ports.end()) {
      continue;
    }

    port_data pd;
    pd.port_name_ = bridge;
    pd.setState(bridge, config.bridge_config_, config.interface_configs_);
    pd.setLinkState(bridge, config.bridge_config_, config.interface_statuses_);

    auto optional = config.ip_configs_.GetIPConfig(bridge);
    erh_assert(optional.has_value(), eStatusCode::INVALID_PARAMETER,
               ::std::string("Failed to get IP config for bridge ") + bridge, true);

    auto c = *optional;
    pd.setType(c);
    pd.setIpAddress(c.address_);
    pd.setNetmask(c.netmask_);

    ipConfig.port_name_list.push_back(bridge);
    ipConfig.port_data_list.push_back(pd);
  }
}

}  // namespace configdnsmasq
