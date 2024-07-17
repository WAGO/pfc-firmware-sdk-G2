// Copyright (c) 2024 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stp.hpp>
#include <exception>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <system_error>

namespace wago::stp::config_stp {

using parameter_map = std::unordered_map<::std::string, ::std::string>;

void check_that_parameters_are_known(parameter_map const& parameters);

void edit_parameters(const ::std::string& json_string, wago::stp::lib::stp_config& config);

enum class stp_config_error {
  invalid_parameter = 1,
  missing_parameter = 2,
};
class stp_config_exception final : public ::std::runtime_error {
 public:
  stp_config_exception(stp_config_error stp_config_error, ::std::string const& what)
      : ::std::runtime_error(what), error_(stp_config_error) {
  }
  stp_config_exception(const stp_config_exception&)            = default;
  stp_config_exception& operator=(const stp_config_exception&) = default;
  stp_config_exception(stp_config_exception&&)                 = default;
  stp_config_exception& operator=(stp_config_exception&&)      = default;

  ~stp_config_exception() final = default;

  stp_config_error error() const {
    return error_;
  }

 private:
  stp_config_error error_;
};

}  // namespace wago::snmp_config
