// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#include <glib.h>
#pragma GCC diagnostic pop

#include "snmp_config_handler.hpp"

using ::wago::snmp_config::edit_basic_parameter;
using ::wago::snmp_config::edit_v1v2c_community;
using ::wago::snmp_config::edit_v1v2c_trap_receiver;
using ::wago::snmp_config::edit_v3_user_or_trap;
using ::wago::snmp_config::check_that_parameters_are_known;
using ::wago::snmp_config::snmp_config_error;
using ::wago::snmp_config_lib::ReadSnmpConfig;
using ::wago::snmp_config_lib::WriteSnmpConfig;
namespace {

using namespace ::std::string_literals;

[[noreturn]] void show_help() {
  const ::std::string help_text = {R"(
      * Change the SNMP configuration *,
      Usage: config_snmp < parameter-name=parameter-value > [ parameter-name=parameter-value ] ...

      parameter-value:
      set-snmp: enabled | disabled
      device-name: any string
      description: any string
      physical-location: any string
      contact: any string
      objectID: any OID which represents the sysObjectID
      v1-v2c-state: disabled | enabled, in case of enabled, v1-v2c-community-name must be given
      v1-v2c-community-name: string without spaces, min 1, max 32 characters

      Following 3 parameters must be given together:
      v1-v2c-community-edit: add | delete
      v1-v2c-community-name: string without spaces, min 1, max 32 characters
      v1-v2c-community-access: read-only | read-write

      Following 4 parameters must be given together:
      v1-v2c-trap-receiver-edit: add | delete
      v1-v2c-trap-receiver-address: ip address format
      v1-v2c-trap-receiver-community-name: string without spaces, min 1, max 32 characters
      v1-v2c-trap-receiver-version-value: v1 | v2c

      Following 7 parameters must be given together:
      v3-edit | v3-user-edit | v3-trap-receiver-edit: add | delete
      v3-user-access: read-only | read-write
      v3-auth-name: string without spaces, min 1, max 32 characters
      v3-auth-type: none | MD5 | SHA | SHA224 | SHA256 | SHA384 | SHA512
      v3-auth-key: string without spaces, min 8 characters
      v3-privacy: none | DES | AES | AES128 | AES192 | AES192C | AES256 | AES256C
      v3-privacy-key: string without spaces, min 8 characters; optional - if it is not given, netsnmp will use auth type value
      v3-notification-receiver: ip address format)"};

  ::std::cout << help_text << ::std::endl;
  ::std::exit(0);
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

void clear_last_error() {
  write_last_error(::std::string{});
}

[[noreturn]] void exit_with_error(snmp_config_error code, ::std::string const &text) {
  write_last_error(text);
  ::std::exit(static_cast<int>(code));
}

::std::string uri_unescape(const ::std::string &escaped_uri) {
  auto result_cstr = std::unique_ptr<char, decltype(std::free)*>(
      g_uri_unescape_string(escaped_uri.c_str(), ""), std::free);
  if (result_cstr != nullptr) {
    return std::string { result_cstr.get() };
  }
  return std::string { };
}

}  // namespace

int main(int argc, const char *argv[]) {
  ::std::set<::std::string> help_args{"-h", "--help"};
  ::std::vector<::std::string> args(argv, argv + argc);
  ::std::map<::std::string, ::std::string> parameters{};
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

  auto const current_snmp_config = ReadSnmpConfig();
  auto modified_snmp_config      = current_snmp_config;

  try {
    check_that_parameters_are_known(parameters);
    edit_basic_parameter(parameters, modified_snmp_config);
    edit_v3_user_or_trap(parameters, modified_snmp_config);
    edit_v1v2c_community(parameters, modified_snmp_config);
    edit_v1v2c_trap_receiver(parameters, modified_snmp_config);
  } catch (wago::snmp_config::snmp_config_exception const &e) {
    exit_with_error(e.error(), e.what());
  } catch (::std::exception const &e) {
    exit_with_error(snmp_config_error::invalid_parameter, e.what());
  } catch (...) {
    exit_with_error(snmp_config_error::invalid_parameter, "Unknown exception occured");
  }

  if (current_snmp_config != modified_snmp_config) {
    auto write_result = WriteSnmpConfig(modified_snmp_config);
    for (auto& status : write_result.GetStatuses()) {
      if (!status.IsOk()) {
        exit_with_error(snmp_config_error::invalid_parameter, status.ToString());
      }
    }
  }

  return 0;
}
