// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "stp_data_provider_i.hpp"

namespace wago::stp::get_stp_config {

class stp_data_provider : public stp_data_provider_i {
 public:
  stp_data_provider()           = default;
  ~stp_data_provider() override = default;

  stp_data_provider(stp_data_provider &&)                 = delete;
  stp_data_provider(const stp_data_provider &)            = delete;
  stp_data_provider &operator=(stp_data_provider &&)      = delete;
  stp_data_provider &operator=(const stp_data_provider &) = delete;

  ::wago::stp::lib::status get_stp_config(::wago::stp::lib::stp_config &config) const override;
  ::wago::stp::lib::status get_stp_info(::wago::stp::lib::stp_info &info) const override;
};

}  // namespace wago::stp::get_stp_config