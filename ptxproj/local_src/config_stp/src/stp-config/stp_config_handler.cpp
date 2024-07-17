// Copyright (c) 2024 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "stp_config_handler.hpp"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <nlohmann/json.hpp>
#include <set>
#include <string>

#include "stp.hpp"

namespace wago::stp::lib {
NLOHMANN_JSON_SERIALIZE_ENUM(protocol_version, {
                                                   {protocol_version::STP, "stp"},
                                                   {protocol_version::RSTP, "rstp"},
                                                   {protocol_version::MSTP, "mstp"},
                                               })
}

namespace wago::stp::config_stp {

using wago::stp::lib::stp_config;
using wago::stp::lib::stp_port_config;

namespace {

constexpr auto version_key         = "version";
constexpr auto stp_enabled_key     = "enabled";
constexpr auto protocol_key        = "protocol";
constexpr auto bridge_key          = "bridge";
constexpr auto bridge_priority_key = "priority";
constexpr auto max_age_key         = "max-age";
constexpr auto max_hops_key        = "max-hops";
constexpr auto forward_delay_key   = "forward-delay";
constexpr auto hello_time_key      = "hello-time";
constexpr auto port_configs_key    = "port-configs";
constexpr auto port_priority_key   = "priority";
constexpr auto path_cost_key       = "path-cost";
constexpr auto bpdu_guard_key      = "bpdu-guard";
constexpr auto bpdu_filter_key     = "bpdu-filter";
constexpr auto edge_port_key       = "edge-port";
constexpr auto root_guard_key      = "root-guard";
constexpr auto port_key            = "port";

constexpr auto all_parameters = {"set"};

template <typename T>
using setter_func_map = ::std::map<::std::string, ::std::function<void(T&, const nlohmann::basic_json<>&)>>;

template <typename T>
void read_parameters_to(nlohmann::json const& parameters, setter_func_map<T> const& setter, T& to) {
  for (auto const& p : parameters.items()) {
    if (setter.count(p.key()) > 0) {
      setter.at(p.key())(to, p.value());
    } else {
      throw stp_config_exception(stp_config_error::invalid_parameter, "unknown json field: " + p.key());
    }
  }
}

void check_version_is_supported(const stp_config& config) {
  if (config.version > 1) {
    throw stp_config_exception(stp_config_error::invalid_parameter,
                               "invalid version: " + ::std::to_string(config.version));
  }
}

template <typename T>
T get_checked(const nlohmann::basic_json<>& value, const ::std::string& param_name) {
  int64_t tmp;
  value.get_to(tmp);
  if (tmp < 0 || tmp > ::std::numeric_limits<T>::max()) {
    throw stp_config_exception(stp_config_error::invalid_parameter, "The value of" + param_name + " must be in the range from 0 to 255");
  }

  return static_cast<T>(tmp);
}

::std::string get_port_name_with_eth_prefix(const ::std::string& value) {
  if (value.rfind('X', 0) == 0) {
    return "eth" + value;
  }
  return value;
}

void edit_bridge(stp_config& config, const nlohmann::basic_json<>& json) {
  ::std::string br;
  json.get_to(br);

  if(config.bridge != br) {
    // If the bridge value changes, the port config corresponding to the previously selected bridge is no longer valid
    // for the current bridge.
    config.bridge = br;
    config.port_configs.clear();
  }
}

void edit_port_parameters(stp_config& config, const nlohmann::basic_json<>& json) {
  const setter_func_map<stp_port_config> setter_funcs = {
      {port_key, [](stp_port_config& p, const nlohmann::basic_json<>& value) { p.port = get_port_name_with_eth_prefix(value); }},
      {bpdu_filter_key, [](stp_port_config& p, const nlohmann::basic_json<>& value) { value.get_to(p.bpdu_filter); }},
      {bpdu_guard_key, [](stp_port_config& p, const nlohmann::basic_json<>& value) { value.get_to(p.bpdu_guard); }},
      {edge_port_key, [](stp_port_config& p, const nlohmann::basic_json<>& value) { value.get_to(p.edge_port); }},
      {path_cost_key,
       [](stp_port_config& p, const nlohmann::basic_json<>& value) { p.path_cost = get_checked<uint16_t>(value, path_cost_key); }},
      {port_priority_key,
       [](stp_port_config& p, const nlohmann::basic_json<>& value) { p.priority = get_checked<uint8_t>(value, port_priority_key); }},
      {root_guard_key, [](stp_port_config& p, const nlohmann::basic_json<>& value) { value.get_to(p.root_guard); }},
  };

  if (!json.is_array()) {
    throw stp_config_exception(stp_config_error::invalid_parameter, "port-configs must be an array");
  }

  for (auto const& jport_config : json) {
    auto port_it = jport_config.find(port_key);
    if (port_it == jport_config.end()) {
      throw stp_config_exception(stp_config_error::invalid_parameter, "port-configs must contain a port");
    }
    auto port_name = get_port_name_with_eth_prefix(*port_it);
    auto it        = std::find_if(config.port_configs.begin(), config.port_configs.end(),
                                  [&](stp_port_config& p) { return p.port == port_name; });

    if (it != config.port_configs.end()) {
      read_parameters_to(jport_config, setter_funcs, *it);
    } else {
      stp_port_config p{};
      read_parameters_to(jport_config, setter_funcs, p);
      config.port_configs.emplace_back(p);
    }
  }
}

}  // namespace

void check_that_parameters_are_known(parameter_map const& parameters) {
  static const auto known_parameters = ::std::set<::std::string>{all_parameters.begin(), all_parameters.end()};
  for (auto& parameter : parameters) {
    if (known_parameters.count(parameter.first) == 0) {
      throw stp_config_exception(stp_config_error::invalid_parameter,
                                 "invalid command line parameter: " + parameter.first);
    }
  }
}

void edit_parameters(const ::std::string& json_string, stp_config& config) {
  // clang-format off
  const setter_func_map<stp_config> setter_funcs = {
      {version_key,         [](stp_config& c, const nlohmann::basic_json<>& value) { c.version = get_checked<uint8_t>(value, version_key); check_version_is_supported(c); }},
      {stp_enabled_key,     [](stp_config& c, const nlohmann::basic_json<>& value) { value.get_to(c.enabled); }},
      {protocol_key,        [](stp_config& c, const nlohmann::basic_json<>& value) { value.get_to(c.protocol); }},
      {bridge_key,          edit_bridge},
      {bridge_priority_key, [](stp_config& c, const nlohmann::basic_json<>& value) { c.priority = get_checked<uint8_t>(value, bridge_priority_key); }},
      {max_age_key,         [](stp_config& c, const nlohmann::basic_json<>& value) { c.max_age = get_checked<uint8_t>(value, max_age_key); }},
      {max_hops_key,        [](stp_config& c, const nlohmann::basic_json<>& value) { c.max_hops = get_checked<uint8_t>(value, max_hops_key); }},
      {forward_delay_key,   [](stp_config& c, const nlohmann::basic_json<>& value) { c.forward_delay = get_checked<uint8_t>(value, forward_delay_key); }},
      {hello_time_key,      [](stp_config& c, const nlohmann::basic_json<>& value) { c.hello_time = get_checked<uint8_t>(value, hello_time_key); }},
      {port_configs_key,    edit_port_parameters}
  };
  // clang-format on

  try {
    nlohmann::json j = nlohmann::json::parse(json_string);
    read_parameters_to(j, setter_funcs, config);
  } catch (nlohmann::json::exception const& e) {
    throw stp_config_exception(stp_config_error::invalid_parameter, e.what());
  } catch (stp_config_exception const& e) {
    throw e;
  }
}

}  // namespace wago::stp::config_stp
