/*
 * SnmpParameterValidation.cpp
 *
 *  Created on: 14.02.2022
 *      Author: u016903
 */

#include "SnmpParameterValidation.hpp"

#include <regex>
#include <iostream>

#include <locale>
#include <codecvt>
#include <string>

namespace wago::snmp_config_lib {

namespace {

/*::std::string is not suitable for processing strings with umlauts because they are larger than a char.
 * wchar reserves 4 bytes for each character.
 */
::std::string GetFirstUtf8CharacterFromString(::std::string &str) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::wstring wide = converter.from_bytes(str);
  wide = wide.front();
  return converter.to_bytes(wide);
}

bool checkString(const ::std::string &value, ::std::string &&regex, size_t min_string_length, size_t max_string_length,
                 ::std::string &match_error) {
  match_error = ::std::string { };
  if (value.size() < min_string_length || value.size() > max_string_length) {
    if (min_string_length == 0) {
      match_error = "the string can be up to " + ::std::to_string(max_string_length) + " characters long.";
    } else {
      match_error = "the string must be between " + ::std::to_string(min_string_length) + " and "
          + ::std::to_string(max_string_length) + " characters long.";
    }
    return false;
  }

  bool success = false;
  ::std::regex expr { regex };
  ::std::smatch sm;
  if (::std::regex_match(value, sm, expr)) {
    success = true;
  } else {
    ::std::regex_search(value, sm, expr);
    ::std::string suffix = sm.suffix();
    if (not suffix.empty()) {
      match_error = ::std::string("character '") + GetFirstUtf8CharacterFromString(suffix) + "' is not allowed.";
    }
  }
  return success;

}

bool IsAnyString(const ::std::string &value, ::std::string &match_error) {
  return checkString(value, "[A-Za-z0-9 !?:;\\.,{}\\[\\]()*~'_\\-@+]{0,255}", 0, 255, match_error);  //NOLINT modernize-raw-string-literal ::std::regex does not support raw-string-literal
}

bool IsSelectedCharsWithLength8_32(const ::std::string &value, ::std::string &match_error) {
  return checkString(value, "[A-Za-z0-9!()*~'._-]{0,32}", 8, 32, match_error);
}

Statuses ValidateBasicParameter(const SnmpConfig &config) {
  Statuses s;
  ::std::string match_error { };
  if (not IsValidName(config.sys_name_, match_error)) {
    s.AddErrorStatus(
        Status { StatusCode::WRONG_PARAMETER_PATTERN, "Invalid name: '" + config.sys_name_ + "' " + match_error });
  }
  if (not IsValidContact(config.sys_contact_, match_error)) {
    s.AddErrorStatus(
        Status { StatusCode::WRONG_PARAMETER_PATTERN, "Invalid contact '" + config.sys_contact_ + "' " + match_error });
  }
  if (not IsValidDescription(config.sys_descr_, match_error)) {
    s.AddErrorStatus(
        Status { StatusCode::WRONG_PARAMETER_PATTERN, "Invalid description '" + config.sys_descr_ + "' " + match_error });
  }
  if (not IsValidLocation(config.sys_location_, match_error)) {
    s.AddErrorStatus(
        Status { StatusCode::WRONG_PARAMETER_PATTERN, "Invalid location: '" + config.sys_location_ + "' " + match_error });
  }
  if (not IsValidObjectId(config.sys_objectID_, match_error)) {
    s.AddErrorStatus(
        Status { StatusCode::WRONG_PARAMETER_PATTERN, "Invalid objectID: '" + config.sys_objectID_ + "' " + match_error });
  }
  return s;
}

void ValidateDuplicateUserEntry(const ::std::vector<User> &user, Statuses &s, ::std::string &&error_msg) {
  auto data = user;
  ::std::sort(data.begin(), data.end());
  auto it = ::std::adjacent_find(data.begin(), data.end(), [](const User &u1, const User &u2) {
    return u1.name_ == u2.name_;
  });

  if (it != data.end()) {
    s.AddErrorStatus(Status { StatusCode::WRONG_PARAMETER_PATTERN, error_msg + ": '" + it->name_ + "'" });
  }
}

void ValidateDuplicateTrapReceiverV3Entry(const ::std::vector<TrapReceiverV3> &trap_receiver, Statuses &s,
                                          ::std::string &&error_msg) {
  auto data = trap_receiver;
  ::std::sort(data.begin(), data.end());
  auto it = ::std::adjacent_find(data.begin(), data.end(), [](const TrapReceiverV3 &t1, const TrapReceiverV3 t2) {
    return (t1.name_ == t2.name_ && t1.host_ == t2.host_);
  });

  if (it != data.end()) {
    s.AddErrorStatus(Status { StatusCode::WRONG_PARAMETER_PATTERN, error_msg + ": '" + it->name_ + "'" });
  }
}

void ValidateDuplicateCommunityEntry(const ::std::vector<Community> &communities, Statuses &s) {

  auto data = communities;
  std::sort(data.begin(), data.end());
  auto it = std::adjacent_find(data.begin(), data.end());
  if (it != data.end()) {
    s.AddErrorStatus(
        Status { StatusCode::WRONG_PARAMETER_PATTERN, "duplicated entry for community: '" + it->name_ + "'" });
  }
}

void ValidateDuplicateTrapReceiverV1V2cEntry(const ::std::vector<TrapReceiverV1V2c> &traps, Statuses &s) {

  auto data = traps;
  std::sort(data.begin(), data.end());
  auto it = std::adjacent_find(data.begin(), data.end());
  if (it != data.end()) {
    s.AddErrorStatus(
        Status { StatusCode::WRONG_PARAMETER_PATTERN, "duplicated entry for trap receiver: '" + it->host_ + "'" });
  }
}

void ValidateCommunityName(const ::std::string &community, Statuses &s, ::std::string &&error_msg) {
  ::std::string match_error;
  if (not IsValidCommunityName(community, match_error)) {
    s.AddErrorStatus(
        Status { StatusCode::WRONG_PARAMETER_PATTERN, error_msg + ": '" + community + "' " + match_error });
  }
}

void ValidateV3Name(const ::std::string &name, Statuses &s, ::std::string &&error_msg) {
  ::std::string match_error;
  if (not IsValidV3Name(name, match_error)) {
    s.AddErrorStatus(Status { StatusCode::WRONG_PARAMETER_PATTERN, error_msg + ": '" + name + "' " + match_error });
  }
}

void ValidateHost(const ::std::string &name, Statuses &s, ::std::string &&error_msg) {
  ::std::string match_error;
  if (not IsValidHost(name, match_error)) {
    s.AddErrorStatus(Status { StatusCode::WRONG_PARAMETER_PATTERN, error_msg + ": '" + name + "' " + match_error });
  }
}

template<class T>
void ValidateAuthenticationKey(const T &data, Statuses &s, ::std::string &&error_msg) {
  ::std::string match_error;
  if (data.security_level_ == SecurityLevel::AuthNoPriv || data.security_level_ == SecurityLevel::AuthPriv) {
    if (not IsValidAuthenticationKey(data.authentication_key_, match_error)) {
      s.AddErrorStatus(
          Status { StatusCode::WRONG_PARAMETER_PATTERN, error_msg + ": '" + data.authentication_key_ + "' "
              + match_error });
    }
  }
}

template<class T>
void ValidatePrivacyKey(const T &data, Statuses &s, ::std::string &&error_msg) {
  ::std::string match_error;
  if (data.security_level_ == SecurityLevel::AuthPriv) {
    if (not IsValidPrivacyKey(data.privacy_key_, match_error)) {
      s.AddErrorStatus(
          Status { StatusCode::WRONG_PARAMETER_PATTERN, error_msg + ": '" + data.privacy_key_ + "' " + match_error });
    }
  }
}

Statuses ValidateTrapV1V2cParameter(const SnmpConfig &config) {
  Statuses s;
  ::std::string match_error { };
  for (auto &trap : config.trap_receivers_V1V2c_) {
    ValidateHost(trap.host_, s, "invalid host for trap receiver");
    ValidateCommunityName(trap.community_, s, "invalid community name for trap receiver");
  }

  ValidateDuplicateTrapReceiverV1V2cEntry(config.trap_receivers_V1V2c_, s);
  return s;
}

Statuses ValidateCommunityParameter(const SnmpConfig &config) {
  Statuses s;
  ::std::string match_error { };
  for (auto &community : config.communities_) {
    ValidateCommunityName(community.name_, s, "invalid community name");
  }

  ValidateDuplicateCommunityEntry(config.communities_, s);
  return s;
}

Statuses ValidateTrapV3Parameter(const SnmpConfig &config) {
  Statuses s;
  for (auto &trap : config.trap_receivers_V3_) {
    ValidateV3Name(trap.name_, s, "invalid name for trap receiver");
    ValidateHost(trap.host_, s, "invalid host for trap receiver");
    ValidateAuthenticationKey(trap, s, "invalid authentication key for trap receiver");
    ValidatePrivacyKey(trap, s, "invalid privacy key for trap receiver");
  }

  ValidateDuplicateTrapReceiverV3Entry(config.trap_receivers_V3_, s, "duplicated trap receiver name");
  return s;
}

Statuses ValidateUserParameter(const SnmpConfig &config) {
  Statuses s;
  for (auto &user : config.user_) {
    ValidateV3Name(user.name_, s, "invalid name for user");
    ValidateAuthenticationKey(user, s, "invalid authentication key for user");
    ValidatePrivacyKey(user, s, "invalid privacy key for user");
  }

  ValidateDuplicateUserEntry(config.user_, s, "duplicated user name");
  return s;
}

}  //namespace

bool IsValidName(const ::std::string &value, ::std::string &match_error) {
  return IsAnyString(value, match_error);
}

bool IsValidDescription(const ::std::string &value, ::std::string &match_error) {
  return IsAnyString(value, match_error);
}

bool IsValidContact(const ::std::string &value, ::std::string &match_error) {
  return IsAnyString(value, match_error);
}

bool IsValidLocation(const ::std::string &value, ::std::string &match_error) {
  return IsAnyString(value, match_error);
}

bool IsValidObjectId(const ::std::string &value, ::std::string &match_error) {
  return checkString(value, "(\\.\\d+)*", 0, 128, match_error);
}

bool IsValidCommunityName(const ::std::string &value, ::std::string &match_error) {
  return checkString(value, "[A-Za-z0-9!?:;\\.,{}\\[\\]()*~'_\\-@+]{0,32}", 1, 32, match_error);  //NOLINT modernize-raw-string-literal ::std::regex does not support raw-string-literal
}

bool IsValidHost(const ::std::string &value, ::std::string &match_error) {
  return checkString(value, "[a-zA-Z0-9\\-\\.]{0,255}", 1, 255, match_error);
}

bool IsValidUserName(const ::std::string &value, ::std::string &match_error) {
  return IsSelectedCharsWithLength8_32(value, match_error);
}

bool IsValidAuthenticationKey(const ::std::string &value, ::std::string &match_error) {
  return IsSelectedCharsWithLength8_32(value, match_error);
}

bool IsValidV3Name(const ::std::string &value, ::std::string &match_error) {
  return checkString(value, "[A-Za-z0-9!()*~'._-]{0,32}", 1, 32, match_error);  //NOLINT modernize-raw-string-literal ::std::regex does not support raw-string-literal
}

bool IsValidPrivacyKey(const ::std::string &value, ::std::string &match_error) {
  if (value.empty()) {
    return true;
  }
  return IsSelectedCharsWithLength8_32(value, match_error);
}

Statuses Validate(const SnmpConfig &config) {
  Statuses s;
  s.AddErrorStatuses(ValidateBasicParameter(config));
  s.AddErrorStatuses(ValidateCommunityParameter(config));
  s.AddErrorStatuses(ValidateTrapV1V2cParameter(config));
  s.AddErrorStatuses(ValidateUserParameter(config));
  s.AddErrorStatuses(ValidateTrapV3Parameter(config));

  return s;
}

}  //wago::snmp_config_lib
