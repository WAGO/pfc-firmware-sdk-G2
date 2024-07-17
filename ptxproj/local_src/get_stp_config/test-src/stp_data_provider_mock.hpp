// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <gmock/gmock.h>
#include "stp_data_provider_i.hpp"

namespace wago::stp::get_stp_config {

class stp_data_provider_mock : public stp_data_provider_i {
 public:
  MOCK_CONST_METHOD1(get_stp_config, ::wago::stp::lib::status(::wago::stp::lib::stp_config &));
  MOCK_CONST_METHOD1(get_stp_info, ::wago::stp::lib::status(::wago::stp::lib::stp_info &));
};

}  // namespace wago::stp::get_stp_config