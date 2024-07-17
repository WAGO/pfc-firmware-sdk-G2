// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "stp_config_output.hpp"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <set>
#include <sstream>
#include <vector>

#include "network_config.hpp"
#include "stp.hpp"

using wago::stp::lib::status;
using wago::stp::lib::stp_config;
using wago::stp::lib::stp_info;
using wago::stp::lib::stp_port_config;
using wago::stp::lib::stp_port_info;
using json = nlohmann::json;

namespace wago::stp::lib {

// NOLINTNEXTLINE modernize-avoid-c-arrays
NLOHMANN_JSON_SERIALIZE_ENUM(protocol_version, {{wago::stp::lib::protocol_version::STP, "stp"},
                                                {wago::stp::lib::protocol_version::RSTP, "rstp"},
                                                {wago::stp::lib::protocol_version::MSTP, "mstp"}})

void to_json(json& j, const stp_port_config& config) {
  j = json{{"bpdu-filter", config.bpdu_filter},
           {"bpdu-guard", config.bpdu_guard},
           {"edge-port", config.edge_port},
           {"path-cost", config.path_cost},
           {"port", config.port},
           {"priority", config.priority},
           {"root-guard", config.root_guard}};
}

void to_json(json& j, const stp_port_info& info) {
  j = json{{"bpdu-filter", info.bpdu_filter},
           {"bpdu-guard", info.bpdu_guard},
           {"edge-port", info.edge_port},
           {"path-cost", info.path_cost},
           {"port", info.port},
           {"priority", info.priority},
           {"role", info.role},
           {"root-guard", info.root_guard},
           {"status", info.status}};
}

// clang-format off
void to_json(json& j, const stp_config& config) {
  j = json{{"bridge", config.bridge},
           {"enabled", config.enabled},
           {"forward-delay", config.forward_delay},
           {"hello-time", config.hello_time},
           {"max-age", config.max_age},
           {"max-hops", config.max_hops},
           {"port-configs", config.port_configs},
           {"priority", config.priority},
           {"protocol", config.protocol},
           {"version", config.version}};
}

void to_json(json& j, const stp_info& info) {
  j = json{{"bridge", info.bridge},
           {"enabled", info.enabled},
           {"forward-delay", info.forward_delay},
           {"hello-time", info.hello_time},
           {"max-age", info.max_age},
           {"max-hops", info.max_hops},
           {"path-cost", info.path_cost},
           {"ports", info.ports},
           {"priority", info.priority},
           {"protocol", info.protocol}};
}
// clang-format on

}  // namespace wago::stp::lib

namespace {

void show_help() {
  const ::std::string help_text = {R"(
      Usage: get_stp [--backup [--backup-targetversion=<fw-version>] | --pretty ]

      Print the STP configuration.

      -c|--config                              Print the STP configuration.
      -i|--info                                Print the current STP status.
      -b|--backup                              Output the configuration in backup file format.
      -h|--help                                Print this help text.

      -t|--backup-targetversion=<fw-version>   Request a specific target firmware version, e.g. 04.04.00(26)
      -p|--pretty                              Output the configuration in json format that is more readable for humans.
      )"};

  ::std::cout << help_text << ::std::endl;
}

void print_json(const json& j, const bool pretty) {
  if (pretty) {
    ::std::cout << std::setw(4) << j;
  } else {
    ::std::cout << j;
  }
}

void print_backup_content_v1(const json& j) {
  const ::std::string key           = "stp.data";
  const int chars_per_line_         = 88;
  const size_t value_chars_per_line = chars_per_line_ - (key.length() + 1);

  std::stringstream ss;
  ::std::string data = j.dump();
  auto lines         = static_cast<uint32_t>(::std::floor(data.size() / value_chars_per_line));
  for (uint32_t line = 0; line < lines; line++) {
    ::std::string substring = data.substr(line * value_chars_per_line, value_chars_per_line);
    ss << key << '=' << substring << '\n';
  }

  auto remaining_chars = data.size() % value_chars_per_line;
  if (remaining_chars > 0) {
    ::std::string substring = data.substr(lines * value_chars_per_line, data.size());
    ss << key << '=' << substring << '\n';
  }

  ::std::cout << ss.str();
}

stp_port_config get_or_default(const ::std::vector<stp_port_config>& port_configs, const ::std::string& port) {
  auto itr = ::std::find_if(port_configs.cbegin(), port_configs.cend(),
                            [&](const stp_port_config& pc) { return pc.port == port; });
  if (itr != port_configs.cend()) {
    return *itr;
  }

  stp_port_config pc;
  pc.port = port;

  return pc;
}

void write_last_error(::std::string const& text) {
  ::std::ofstream last_error;
  last_error.open("/tmp/last_error.txt");
  if (last_error.good()) {
    last_error << text;
    last_error.flush();
    last_error.close();
  }
}

}  // namespace

namespace wago::stp::get_stp_config {

void update_stp_config_regarding_bridge_ports(const ::std::vector<::std::string>& ports, stp_config& stp_config) {
  auto port_configs = stp_config.port_configs;

  stp_config.port_configs.clear();
  for (const auto& port : ports) {
    stp_config.port_configs.emplace_back(get_or_default(port_configs, port));
  }
}

status get_stp_config_as_json(const stp_config& c, json& j) {
  ::std::vector<::std::string> ports;
  stp_config config = c;

  status s = wago::stp::network_config::get_bridge_interfaces(config.bridge, ports);

  if (s.ok()) {
    update_stp_config_regarding_bridge_ports(ports, config);
    s = config_to_json(j, config);
  }

  return s;
}

wago::stp::lib::status parse_commandline_args(const ::std::vector<::std::string>& args, parameter_list& parameters,
                                              option_map& options) {
  wago::stp::lib::status s{};

  // actions
  ::std::set<::std::string> config_args{"-c", "--config"};
  ::std::set<::std::string> info_args{"-i", "--info"};
  ::std::set<::std::string> backup_args{"-b", "--backup"};
  ::std::set<::std::string> help_args{"-h", "--help"};
  // options
  ::std::set<::std::string> backup_targetversion_args{"-t=", "--backup-targetversion="};
  ::std::set<::std::string> pretty_args{"-p", "--pretty"};

  if (args.size() > 2) {
    return {wago::stp::lib::status_code::WRONG_PARAMETER_PATTERN, "Invalid parameter count."};
  }

  for (const auto& arg : args) {
    wago::stp::get_stp_config::action action_arg;

    if (arg.rfind("-t=", 0) == 0 || arg.rfind("--backup-targetversion=", 0) == 0) {
      auto pos = arg.find('=');
      if (pos + 1 < arg.length()) {
        ::std::string version = arg.substr(pos + 1);
        if (is_valid_fw_version(version)) {
          options.emplace(action_option::targetversion, version);
          continue;
        }
        s = {wago::stp::lib::status_code::WRONG_PARAMETER_PATTERN, "Invalid targetversion value"};
      } else {
        s = {wago::stp::lib::status_code::WRONG_PARAMETER_PATTERN, "Missing targetversion value"};
      }
    } else if (config_args.count(arg) > 0) {
      action_arg = wago::stp::get_stp_config::action::json_config;
    } else if (info_args.count(arg) > 0) {
      action_arg = wago::stp::get_stp_config::action::json_info;
    } else if (backup_args.count(arg) > 0) {
      action_arg = wago::stp::get_stp_config::action::backup;
    } else if (help_args.count(arg) > 0) {
      action_arg = wago::stp::get_stp_config::action::help;
    } else if (pretty_args.count(arg) > 0) {
      options.emplace(action_option::pretty, "");
      continue;
    } else {
      s = {wago::stp::lib::status_code::WRONG_PARAMETER_PATTERN, "Unknown parameter"};
    }

    if (s.ok()) {
      if (::std::find(parameters.cbegin(), parameters.cend(), action_arg) == parameters.cend()) {
        parameters.emplace_back(action_arg);
      } else {
        s = {wago::stp::lib::status_code::WRONG_PARAMETER_PATTERN, "Found duplicate parameter"};
      }
    }

    if (!s.ok()) {
      break;
    }
  }

  if (parameters.empty()) {
    parameters.emplace_back(wago::stp::get_stp_config::action::help);
  }

  return s;
}

status execute_action(const parameter_list& parameters, const option_map& options,
                      const stp_data_provider_i& provider) {
  if (parameters.size() != 1) {
    return status{lib::status_code::WRONG_PARAMETER_PATTERN, "Invalid parameter count."};
  }

  action_map action_funcs = {
      {action::json_config, [](const option_map& om, const stp_data_provider_i& p) { print_json_config(om, p); }},
      {action::json_info, [](const option_map& om, const stp_data_provider_i& p) { print_json_info(om, p); }},
      {action::backup, [](const option_map& om, const stp_data_provider_i& p) { print_backup_content(om, p); }},
      {action::help, [](const option_map&, const stp_data_provider_i&) { show_help(); }}};

  action selected_action = parameters.front();
  action_funcs.at(selected_action)(options, provider);

  return status{};
}

status config_to_json(json& j, const stp_config& config) {
  status s{};
  if (config.version == 1) {
    j = config;
  } else {
    s = status{wago::stp::lib::status_code::JSON_PARSE_ERROR, "Unsupported config version"};
  }

  return s;
}

[[noreturn]] void exit_with_error(get_stp_config_error code, ::std::string const& text) {
  write_last_error(text);
  ::std::exit(static_cast<int>(code));
}

void clear_last_error() {
  write_last_error(::std::string{});
}

void print_json_config(const wago::stp::get_stp_config::option_map& options,
                       const wago::stp::get_stp_config::stp_data_provider_i& provider) {
  json j;
  stp_config config;

  status s = provider.get_stp_config(config);
  if (s.ok()) {
    s = wago::stp::get_stp_config::get_stp_config_as_json(config, j);
  }

  if (s.ok()) {
    print_json(j, options.count(wago::stp::get_stp_config::action_option::pretty) > 0);
  } else {
    exit_with_error(wago::stp::get_stp_config::get_stp_config_error::internal_error, s.to_string());
  }
}

void print_json_info(const wago::stp::get_stp_config::option_map& options,
                     const wago::stp::get_stp_config::stp_data_provider_i& provider) {
  stp_info info;

  status s = provider.get_stp_info(info);

  if (s.ok()) {
    json j = info;
    print_json(j, options.count(wago::stp::get_stp_config::action_option::pretty) > 0);
  } else {
    exit_with_error(wago::stp::get_stp_config::get_stp_config_error::internal_error, s.to_string());
  }
}

void print_backup_content(const option_map& options, const stp_data_provider_i& provider) {
  json j;
  stp_config config;

  status s = provider.get_stp_config(config);
  if (s.ok()) {
    s = get_stp_config_as_json(config, j);
  }

  if (s.ok()) {
    ::std::string version = "04.04.00(26)";
    if (options.count(action_option::targetversion) > 0) {
      version = options.at(action_option::targetversion);
    }

    print_backup_content_v1(j);

  } else {
    exit_with_error(get_stp_config_error::internal_error, s.to_string());
  }
}

bool is_valid_fw_version(const ::std::string& version) {
  return std::regex_match(version, std::regex(R"(^[0-9]{2}\.[0-9]{2}\.[0-9]{2}\([0-9]{2}\))"));
}

}  // namespace wago::stp::get_stp_config