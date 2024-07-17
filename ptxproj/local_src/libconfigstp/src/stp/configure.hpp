// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "stp.hpp"

namespace wago::stp::lib {

status configure_stp(const stp_config &config) ;
status read_persistence(stp_config &config);
status write_persistence(const stp_config &config);

}