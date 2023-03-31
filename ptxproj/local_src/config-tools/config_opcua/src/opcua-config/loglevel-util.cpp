// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "loglevel-util.hpp"

namespace wago::opcua_config::log_levels
{

int log_level_from_string(const std::string & s)
{
  if (s == "error")
  {
    return error;
  }
  if (s == "warning")
  {
    return warning;
  }
  if (s == "info")
  {
    return info;
  }
  if (s == "debug")
  {
    return debug;
  }

  return warning;
}

std::string string_from_log_level(int l)
{
  switch (l)
  {
  case error:
    return "error";
  case warning:
    return "warning";
  case info:
    return "info";
  case debug:
    return "debug";
  default:
    break;
  }

  return "warning";
}

}