// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <functional>
#include <nlohmann/json_fwd.hpp>
#include <stp.hpp>

#include "stp_data_provider_i.hpp"

namespace wago::stp::get_stp_config {

enum class get_stp_config_error { parameter_parse_error = 1, internal_error = 2 };

enum class action { json_config, json_info, backup, help };

enum class action_option { pretty, targetversion };

using parameter_list = ::std::vector<action>;
using option_map     = ::std::map<action_option, ::std::string>;
using action_map =
    ::std::map<get_stp_config::action, ::std::function<void(const option_map&, const stp_data_provider_i&)>>;

void update_stp_config_regarding_bridge_ports(const ::std::vector<::std::string>& ports,
                                              ::wago::stp::lib::stp_config& stp_config);
::wago::stp::lib::status get_stp_config_as_json(const ::wago::stp::lib::stp_config& c, ::nlohmann::json& j);
::wago::stp::lib::status parse_commandline_args(const ::std::vector<::std::string>& args, parameter_list& parameters,
                                                option_map& options);
::wago::stp::lib::status execute_action(const parameter_list& parameters, const option_map& options,
                                        const stp_data_provider_i& provider);
[[noreturn]] void exit_with_error(get_stp_config_error code, ::std::string const& text);
void clear_last_error();
::wago::stp::lib::status config_to_json(::nlohmann::json& j, const ::wago::stp::lib::stp_config& config);
void print_backup_content(const option_map& options, const stp_data_provider_i& provider);
void generate_output(const ::std::function<void(const ::nlohmann::json& j)>& func);
void print_json_config(const wago::stp::get_stp_config::option_map& options,
                       const wago::stp::get_stp_config::stp_data_provider_i& provider);
void print_json_info(const wago::stp::get_stp_config::option_map& options,
                     const wago::stp::get_stp_config::stp_data_provider_i& provider);
bool is_valid_fw_version(const ::std::string& version);

}  // namespace wago::stp::get_stp_config