// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "stp.hpp"

#include <nlohmann/json.hpp>

namespace wago::stp::lib {

NLOHMANN_JSON_SERIALIZE_ENUM(protocol_version, {{protocol_version::STP, "stp"},
                                                {protocol_version::RSTP, "rstp"}})

status parse_stp_info(::std::string& json_str, stp_info& info);

}