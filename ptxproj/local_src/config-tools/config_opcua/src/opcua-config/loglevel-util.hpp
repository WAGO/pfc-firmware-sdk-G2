// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

namespace wago::opcua_config
{

namespace log_levels_cds
{

const int error = 0x020;
const int warning = 0x010;
const int system = 0x008;
const int info = 0x004;
const int debug = 0x002;
const int content = 0x001;

const int datetime = 0x080;
const int mutex = 0x100;
const int semaphore = 0x200;
const int memory = 0x400;
const int socket = 0x800;

}

namespace log_levels
{

const int error = log_levels_cds::error;
const int warning = log_levels_cds::error | log_levels_cds::warning;
const int info = log_levels_cds::error | log_levels_cds::warning | log_levels_cds::system | log_levels_cds::info;
const int debug = log_levels_cds::error |
                  log_levels_cds::warning |
                  log_levels_cds::system |
                  log_levels_cds::info |
                  log_levels_cds::debug |
                  log_levels_cds::content |
                  log_levels_cds::datetime |
                  log_levels_cds::mutex |
                  log_levels_cds::semaphore |
                  log_levels_cds::memory |
                  log_levels_cds::socket;

int log_level_from_string(const std::string & s);
std::string string_from_log_level(int l);

};

} // namespace wago::opcua_config
