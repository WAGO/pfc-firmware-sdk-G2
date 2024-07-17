// Copyright (c) 2024 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stp.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#include <glib.h>
#pragma GCC diagnostic pop

#include "stp_config_handler.hpp"

using wago::stp::config_stp::stp_config_error;
using wago::stp::config_stp::stp_config_exception;

using wago::stp::lib::status;
using wago::stp::lib::stp_config;

namespace {

using namespace ::std::string_literals;

[[noreturn]] void show_help() {
  const ::std::string help_text = {R"(
      * Change the STP configuration *,
      Usage: config_stp < parameter-name=parameter-values >

      Example: config_stp set='json-string'

      parameter-value:
      set               'json'

      with json = ...
      e.g. '{"bridge":"br1","enabled":false,"forward-delay":15,"hello-time":2,"max-age":20,"max-hops":20,"port-configs":[{"bpdu-filter":false,"bpdu-guard":false,"edge-port":true,"path-cost":3,"port":"X2","priority":1,"root-guard":false}],"priority":8,"protocol":"stp","version":1}'

      bridge:         any configured bridge e.g. br0
      enabled:        true | false
      forward-delay:  value from the range 4-30
      hello-time:     value from the range 1-10
      max-age:        value from the range 6-40
      max-hops:       value from the range 6-40
      priority:       value from the range 0-15
      protocol:       stp | rstp
      version:        version of the configuration parameter set
      port-configs:   array of port configurations

      parameter set of a port config:
        bpdu-filter:  true | false
        bpdu-guard:   true | false
        edge-port:    true | false
        path-cost:    value from the range 0-65535
        port:         ethernet port, e.g. X1
        priority:     value from the range 0-15
        root-guard:   true | false

      )"};

  ::std::cout << help_text << ::std::endl;
  ::std::exit(0);
}

void write_last_error(::std::string const &text) {
  ::std::ofstream last_error;
  last_error.open("/tmp/last_error.txt");
  if (last_error.good()) {
    last_error << text;
    last_error.flush();
    last_error.close();
  }
}

void clear_last_error() {
  write_last_error(::std::string{});
}

[[noreturn]] void exit_with_error(stp_config_error code, ::std::string const &text) {
  write_last_error(text);
  ::std::exit(static_cast<int>(code));
}

::std::string uri_unescape(const ::std::string &escaped_uri) {
  auto result_cstr =
      std::unique_ptr<char, decltype(std::free) *>(g_uri_unescape_string(escaped_uri.c_str(), ""), std::free);
  if (result_cstr != nullptr) {
    return std::string{result_cstr.get()};
  }
  return std::string{};
}

}  // namespace

int main(int argc, const char *argv[]) {
  ::std::set<::std::string> help_args{"-h", "--help"};
  ::std::vector<::std::string> args(argv, argv + argc);
  ::std::unordered_map<::std::string, ::std::string> parameters{};
  for (auto &arg : args) {
    if (help_args.count(arg) > 0) {
      show_help();
    } else if (auto pos = arg.find('='); pos != arg.npos) {
      auto key   = arg.substr(0, pos);
      auto value = uri_unescape(arg.substr(pos + 1));
      parameters.emplace(key, value);
    }
  }

  clear_last_error();

  stp_config current_stp_config{};
  stp_config modified_stp_config{};
  status s = wago::stp::lib::get_persisted(current_stp_config);
  if (!s.ok()) {
    exit_with_error(stp_config_error::invalid_parameter, s.to_string());
  }
  modified_stp_config = current_stp_config;

  try {
    wago::stp::config_stp::check_that_parameters_are_known(parameters);
    wago::stp::config_stp::edit_parameters(parameters["set"], modified_stp_config);
  } catch (stp_config_exception const &e) {
    exit_with_error(e.error(), e.what());
  } catch (::std::exception const &e) {
    exit_with_error(stp_config_error::invalid_parameter, e.what());
  } catch (...) {
    exit_with_error(stp_config_error::invalid_parameter, "Unknown exception occurred");
  }

  if (current_stp_config != modified_stp_config) {
    s = wago::stp::lib::configure(modified_stp_config);
    if (!s.ok()) {
      wago::stp::lib::configure(current_stp_config);
      exit_with_error(stp_config_error::invalid_parameter, s.to_string());
    }
  }

  return 0;
}
