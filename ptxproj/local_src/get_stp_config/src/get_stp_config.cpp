// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "stp_config_output.hpp"
#include "stp_data_provider.hpp"

int main(int argc, const char* argv[]) {
  ::std::vector<::std::string> args(argv + 1, argv + argc);  // Ignore the program name.
  wago::stp::get_stp_config::parameter_list parameters;
  wago::stp::get_stp_config::option_map options;

  wago::stp::get_stp_config::clear_last_error();

  ::wago::stp::lib::status s = parse_commandline_args(args, parameters, options);

  if (s.ok()) {
    wago::stp::get_stp_config::stp_data_provider data_provider{};
    s = execute_action(parameters, options, data_provider);
  } else {
    exit_with_error(wago::stp::get_stp_config::get_stp_config_error::parameter_parse_error, s.to_string());
  }

  if (!s.ok()) {
    exit_with_error(wago::stp::get_stp_config::get_stp_config_error::parameter_parse_error, s.to_string());
  }

  return 0;
}
