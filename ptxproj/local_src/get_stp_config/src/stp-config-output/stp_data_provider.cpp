// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
#include <stp_data_provider.hpp>
#include "utils.hpp"

namespace wago::stp::get_stp_config {

::wago::stp::lib::status stp_data_provider::get_stp_config(::wago::stp::lib::stp_config& config) const {
  auto status = ::wago::stp::lib::get_persisted(config);

  if (status.ok()) {
    remove_eth_prefix_from_ports(config);
  }

  return status;
}

::wago::stp::lib::status stp_data_provider::get_stp_info(::wago::stp::lib::stp_info& info) const {
  auto status = ::wago::stp::lib::get_info(info);
  
  if (status.ok()) {
    remove_eth_prefix_from_ports(info);
  }

  return status;
}

}  // namespace wago::stp::get_stp_config