// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stp.hpp>

namespace wago::stp::get_stp_config {

class stp_data_provider_i {
 public:
  stp_data_provider_i()          = default;
  virtual ~stp_data_provider_i() = default;

  stp_data_provider_i(stp_data_provider_i &&)                 = delete;
  stp_data_provider_i(const stp_data_provider_i &)            = delete;
  stp_data_provider_i &operator=(stp_data_provider_i &&)      = delete;
  stp_data_provider_i &operator=(const stp_data_provider_i &) = delete;

  virtual ::wago::stp::lib::status get_stp_config(::wago::stp::lib::stp_config &config) const = 0;
  virtual ::wago::stp::lib::status get_stp_info(::wago::stp::lib::stp_info &info) const       = 0;
};

}  // namespace wago::stp::get_stp_config