//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     get_snmp_data
///
///  \brief
///
///  \author   SM : WAGO GmbH & Co. KG
///  \author   MOe : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <iostream>
#include <string>

#include "get_snmp.hpp"

using wago::snmp_config_lib::ReadSnmpConfig;
using wago::get_snmp_data::GetParameter;
using wago::get_snmp_data::Result;

namespace {

Result<size_t> ToIndex(::std::string &index) {
  try {
    int i = ::std::stoi(index) - 1;
    if (i >= 0) {
      return Result<size_t> { static_cast<size_t>(i) };
    }
  } catch (...) {
    return Result<size_t> { INVALID_PARAMETER };
  }
  return Result<size_t> { INVALID_PARAMETER };
}

}  // anonymous namespace

int main(int argc, const char *argv[]) {

  ::std::vector<::std::string> args(argv, argv + argc);

  if (args.size() < 2) {
    return MISSING_PARAMETER;
  }
  ::std::string option { args.at(1) };

  ::std::optional<size_t> index{};
  if (args.size() > 2) {
    auto result = ToIndex(args.at(2));
    if (result.status_ != SUCCESS) {
      return result.status_;
    }
    index = result.value_;
  }

  auto config = ReadSnmpConfig();
  auto result = GetParameter(option, index, config);

  if (result.status_ != SUCCESS) {
    return result.status_;
  }

  ::std::cout << result.value_;

  return SUCCESS;

}
