// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "get_snmp.hpp"

namespace wago::get_snmp_data {

using wago::snmp_config_lib::SnmpConfig;
using wago::snmp_config_lib::Access;
using wago::snmp_config_lib::Privacy;
using wago::snmp_config_lib::Community;
using wago::snmp_config_lib::AuthenticationType;
using wago::snmp_config_lib::VersionV1V2c;
using wago::snmp_config_lib::SecurityLevel;

namespace {

void ExpectParameter(::std::string &&parameter, size_t index, SnmpConfig &config,
                     Result<::std::string> expected_result) {
  auto result = GetParameter(parameter, index, config);
  EXPECT_EQ(expected_result.value_, result.value_);
  EXPECT_EQ(expected_result.status_, result.status_);
}

void ExpectParameter(::std::string &&parameter, SnmpConfig &config, Result<::std::string> expected_result) {
  auto result = GetParameter(parameter, std::nullopt, config);
  EXPECT_EQ(expected_result.value_, result.value_);
  EXPECT_EQ(expected_result.status_, result.status_);
}

}

TEST(get_snmp_test, GetBasicParameter) {

  SnmpConfig config { };

  config.sys_name_ = "TEST1";
  ExpectParameter("device-name", 0, config, Result<::std::string> { config.sys_name_ });

  config.sys_descr_ = "TEST2";
  ExpectParameter("description", 0, config, Result<::std::string> { config.sys_descr_ });

  config.sys_location_ = "TEST3";
  ExpectParameter("physical-location", 0, config, Result<::std::string> { config.sys_location_ });

  config.sys_contact_ = "TEST4";
  ExpectParameter("contact", 0, config, Result<::std::string> { config.sys_contact_ });

  config.sys_objectID_ = "TEST5";
  ExpectParameter("objectID", 0, config, Result<::std::string> { config.sys_objectID_ });

}

TEST(get_snmp_test, GetBasicParameterAndIgnoreIndex) {

  SnmpConfig config { };

  config.sys_name_ = "TEST1";
  ExpectParameter("device-name", 99, config, Result<::std::string> { config.sys_name_ });
}

TEST(get_snmp_test, UnsupportedParameter) {
  SnmpConfig config { };
  ExpectParameter("parameter_xyz", 0, config, Result<::std::string> { INVALID_PARAMETER });
}

TEST(get_snmp_test, GetV1V2cStateParameter) {
  SnmpConfig config { };

  ExpectParameter("v1-v2c-state", 0, config, Result<::std::string> { "disabled" });

  config.communities_.emplace_back(Community { "name", Access::ReadWrite });

  ExpectParameter("v1-v2c-state", 0, config, Result<::std::string> { "enabled" });

}

TEST(get_snmp_test, GetCommunitiesParameter) {

  SnmpConfig config { };

  ExpectParameter("v1-v2c-community-name", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v1-v2c-community-access", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v1-v2c-community-name", 2, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v1-v2c-community-access", 2, config, Result<::std::string> { NOT_FOUND });

  config.communities_.emplace_back("comm1", Access::ReadWrite);
  config.communities_.emplace_back("comm2", Access::ReadWrite);
  config.communities_.emplace_back("comm3", Access::ReadOnly);

  ExpectParameter("v1-v2c-community-name", 0, config, Result<::std::string> { "comm1" });
  ExpectParameter("v1-v2c-community-access", 0, config, Result<::std::string> { AccessAsString(Access::ReadWrite) });
  ExpectParameter("v1-v2c-community-name", 2, config, Result<::std::string> { "comm3" });
  ExpectParameter("v1-v2c-community-access", 2, config, Result<::std::string> { AccessAsString(Access::ReadOnly) });
}

TEST(get_snmp_test, GetFirstRWCommunitiyParameterLegacyAPI) {
  // This test is required to ensure compatibility for backup/restor in an old FW (FW<=23).
  //In the legacy FW there was only one readwrite communities and only these should be taken over with a downgrade.
  //For this purpose, v1-v2c-community-name without index is called in the backup.
  //Without index, this parameter returns the first rw community in the list or none.
  SnmpConfig config { };

  ExpectParameter("v1-v2c-community-name", config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v1-v2c-community-access", config, Result<::std::string> { NOT_FOUND });

  config.communities_.emplace_back("comm1", Access::ReadOnly);
  config.communities_.emplace_back("comm2", Access::ReadWrite);
  config.communities_.emplace_back("comm3", Access::ReadOnly);

  ExpectParameter("v1-v2c-community-name", config, Result<::std::string> { "comm2" });
  ExpectParameter("v1-v2c-community-access", config, Result<::std::string> { NOT_FOUND });
}

TEST(get_snmp_test, GetTrapReceiverV1V2cParameter) {

  SnmpConfig config { };

  ExpectParameter("v1-v2c-trap-receiver-address", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v1-v2c-trap-receiver-community-name", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v1-v2c-trap-receiver-version", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v1-v2c-trap-receiver-address", 1, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v1-v2c-trap-receiver-community-name", 1, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v1-v2c-trap-receiver-version", 1, config, Result<::std::string> { NOT_FOUND });

  config.trap_receivers_V1V2c_.emplace_back("host1", "comm1", VersionV1V2c::V1);
  config.trap_receivers_V1V2c_.emplace_back("host2", "comm2", VersionV1V2c::V2c);
  config.trap_receivers_V1V2c_.emplace_back("host3", "comm3", VersionV1V2c::V2c);

  ExpectParameter("v1-v2c-trap-receiver-address", 0, config, Result<::std::string> { "host1" });
  ExpectParameter("v1-v2c-trap-receiver-community-name", 0, config, Result<::std::string> { "comm1" });
  ExpectParameter("v1-v2c-trap-receiver-version", 0, config,
                  Result<::std::string> { VersionV1V2cAsString(VersionV1V2c::V1) });
  ExpectParameter("v1-v2c-trap-receiver-address", 1, config, Result<::std::string> { "host2" });
  ExpectParameter("v1-v2c-trap-receiver-community-name", 1, config, Result<::std::string> { "comm2" });
  ExpectParameter("v1-v2c-trap-receiver-version", 1, config,
                  Result<::std::string> { VersionV1V2cAsString(VersionV1V2c::V2c) });

  //without index means index=0
  ExpectParameter("v1-v2c-trap-receiver-address", config, Result<::std::string> { "host1" });
  ExpectParameter("v1-v2c-trap-receiver-community-name", config, Result<::std::string> { "comm1" });
  ExpectParameter("v1-v2c-trap-receiver-version", config,
                  Result<::std::string> { VersionV1V2cAsString(VersionV1V2c::V1) });
}

TEST(get_snmp_test, GetUserParameters) {
  SnmpConfig config { };

  ExpectParameter("v3-user-auth-name", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v3-user-auth-type", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v3-user-auth-key", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v3-user-privacy", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v3-user-privacy-key", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v3-user-access", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v3-user-security-level", 0, config, Result<::std::string> { NOT_FOUND });

  config.user_.emplace_back("name1", Access::ReadWrite, SecurityLevel::AuthPriv, AuthenticationType::MD5, "authKey1",
                            Privacy::AES, "privKey1");
  config.user_.emplace_back("name2", Access::ReadOnly, SecurityLevel::AuthNoPriv, AuthenticationType::SHA, "authKey2",
                            Privacy::None, "");
  config.user_.emplace_back("name3", Access::ReadWrite, SecurityLevel::NoAuthNoPriv, AuthenticationType::None, "",
                            Privacy::None, "");

  ExpectParameter("v3-user-auth-name", 0, config, Result<::std::string> { "name1" });
  ExpectParameter("v3-user-auth-type", 0, config,
                  Result<::std::string> { AuthenticationTypeAsStr(AuthenticationType::MD5) });
  ExpectParameter("v3-user-auth-key", 0, config, Result<::std::string> { "authKey1" });
  ExpectParameter("v3-user-privacy", 0, config, Result<::std::string> { PrivacyAsStr(Privacy::AES) });
  ExpectParameter("v3-user-privacy-key", 0, config, Result<::std::string> { "privKey1" });
  ExpectParameter("v3-user-access", 0, config, Result<::std::string> { AccessAsString(Access::ReadWrite) });
  ExpectParameter("v3-user-security-level", 0, config,
                  Result<::std::string> { SecurityLevelAsStr(SecurityLevel::AuthPriv) });

  ExpectParameter("v3-user-auth-name", 1, config, Result<::std::string> { "name2" });
  ExpectParameter("v3-user-auth-type", 1, config,
                  Result<::std::string> { AuthenticationTypeAsStr(AuthenticationType::SHA) });
  ExpectParameter("v3-user-auth-key", 1, config, Result<::std::string> { "authKey2" });
  ExpectParameter("v3-user-privacy", 1, config, Result<::std::string> { PrivacyAsStr(Privacy::None) });
  ExpectParameter("v3-user-privacy-key", 1, config, Result<::std::string> { "" });
  ExpectParameter("v3-user-access", 1, config, Result<::std::string> { AccessAsString(Access::ReadOnly) });
  ExpectParameter("v3-user-security-level", 1, config,
                  Result<::std::string> { SecurityLevelAsStr(SecurityLevel::AuthNoPriv) });

  ExpectParameter("v3-user-auth-name", 2, config, Result<::std::string> { "name3" });
  ExpectParameter("v3-user-auth-type", 2, config,
                  Result<::std::string> { AuthenticationTypeAsStr(AuthenticationType::None) });
  ExpectParameter("v3-user-auth-key", 2, config, Result<::std::string> { "" });
  ExpectParameter("v3-user-privacy", 2, config, Result<::std::string> { PrivacyAsStr(Privacy::None) });
  ExpectParameter("v3-user-privacy-key", 2, config, Result<::std::string> { "" });
  ExpectParameter("v3-user-access", 2, config, Result<::std::string> { AccessAsString(Access::ReadWrite) });
  ExpectParameter("v3-user-security-level", 2, config,
                  Result<::std::string> { SecurityLevelAsStr(SecurityLevel::NoAuthNoPriv) });

  //without index means index=0
  ExpectParameter("v3-user-auth-name", config, Result<::std::string> { "name1" });
  ExpectParameter("v3-user-auth-type", config,
                  Result<::std::string> { AuthenticationTypeAsStr(AuthenticationType::MD5) });
  ExpectParameter("v3-user-auth-key", config, Result<::std::string> { "authKey1" });
  ExpectParameter("v3-user-privacy", config, Result<::std::string> { PrivacyAsStr(Privacy::AES) });
  ExpectParameter("v3-user-privacy-key", config, Result<::std::string> { "privKey1" });
  ExpectParameter("v3-user-access", config, Result<::std::string> { AccessAsString(Access::ReadWrite) });
  ExpectParameter("v3-user-security-level", config,
                  Result<::std::string> { SecurityLevelAsStr(SecurityLevel::AuthPriv) });
}

TEST(get_snmp_test, GetTrapReceiverV3Parameters) {
  SnmpConfig config { };

  ExpectParameter("v3-trap-receiver-auth-name", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v3-trap-receiver-auth-type", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v3-trap-receiver-auth-key", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v3-trap-receiver-privacy", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v3-trap-receiver-privacy-key", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v3-trap-receiver-security-level", 0, config, Result<::std::string> { NOT_FOUND });
  ExpectParameter("v3-trap-receiver-host", 0, config, Result<::std::string> { NOT_FOUND });

  config.trap_receivers_V3_.emplace_back("name1", SecurityLevel::AuthPriv, AuthenticationType::MD5, "authKey1",
                                         Privacy::AES, "privKey1", "host1");
  config.trap_receivers_V3_.emplace_back("name2", SecurityLevel::AuthNoPriv, AuthenticationType::SHA, "authKey2",
                                         Privacy::None, "", "host2");
  config.trap_receivers_V3_.emplace_back("name3", SecurityLevel::NoAuthNoPriv, AuthenticationType::None, "",
                                         Privacy::None, "", "host3");

  ExpectParameter("v3-trap-receiver-auth-name", 0, config, Result<::std::string> { "name1" });
  ExpectParameter("v3-trap-receiver-auth-type", 0, config,
                  Result<::std::string> { AuthenticationTypeAsStr(AuthenticationType::MD5) });
  ExpectParameter("v3-trap-receiver-auth-key", 0, config, Result<::std::string> { "authKey1" });
  ExpectParameter("v3-trap-receiver-privacy", 0, config, Result<::std::string> { PrivacyAsStr(Privacy::AES) });
  ExpectParameter("v3-trap-receiver-privacy-key", 0, config, Result<::std::string> { "privKey1" });
  ExpectParameter("v3-trap-receiver-security-level", 0, config,
                  Result<::std::string> { SecurityLevelAsStr(SecurityLevel::AuthPriv) });
  ExpectParameter("v3-trap-receiver-host", 0, config, Result<::std::string> { "host1" });

  ExpectParameter("v3-trap-receiver-auth-name", 1, config, Result<::std::string> { "name2" });
  ExpectParameter("v3-trap-receiver-auth-type", 1, config,
                  Result<::std::string> { AuthenticationTypeAsStr(AuthenticationType::SHA) });
  ExpectParameter("v3-trap-receiver-auth-key", 1, config, Result<::std::string> { "authKey2" });
  ExpectParameter("v3-trap-receiver-privacy", 1, config, Result<::std::string> { PrivacyAsStr(Privacy::None) });
  ExpectParameter("v3-trap-receiver-privacy-key", 1, config, Result<::std::string> { "" });
  ExpectParameter("v3-trap-receiver-security-level", 1, config,
                  Result<::std::string> { SecurityLevelAsStr(SecurityLevel::AuthNoPriv) });
  ExpectParameter("v3-trap-receiver-host", 1, config, Result<::std::string> { "host2" });

  ExpectParameter("v3-trap-receiver-auth-name", 2, config, Result<::std::string> { "name3" });
  ExpectParameter("v3-trap-receiver-auth-type", 2, config,
                  Result<::std::string> { AuthenticationTypeAsStr(AuthenticationType::None) });
  ExpectParameter("v3-trap-receiver-auth-key", 2, config, Result<::std::string> { "" });
  ExpectParameter("v3-trap-receiver-privacy", 2, config, Result<::std::string> { PrivacyAsStr(Privacy::None) });
  ExpectParameter("v3-trap-receiver-privacy-key", 2, config, Result<::std::string> { "" });
  ExpectParameter("v3-trap-receiver-security-level", 2, config,
                  Result<::std::string> { SecurityLevelAsStr(SecurityLevel::NoAuthNoPriv) });
  ExpectParameter("v3-trap-receiver-host", 2, config, Result<::std::string> { "host3" });

  //without index means index=0
  ExpectParameter("v3-trap-receiver-auth-name", config, Result<::std::string> { "name1" });
  ExpectParameter("v3-trap-receiver-auth-type", config,
                  Result<::std::string> { AuthenticationTypeAsStr(AuthenticationType::MD5) });
  ExpectParameter("v3-trap-receiver-auth-key", config, Result<::std::string> { "authKey1" });
  ExpectParameter("v3-trap-receiver-privacy", config, Result<::std::string> { PrivacyAsStr(Privacy::AES) });
  ExpectParameter("v3-trap-receiver-privacy-key", config, Result<::std::string> { "privKey1" });
  ExpectParameter("v3-trap-receiver-security-level", config,
                  Result<::std::string> { SecurityLevelAsStr(SecurityLevel::AuthPriv) });
  ExpectParameter("v3-trap-receiver-host", config, Result<::std::string> { "host1" });
}

TEST(get_snmp_test, GetTrapReceiverV1V2cParametersAsJson) {
  SnmpConfig config { };

  ExpectParameter("v1-v2c-trap-receiver-list-json", 0, config, Result<::std::string> { "[ ]" });

  config.trap_receivers_V1V2c_.emplace_back("host1", "comm1", VersionV1V2c::V1);

  ::std::string expected = R"([ { "address": "host1", "communityName": "comm1", "version": "v1" } ])";
  ExpectParameter("v1-v2c-trap-receiver-list-json", 0, config, Result<::std::string> { expected });
}

TEST(get_snmp_test, GetCommunityParametersAsJson) {
  SnmpConfig config { };

  ExpectParameter("v1-v2c-community-list-json", 0, config, Result<::std::string> { "[ ]" });

  config.communities_.emplace_back("comm1", Access::ReadOnly);

  ::std::string expected = R"([ { "communityName": "comm1", "access": "read-only" } ])";
  ExpectParameter("v1-v2c-community-list-json", 0, config, Result<::std::string> { expected });

  config.communities_.emplace_back("comm2", Access::ReadWrite);

  expected =
      R"([ { "communityName": "comm1", "access": "read-only" }, { "communityName": "comm2", "access": "read-write" } ])";
  ExpectParameter("v1-v2c-community-list-json", 0, config, Result<::std::string> { expected });

}

TEST(get_snmp_test, GetUserListV3ParametersAsJson) {
  //legacy api test for v3-user-list-json. v3-user-list-json shows user and trap combination.
  SnmpConfig config { };

  //no user and trap
  ExpectParameter("v3-user-list-json", 0, config, Result<::std::string> { "[ ]" });

  //user without trap information
  config.user_.emplace_back("name1", Access::ReadWrite, SecurityLevel::AuthPriv, AuthenticationType::MD5, "authKey1",
                            Privacy::AES, "privKey1");
  ::std::string expected =
      R"([ { "authName": "name1", "authType": "MD5", "authKey": "authKey1", "privacy": "AES", "privacyKey": "privKey1", "notificationReceiver": "" } ])";
  ExpectParameter("v3-user-list-json", 0, config, Result<::std::string> { expected });

  //user with trap (notificationReceiver e.g. host) information
  config.trap_receivers_V3_.emplace_back("name1", SecurityLevel::AuthPriv, AuthenticationType::MD5, "authKey1",
                                         Privacy::AES, "privKey1", "host1");
  expected =
      R"([ { "authName": "name1", "authType": "MD5", "authKey": "authKey1", "privacy": "AES", "privacyKey": "privKey1", "notificationReceiver": "" }, { "authName": "name1", "authType": "MD5", "authKey": "authKey1", "privacy": "AES", "privacyKey": "privKey1", "notificationReceiver": "host1" } ])";
  ExpectParameter("v3-user-list-json", 0, config, Result<::std::string> { expected });
}

TEST(get_snmp_test, GetUserV3ParametersAsJson) {
  SnmpConfig config { };

  ExpectParameter("v3-users-json", 0, config, Result<::std::string> { "[ ]" });

  config.user_.emplace_back("name1", Access::ReadWrite, SecurityLevel::AuthPriv, AuthenticationType::MD5, "authKey1",
                            Privacy::AES, "privKey1");

  ::std::string expected =
      R"([ { "authName": "name1", "authType": "MD5", "authKey": "authKey1", "privacy": "AES", "privacyKey": "privKey1", "securityLevel": "auth-priv", "access": "read-write" } ])";
  ExpectParameter("v3-users-json", 0, config, Result<::std::string> { expected });

  config.user_.emplace_back("name2", Access::ReadOnly, SecurityLevel::NoAuthNoPriv, AuthenticationType::None, "",
                            Privacy::None, "");

  expected =
      R"([ { "authName": "name1", "authType": "MD5", "authKey": "authKey1", "privacy": "AES", "privacyKey": "privKey1", "securityLevel": "auth-priv", "access": "read-write" }, { "authName": "name2", "authType": "none", "authKey": "", "privacy": "none", "privacyKey": "", "securityLevel": "no-auth-no-priv", "access": "read-only" } ])";
  ExpectParameter("v3-users-json", 0, config, Result<::std::string> { expected });
}

TEST(get_snmp_test, GetTrapReceiverV3ParametersAsJson) {
  SnmpConfig config { };

  ExpectParameter("v3-trap-receivers-json", 0, config, Result<::std::string> { "[ ]" });

  config.trap_receivers_V3_.emplace_back("name1", SecurityLevel::AuthPriv, AuthenticationType::MD5, "authKey1",
                                         Privacy::DES, "privKey1", "host1");

  ::std::string expected =
      R"([ { "authName": "name1", "authType": "MD5", "authKey": "authKey1", "privacy": "DES", "privacyKey": "privKey1", "securityLevel": "auth-priv", "host": "host1" } ])";
  ExpectParameter("v3-trap-receivers-json", 0, config, Result<::std::string> { expected });

  config.trap_receivers_V3_.emplace_back("name2", SecurityLevel::NoAuthNoPriv, AuthenticationType::None, "",
                                         Privacy::None, "", "host2");

  expected =
      R"([ { "authName": "name1", "authType": "MD5", "authKey": "authKey1", "privacy": "DES", "privacyKey": "privKey1", "securityLevel": "auth-priv", "host": "host1" }, { "authName": "name2", "authType": "none", "authKey": "", "privacy": "none", "privacyKey": "", "securityLevel": "no-auth-no-priv", "host": "host2" } ])";
  ExpectParameter("v3-trap-receivers-json", 0, config, Result<::std::string> { expected });
}

TEST(get_snmp_test, GetHelpText) {

  SnmpConfig config { };

  ::std::string option = "--help";
  auto result = GetParameter(option, 0, config);
  EXPECT_FALSE(result.value_.empty());
  EXPECT_EQ(SUCCESS, result.status_);

  option = "-h";
  result = GetParameter(option, 0, config);
  EXPECT_FALSE(result.value_.empty());
  EXPECT_EQ(SUCCESS, result.status_);

}

}
// namespace wago::snmp_config
