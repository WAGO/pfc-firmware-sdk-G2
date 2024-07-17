// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <sstream>

#include "stp.hpp"

namespace wago::stp::lib {

  stp_config from_stream(::std::stringstream &ss);
  ::std::stringstream to_stream(stp_config config, bool only_mstp_config = false);

}  // namespace wago::std::lib
