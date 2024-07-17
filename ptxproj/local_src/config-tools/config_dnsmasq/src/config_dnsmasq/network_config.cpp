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

  ::std::array<char,256> buffer{};
  int status = gethostname(buffer.data(), sizeof(buffer));
  if (status != SUCCESS) {
    erh_set_error(SYSTEM_CALL_ERROR, "gethostname failed.", true);
    return "";
  }

  return ::std::string{buffer.data()};
}

} // Anonymous namespace

void netcfg_read_settings(ip_configuration &data, std::vector<std::string> &legal_ports,
                          const prgconf_t &prgconf, bool debugmode) {
  napi::BridgeConfig bridge_config;
  napi::IPConfigs ip_configs;
  napi::InterfaceConfigs interface_configs;
  netconf::InterfaceStatuses interface_statuses;

  ::netconf::Status status;

  if (prgconf.bridge_config.empty() || prgconf.ip_config.empty() || prgconf.interface_config.empty() || prgconf.interface_statuses.empty()) {
    status = napi::GetBridgeConfig(bridge_config);
    erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

    status = napi::GetCurrentIPConfigs(ip_configs);
    erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

    status = napi::GetInterfaceConfigs(interface_configs);
    erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

    status = napi::GetInterfaceStatuses(interface_statuses);
    erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());
  } else {
    status = napi::MakeBridgeConfig(prgconf.bridge_config, bridge_config);
    erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

    status = napi::MakeIPConfigs(prgconf.ip_config, ip_configs);
    erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

    status = napi::MakeInterfaceConfigs(prgconf.interface_config, interface_configs);
    erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());

    status = napi::MakeInterfaceStatuses(prgconf.interface_statuses, interface_statuses);
    erh_assert(status.IsOk(), eStatusCode::INVALID_PARAMETER, status.ToString());
  }

  generate_legal_ports(interface_configs, legal_ports);

  parse_config_parameter(data, legal_ports, debugmode, bridge_config, ip_configs, interface_configs, interface_statuses);
}

void generate_legal_ports(const napi::InterfaceConfigs& interface_configs, ::std::vector<::std::string> &legal_ports) {

  const auto& configs = interface_configs.GetConfig();
  auto count = ::std::count_if(configs.cbegin(), configs.cend(), [](const auto& c) {
    return c.interface_.GetLabel().find('X') == 0; // starts with X, e.g. X1, X12
  });

  for(int i = 0; i < count; i++) {
    legal_ports.push_back("br" + ::std::to_string(i));
  }
}

void parse_config_parameter(configdnsmasq::ip_configuration &ipConfig, std::vector<std::string> &legal_ports, bool debugmode,
                            const napi::BridgeConfig& bridge_config, const napi::IPConfigs& ip_configs,
                            const napi::InterfaceConfigs& interface_configs,
                            const netconf::InterfaceStatuses& interface_statuses) {

  
  ipConfig.host_name = get_hostname(debugmode);

  auto bridges = bridge_config.GetBridges();

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
    pd.setState(bridge, bridge_config, interface_configs);
    pd.setLinkState(bridge, bridge_config, interface_statuses);

    auto optional = ip_configs.GetIPConfig(bridge);
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

} // namespace configdnsmasq
