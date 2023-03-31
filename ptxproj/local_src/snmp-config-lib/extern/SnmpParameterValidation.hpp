#pragma once

#include <string>

#include "Status.hpp"
#include "SnmpConfig.hpp"

namespace wago::snmp_config_lib{

Statuses Validate(const SnmpConfig &config);

bool IsValidName(const ::std::string &value, ::std::string& match_error);
bool IsValidDescription(const ::std::string &value, ::std::string& match_error);
bool IsValidContact(const ::std::string &value, ::std::string& match_error);
bool IsValidLocation(const ::std::string &value, ::std::string& match_error);
bool IsValidObjectId(const ::std::string &value, ::std::string &match_error);
bool IsValidCommunityName(const ::std::string &value, ::std::string &match_error);
bool IsValidHost(const ::std::string &value, ::std::string &match_error);
bool IsValidUserName(const ::std::string &value, ::std::string &match_error);
bool IsValidAuthenticationKey(const ::std::string &value, ::std::string &match_error);
bool IsValidPrivacyKey(const ::std::string &value, ::std::string &match_error);
bool IsValidV3Name(const ::std::string &value, ::std::string &match_error);

}
