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

#include <exception>
#include <functional>
#include <sstream>
#include <map>
#include <iostream>

#include "get_snmp.hpp"

using wago::snmp_config_lib::AuthenticationType;
using wago::snmp_config_lib::Privacy;
using wago::snmp_config_lib::SecurityLevel;
using wago::snmp_config_lib::VersionV1V2c;
using wago::snmp_config_lib::Access;
using wago::snmp_config_lib::SnmpConfig;

namespace wago::get_snmp_data {

namespace {

Result<::std::string> ShowHelpText() {

  ::std::string help_text =
      R"(
* Get SNMP configuration data *

Usage: get_snmp_data < parameter >

Basic:
  device-name | description | physical-location | contact | objectID
Community:
  v1-v2c-state | v1-v2c-community-name | v1-v2c-community-access
Trap receiver v1/v2c: 
  v1-v2c-trap-receiver-address | v1-v2c-trap-receiver-community-name | v1-v2c-trap-receiver-version
User:
  v3-user-auth-name | v3-user-auth-type | v3-user-auth-key | v3-user-privacy | v3-user-privacy-key | v3-user-access | v3-user-security-level 
Trap receiver v3:
  v3-trap-receiver-auth-name | v3-trap-receiver-auth-type | v3-trap-receiver-auth-key | v3-trap-receiver-privacy | v3-trap-receiver-privacy-key | v3-trap-receiver-security-level | v3-trap-receiver-host 
Json format:
  v1-v2c-community-list-json | v1-v2c-trap-receiver-list-json | v3-user-list-json | v3-users-json | v3-trap-receivers-json 

)";

  return Result<::std::string> { help_text };
}

bool ContainsRWCommunity(const SnmpConfig &snmpConfig) {
  for (const auto &c : snmpConfig.communities_) {
    if (c.access_ == Access::ReadWrite) {
      return true;
    }
  }
  return false;
}

Result<::std::string> GetV1V2cCommunityListJson(const SnmpConfig &snmpConfig) {
  ::std::stringstream ss;

  if (snmpConfig.communities_.empty()) {
    ss << "[ ]";
  } else {
    ss << "[ ";
    for (auto &config : snmpConfig.communities_) {
      ss << R"({ "communityName": ")" << config.name_ << R"(", )";
      ss << R"("access": ")" << AccessAsString(config.access_) << R"(" }, )";
    }
    ss.seekp(-2, std::ios_base::end);
    ss << " ]";
  }

  return Result<::std::string> { ss.str() };
}

Result<::std::string> GetV1V2cTrapReceiverListJson(const SnmpConfig &snmpConfig) {
  ::std::stringstream ss;

  if (snmpConfig.trap_receivers_V1V2c_.empty()) {
    ss << "[ ]";
  } else {
    ss << "[ ";
    for (auto &config : snmpConfig.trap_receivers_V1V2c_) {
      ss << R"({ "address": ")" << config.host_ << R"(", )";
      ss << R"("communityName": ")" << config.community_ << R"(", )";
      ss << R"("version": ")" << VersionV1V2cAsString(config.version_) << R"(" }, )";
    }
    ss.seekp(-2, std::ios_base::end);
    ss << " ]";
  }

  return Result<::std::string> { ss.str() };
}

Result<::std::string> GetV3UserListJson(const SnmpConfig &snmpConfig) {
  ::std::stringstream ss;

  if (snmpConfig.user_.empty()) {
    ss << "[ ]";
  } else {
    ss << "[ ";
    for (auto &config : snmpConfig.user_) {
      ss << R"({ "authName": ")" << config.name_ << R"(", )";
      ss << R"("authType": ")" << AuthenticationTypeAsStr(config.authentication_type_) << R"(", )";
      ss << R"("authKey": ")" << config.authentication_key_ << R"(", )";
      ss << R"("privacy": ")" << PrivacyAsStr(config.privacy_) << R"(", )";
      ss << R"("privacyKey": ")" << config.privacy_key_ << R"(", )";
      ::std::string notification_receiver { };
      ss << R"("notificationReceiver": ")" << notification_receiver << R"(" }, )";
    }
    for (auto &config : snmpConfig.trap_receivers_V3_) {
      ss << R"({ "authName": ")" << config.name_ << R"(", )";
      ss << R"("authType": ")" << AuthenticationTypeAsStr(config.authentication_type_) << R"(", )";
      ss << R"("authKey": ")" << config.authentication_key_ << R"(", )";
      ss << R"("privacy": ")" << PrivacyAsStr(config.privacy_) << R"(", )";
      ss << R"("privacyKey": ")" << config.privacy_key_ << R"(", )";
      ss << R"("notificationReceiver": ")" << config.host_ << R"(" }, )";
    }
    ss.seekp(-2, std::ios_base::end);
    ss << " ]";
  }
  return Result<::std::string> { ss.str() };
}

Result<::std::string> GetV3UsersJson(const SnmpConfig &snmpConfig) {
  ::std::stringstream ss;

  if (snmpConfig.user_.empty()) {
    ss << "[ ]";
  } else {
    ss << "[ ";
    for (auto &config : snmpConfig.user_) {
      ss << R"({ "authName": ")" << config.name_ << R"(", )";
      ss << R"("authType": ")" << AuthenticationTypeAsStr(config.authentication_type_) << R"(", )";
      ss << R"("authKey": ")" << config.authentication_key_ << R"(", )";
      ss << R"("privacy": ")" << PrivacyAsStr(config.privacy_) << R"(", )";
      ss << R"("privacyKey": ")" << config.privacy_key_ << R"(", )";
      ss << R"("securityLevel": ")" << SecurityLevelAsStr(config.security_level_) << R"(", )";
      ss << R"("access": ")" << AccessAsString(config.access_) << R"(" }, )";
    }
    ss.seekp(-2, std::ios_base::end);
    ss << " ]";
  }
  return Result<::std::string> { ss.str() };
}

Result<::std::string> GetV3TrapReceiversJson(const SnmpConfig &snmpConfig) {
  ::std::stringstream ss;

  if (snmpConfig.trap_receivers_V3_.empty()) {
    ss << "[ ]";
  } else {
    ss << "[ ";
    for (auto &config : snmpConfig.trap_receivers_V3_) {
      ss << R"({ "authName": ")" << config.name_ << R"(", )";
      ss << R"("authType": ")" << AuthenticationTypeAsStr(config.authentication_type_) << R"(", )";
      ss << R"("authKey": ")" << config.authentication_key_ << R"(", )";
      ss << R"("privacy": ")" << PrivacyAsStr(config.privacy_) << R"(", )";
      ss << R"("privacyKey": ")" << config.privacy_key_ << R"(", )";
      ss << R"("securityLevel": ")" << SecurityLevelAsStr(config.security_level_) << R"(", )";
      ss << R"("host": ")" << config.host_ << R"(" }, )";
    }
    ss.seekp(-2, std::ios_base::end);
    ss << " ]";
  }
  return Result<::std::string> { ss.str() };
}

auto GetFirstReadWriteCommunity(::std::vector<wago::snmp_config_lib::Community>& communities){
  Result<::std::string> result{SUCCESS};
  auto it = ::std::find_if(communities.cbegin(), communities.cend(), [](const auto &c) {
    return c.access_ == Access::ReadWrite;
  });
  if (it != communities.end()) {
    result = Result<::std::string> { it->name_ };
  } else {
    result = Result<::std::string> { NOT_FOUND };
  }
  return result;
}

}  // anonymous namespace

::std::string VersionV1V2cAsString(const VersionV1V2c &version) {
  return (version == VersionV1V2c::V1) ? "v1" : "v2c";
}

::std::string AccessAsString(const Access &access) {
  return (access == Access::ReadWrite) ? "read-write" : "read-only";
}

::std::string AuthenticationTypeAsStr(const AuthenticationType &type) {
  static ::std::map<AuthenticationType, ::std::string> lookup = {
    {AuthenticationType::MD5, "MD5"},
    {AuthenticationType::SHA, "SHA"},
    {AuthenticationType::SHA224, "SHA224"},
    {AuthenticationType::SHA256, "SHA256"},
    {AuthenticationType::SHA384, "SHA384"},
    {AuthenticationType::SHA512, "SHA512"},
    {AuthenticationType::None, "none"},
  };
  
  return lookup.at(type);
}

::std::string PrivacyAsStr(const Privacy &privacy) {
  static ::std::map<Privacy, ::std::string> lookup = {
    {Privacy::AES, "AES"},
    {Privacy::DES, "DES"},
    {Privacy::AES128, "AES128"},
    {Privacy::AES192, "AES192"},
    {Privacy::AES192C, "AES192C"},
    {Privacy::AES256, "AES256"},
    {Privacy::AES256C, "AES256C"},
    {Privacy::None, "none"}
  };

  return lookup.at(privacy);
}

::std::string SecurityLevelAsStr(const SecurityLevel &level) {
  switch (level) {
    case SecurityLevel::NoAuthNoPriv:
      return "no-auth-no-priv";
      break;
    case SecurityLevel::AuthNoPriv:
      return "auth-no-priv";
      break;
    default:
      return "auth-priv";
      break;
  }
}

Result<::std::string> GetParameter(::std::string &paramter_name, std::optional<size_t> index, SnmpConfig &snmp_config) {

  ::std::map<::std::string, ::std::function<Result<::std::string>()>> m;

  //Help
  m.emplace("--help", []() {
    return ShowHelpText();
  });
  m.emplace("-h", []() {
    return ShowHelpText();
  });

  //Basic Parameter
  m.emplace("device-name", [&]() {
    return Result<::std::string> { snmp_config.sys_name_ };
  });
  m.emplace("description", [&]() {
    return Result<::std::string> { snmp_config.sys_descr_ };
  });
  m.emplace("physical-location", [&]() {
    return Result<::std::string> { snmp_config.sys_location_ };
  });
  m.emplace("contact", [&]() {
    return Result<::std::string> { snmp_config.sys_contact_ };
  });
  m.emplace("objectID", [&]() {
    return Result<::std::string> { snmp_config.sys_objectID_ };
  });

  // Communities V1V2c

  m.emplace("v1-v2c-state", [&]() {
    return Result<::std::string> { (ContainsRWCommunity(snmp_config)) ? "enabled" : "disabled" };
  });
  m.emplace("v1-v2c-community-name", [&]() {
    Result<::std::string> result { NOT_FOUND };
    if (not index) {
      //legacy backup support, without an index, the call returns the first rw community in the list.
      result = GetFirstReadWriteCommunity(snmp_config.communities_);
    } else if(index.value_or(0) < snmp_config.communities_.size()){
      result = Result<::std::string> { snmp_config.communities_.at(index.value_or(0)).name_ };
    }
    return result;
  });
  m.emplace("v1-v2c-community-access", [&]() {
    if (index && index.value_or(0) < snmp_config.communities_.size()) {
      return Result<::std::string> { AccessAsString(snmp_config.communities_.at(index.value_or(0)).access_) };
    }
    return Result<::std::string> { NOT_FOUND };
  });

  //Trap Receiver V1V2c
  m.emplace("v1-v2c-trap-receiver-address", [&]() {
    if (index.value_or(0) < snmp_config.trap_receivers_V1V2c_.size()) {
      return Result<::std::string> { snmp_config.trap_receivers_V1V2c_.at(index.value_or(0)).host_ };
    }
    return Result<::std::string> { NOT_FOUND };
  });
  m.emplace("v1-v2c-trap-receiver-community-name", [&]() {
    if (index.value_or(0) < snmp_config.trap_receivers_V1V2c_.size()) {
      return Result<::std::string> { snmp_config.trap_receivers_V1V2c_.at(index.value_or(0)).community_ };
    }
    return Result<::std::string> { NOT_FOUND };
  });
  m.emplace(
      "v1-v2c-trap-receiver-version",
      [&]() {
        if (index.value_or(0) < snmp_config.trap_receivers_V1V2c_.size()) {
          return Result<::std::string> { VersionV1V2cAsString(
              snmp_config.trap_receivers_V1V2c_.at(index.value_or(0)).version_) };
        }
        return Result<::std::string> { NOT_FOUND };
      });

  //Users V3
  m.emplace("v3-user-auth-name", [&]() {
    if (index.value_or(0) < snmp_config.user_.size()) {
      return Result<::std::string> { snmp_config.user_.at(index.value_or(0)).name_ };
    }
    return Result<::std::string> { NOT_FOUND };
  });
  m.emplace(
      "v3-user-auth-type",
      [&]() {
        if (index.value_or(0) < snmp_config.user_.size()) {
          return Result<::std::string> { AuthenticationTypeAsStr(
              snmp_config.user_.at(index.value_or(0)).authentication_type_) };
        }
        return Result<::std::string> { NOT_FOUND };
      });
  m.emplace("v3-user-auth-key", [&]() {
    if (index.value_or(0) < snmp_config.user_.size()) {
      return Result<::std::string> { snmp_config.user_.at(index.value_or(0)).authentication_key_ };
    }
    return Result<::std::string> { NOT_FOUND };
  });
  m.emplace("v3-user-privacy", [&]() {
    if (index.value_or(0) < snmp_config.user_.size()) {
      return Result<::std::string> { PrivacyAsStr(snmp_config.user_.at(index.value_or(0)).privacy_) };
    }
    return Result<::std::string> { NOT_FOUND };
  });
  m.emplace("v3-user-privacy-key", [&]() {
    if (index.value_or(0) < snmp_config.user_.size()) {
      return Result<::std::string> { snmp_config.user_.at(index.value_or(0)).privacy_key_ };
    }
    return Result<::std::string> { NOT_FOUND };
  });
  m.emplace("v3-user-access", [&]() {
    if (index.value_or(0) < snmp_config.user_.size()) {
      return Result<::std::string> { AccessAsString(snmp_config.user_.at(index.value_or(0)).access_) };
    }
    return Result<::std::string> { NOT_FOUND };
  });
  m.emplace("v3-user-security-level", [&]() {
    if (index.value_or(0) < snmp_config.user_.size()) {
      return Result<::std::string> { SecurityLevelAsStr(snmp_config.user_.at(index.value_or(0)).security_level_) };
    }
    return Result<::std::string> { NOT_FOUND };
  });

  //Trap Receiver V3
  m.emplace("v3-trap-receiver-auth-name", [&]() {
    if (index.value_or(0) < snmp_config.trap_receivers_V3_.size()) {
      return Result<::std::string> { snmp_config.trap_receivers_V3_.at(index.value_or(0)).name_ };
    }
    return Result<::std::string> { NOT_FOUND };
  });
  m.emplace(
      "v3-trap-receiver-auth-type",
      [&]() {
        if (index.value_or(0) < snmp_config.trap_receivers_V3_.size()) {
          return Result<::std::string> { AuthenticationTypeAsStr(
              snmp_config.trap_receivers_V3_.at(index.value_or(0)).authentication_type_) };
        }
        return Result<::std::string> { NOT_FOUND };
      });
  m.emplace("v3-trap-receiver-auth-key", [&]() {
    if (index.value_or(0) < snmp_config.trap_receivers_V3_.size()) {
      return Result<::std::string> { snmp_config.trap_receivers_V3_.at(index.value_or(0)).authentication_key_ };
    }
    return Result<::std::string> { NOT_FOUND };
  });
  m.emplace("v3-trap-receiver-privacy", [&]() {
    if (index.value_or(0) < snmp_config.trap_receivers_V3_.size()) {
      return Result<::std::string> { PrivacyAsStr(snmp_config.trap_receivers_V3_.at(index.value_or(0)).privacy_) };
    }
    return Result<::std::string> { NOT_FOUND };
  });
  m.emplace("v3-trap-receiver-privacy-key", [&]() {
    if (index.value_or(0) < snmp_config.trap_receivers_V3_.size()) {
      return Result<::std::string> { snmp_config.trap_receivers_V3_.at(index.value_or(0)).privacy_key_ };
    }
    return Result<::std::string> { NOT_FOUND };
  });
  m.emplace(
      "v3-trap-receiver-security-level",
      [&]() {
        if (index.value_or(0) < snmp_config.trap_receivers_V3_.size()) {
          return Result<::std::string> { SecurityLevelAsStr(
              snmp_config.trap_receivers_V3_.at(index.value_or(0)).security_level_) };
        }
        return Result<::std::string> { NOT_FOUND };
      });
  m.emplace("v3-trap-receiver-host", [&]() {
    if (index.value_or(0) < snmp_config.trap_receivers_V3_.size()) {
      return Result<::std::string> { snmp_config.trap_receivers_V3_.at(index.value_or(0)).host_ };
    }
    return Result<::std::string> { NOT_FOUND };
  });

  //Json
  m.emplace("v1-v2c-trap-receiver-list-json", [&]() {
    return GetV1V2cTrapReceiverListJson(snmp_config);
  });
  m.emplace("v1-v2c-community-list-json", [&]() {
    return GetV1V2cCommunityListJson(snmp_config);
  });
  m.emplace("v3-user-list-json", [&]() {
    return GetV3UserListJson(snmp_config);
  });
  m.emplace("v3-users-json", [&]() {
    return GetV3UsersJson(snmp_config);
  });
  m.emplace("v3-trap-receivers-json", [&]() {
    return GetV3TrapReceiversJson(snmp_config);
  });

  auto it = m.find(paramter_name);
  if (it != m.end()) {
    return it->second();
  }

  return Result<::std::string> { INVALID_PARAMETER };
}

}

