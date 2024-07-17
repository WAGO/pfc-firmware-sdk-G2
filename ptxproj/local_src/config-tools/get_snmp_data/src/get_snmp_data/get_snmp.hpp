#pragma once
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

#include <string>
#include <optional>

#include <SnmpConfig.hpp>
#include <ct_error_handling.h>

namespace wago::get_snmp_data {


template<class T>
struct Result{
  explicit Result(T value): value_{::std::move(value)}{}
  explicit Result(eStatusCode status): status_{status}{}
  T value_{};
  eStatusCode status_ = SUCCESS;
};

Result<::std::string> GetParameter(::std::string& paramter_name, ::std::optional<size_t> index, ::wago::snmp_config_lib::SnmpConfig &snmp_config);

::std::string VersionV1V2cAsString(const wago::snmp_config_lib::VersionV1V2c &version);
::std::string AccessAsString(const wago::snmp_config_lib::Access &access);
::std::string AuthenticationTypeAsStr(const wago::snmp_config_lib::AuthenticationType &type);
::std::string PrivacyAsStr(const wago::snmp_config_lib::Privacy &privacy);
::std::string SecurityLevelAsStr(const wago::snmp_config_lib::SecurityLevel &level);

}  // namespace wago::get_snmp_data
