// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "stp.hpp"

namespace wago::stp::lib {

status write(std::stringstream &ss, ::std::string &&path);

status read(std::stringstream &ss, ::std::string &&path);

}