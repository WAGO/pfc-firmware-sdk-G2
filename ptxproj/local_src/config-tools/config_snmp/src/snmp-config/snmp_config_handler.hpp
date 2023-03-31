// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <SnmpConfig.hpp>
#include <exception>
#include <map>
#include <stdexcept>
#include <string>
#include <system_error>

namespace wago::snmp_config {

using parameter_map = std::map<::std::string, ::std::string>;

void check_that_parameters_are_known(parameter_map const& parameters);

void edit_basic_parameter(parameter_map const& parameters, wago::snmp_config_lib::SnmpConfig& config);

void edit_v1v2c_trap_receiver(parameter_map const& parameters, wago::snmp_config_lib::SnmpConfig& config);

void edit_v1v2c_community(parameter_map const& parameters, wago::snmp_config_lib::SnmpConfig& config);

void edit_v3_user_or_trap(parameter_map const& parameters, wago::snmp_config_lib::SnmpConfig& config);

enum class snmp_config_error {
  invalid_parameter = 1,
  missing_parameter = 2,
};
class snmp_config_exception final : public ::std::runtime_error {
 public:
  snmp_config_exception(snmp_config_error snmp_config_error, ::std::string const& what)
      : ::std::runtime_error(what), error_(snmp_config_error) {
  }
  snmp_config_exception(const snmp_config_exception&)            = default;
  snmp_config_exception& operator=(const snmp_config_exception&) = default;
  snmp_config_exception(snmp_config_exception&&)                 = default;
  snmp_config_exception& operator=(snmp_config_exception&&)      = default;

  ~snmp_config_exception() final = default;

  snmp_config_error error() const {
    return error_;
  }

 private:
  snmp_config_error error_;
};

}  // namespace wago::snmp_config
