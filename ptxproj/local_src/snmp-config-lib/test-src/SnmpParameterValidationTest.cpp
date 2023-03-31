//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <regex>

#include "SnmpParameterValidation.hpp"

namespace wago::snmp_config_lib {

TEST(SnmpParameterValidation, validValues) {

  ::std::string valid_value = "aB!?:;.,{}[]*~@123";
  ::std::string match_error;
  EXPECT_TRUE(IsValidName(valid_value, match_error));
  EXPECT_TRUE(match_error.empty());
  EXPECT_TRUE(IsValidContact(valid_value, match_error));
  EXPECT_TRUE(match_error.empty());
  EXPECT_TRUE(IsValidDescription(valid_value, match_error));
  EXPECT_TRUE(match_error.empty());
  EXPECT_TRUE(IsValidLocation(valid_value, match_error));
  EXPECT_TRUE(match_error.empty());

  ::std::string empty_value;
  EXPECT_TRUE(IsValidName(empty_value, match_error));
  EXPECT_TRUE(match_error.empty());
  EXPECT_TRUE(IsValidContact(empty_value, match_error));
  EXPECT_TRUE(match_error.empty());
  EXPECT_TRUE(IsValidDescription(empty_value, match_error));
  EXPECT_TRUE(match_error.empty());
  EXPECT_TRUE(IsValidLocation(empty_value, match_error));
  EXPECT_TRUE(match_error.empty());
}

TEST(SnmpParameterValidation, invalidValues) {
  ::std::string match_error;

  ::std::string too_long(256, 'a');
  EXPECT_FALSE(IsValidName(too_long, match_error));
  EXPECT_EQ("the string can be up to 255 characters long.", match_error);
  EXPECT_FALSE(IsValidContact(too_long, match_error));
  EXPECT_EQ("the string can be up to 255 characters long.", match_error);
  EXPECT_FALSE(IsValidDescription(too_long, match_error));
  EXPECT_EQ("the string can be up to 255 characters long.", match_error);
  EXPECT_FALSE(IsValidLocation(too_long, match_error));
  EXPECT_EQ("the string can be up to 255 characters long.", match_error);

  ::std::string invalid_char = "$";
  EXPECT_FALSE(IsValidName(invalid_char, match_error));
  EXPECT_EQ("character '$' is not allowed.", match_error);
  EXPECT_FALSE(IsValidContact(invalid_char, match_error));
  EXPECT_EQ("character '$' is not allowed.", match_error);
  EXPECT_FALSE(IsValidDescription(invalid_char, match_error));
  EXPECT_EQ("character '$' is not allowed.", match_error);
  EXPECT_FALSE(IsValidLocation(invalid_char, match_error));
  EXPECT_EQ("character '$' is not allowed.", match_error);

  ::std::string invalid_chars_umlauts = "ä";
  EXPECT_FALSE(IsValidName(invalid_chars_umlauts, match_error));
  EXPECT_EQ("character 'ä' is not allowed.", match_error);
  EXPECT_FALSE(IsValidContact(invalid_chars_umlauts, match_error));
  EXPECT_EQ("character 'ä' is not allowed.", match_error);
  EXPECT_FALSE(IsValidDescription(invalid_chars_umlauts, match_error));
  EXPECT_EQ("character 'ä' is not allowed.", match_error);
  EXPECT_FALSE(IsValidLocation(invalid_chars_umlauts, match_error));
  EXPECT_EQ("character 'ä' is not allowed.", match_error);
}

TEST(SnmpParameterValidation, validObjectID) {
  ::std::string match_error;

  ::std::string valid_value = ".1.2.333.4.5.6";
  EXPECT_TRUE(IsValidObjectId(valid_value, match_error));
  EXPECT_TRUE(match_error.empty());

  ::std::string empty_value;
  EXPECT_TRUE(IsValidObjectId(empty_value, match_error));
  EXPECT_TRUE(match_error.empty());
}

TEST(SnmpParameterValidation, invalidObjectID) {
  ::std::string match_error;

  ::std::string invalid_value = ".a.b.c";
  EXPECT_FALSE(IsValidObjectId(invalid_value, match_error));
  EXPECT_EQ("character '.' is not allowed.", match_error);

  ::std::string too_long(129, '1');
  EXPECT_FALSE(IsValidObjectId(too_long, match_error));
  EXPECT_EQ("the string can be up to 128 characters long.", match_error);

  ::std::string missing_dot_at_beginning("1.2.3");
  EXPECT_FALSE(IsValidObjectId(missing_dot_at_beginning, match_error));
  EXPECT_EQ("character '1' is not allowed.", match_error);

  ::std::string dot_at_the_end(".1.2.3.");
  EXPECT_FALSE(IsValidObjectId(dot_at_the_end, match_error));
  EXPECT_EQ("character '.' is not allowed.", match_error);

  ::std::string double_dot(".1..2");
  EXPECT_FALSE(IsValidObjectId(double_dot, match_error));
  EXPECT_EQ("character '.' is not allowed.", match_error);
}

TEST(SnmpParameterValidation, validCommunityName) {
  ::std::string match_error;

  ::std::string valid_value = "AA09!?:;:{}()[]@";
  EXPECT_TRUE(IsValidCommunityName(valid_value, match_error));
  EXPECT_TRUE(match_error.empty());

}

TEST(SnmpParameterValidation, invalidCommunityName) {
  ::std::string match_error;

  ::std::string invalid_value = "$";
  EXPECT_FALSE(IsValidCommunityName(invalid_value, match_error));
  EXPECT_EQ("character '$' is not allowed.", match_error);

  ::std::string too_long(33, '1');
  EXPECT_FALSE(IsValidCommunityName(too_long, match_error));
  EXPECT_EQ("the string must be between 1 and 32 characters long.", match_error);

  ::std::string invalid_chars_umlauts = "äüö";
  EXPECT_FALSE(IsValidCommunityName(invalid_chars_umlauts, match_error));
  EXPECT_EQ("character 'ä' is not allowed.", match_error);

  ::std::string empty_value;
  EXPECT_FALSE(IsValidCommunityName(empty_value, match_error));
  EXPECT_EQ("the string must be between 1 and 32 characters long.", match_error);
}

TEST(SnmpParameterValidation, duplicatedCommunityEntry) {

  SnmpConfig c;
  c.communities_.emplace_back(Community { "comm1", Access::ReadOnly }); //new entry
  c.communities_.emplace_back(Community { "comm2", Access::ReadOnly }); //new entry
  c.communities_.emplace_back(Community { "comm2", Access::ReadWrite }); //new entry
  EXPECT_TRUE(Validate(c).IsOk());

  c.communities_.emplace_back(Community { "comm2", Access::ReadOnly }); //duplicated entry
  auto s = Validate(c);
  EXPECT_EQ(1, s.GetStatuses().size());
  EXPECT_EQ("duplicated entry for community: 'comm2'", s.GetStatuses().front().ToString());
}

TEST(SnmpParameterValidation, duplicatedTrapReceiverV1V2cEntry) {

  SnmpConfig c;
  c.trap_receivers_V1V2c_.emplace_back(TrapReceiverV1V2c { "host1", "comm1", VersionV1V2c::V2c });//new entry
  c.trap_receivers_V1V2c_.emplace_back(TrapReceiverV1V2c { "host1", "comm1", VersionV1V2c::V1 });//new entry
  c.trap_receivers_V1V2c_.emplace_back(TrapReceiverV1V2c { "host1", "comm2", VersionV1V2c::V1 });//new entry
  c.trap_receivers_V1V2c_.emplace_back(TrapReceiverV1V2c { "host2", "comm2", VersionV1V2c::V1 });//new entry
  EXPECT_TRUE(Validate(c).IsOk());

  c.trap_receivers_V1V2c_.emplace_back(TrapReceiverV1V2c { "host1", "comm1", VersionV1V2c::V1 });//duplicated entry
  auto s = Validate(c);
  EXPECT_EQ(1, s.GetStatuses().size());
  EXPECT_EQ("duplicated entry for trap receiver: 'host1'", s.GetStatuses().front().ToString());
}

TEST(SnmpParameterValidation, duplicatedTrapReceiverV3Name) {

  SnmpConfig c;
  c.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name", SecurityLevel::NoAuthNoPriv,
    AuthenticationType::None, "", Privacy::None, "", "192.168.1.1" });      //new entry
  c.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name1", SecurityLevel::NoAuthNoPriv,
    AuthenticationType::None, "", Privacy::None, "", "192.168.1.1" });      //new entry
  EXPECT_TRUE(Validate(c).IsOk());

  c.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name", SecurityLevel::AuthPriv,
    AuthenticationType::MD5, "12345678", Privacy::DES, "12345678", "192.168.1.1" }); //duplicated entry

  auto s = Validate(c);

  EXPECT_EQ(1, s.GetStatuses().size());
  EXPECT_EQ("duplicated trap receiver name: 'name'", s.GetStatuses().front().ToString());
}

TEST(SnmpParameterValidation, duplicatedUserName) {

  SnmpConfig c;
  c.user_.emplace_back(User { "name", Access::ReadWrite, SecurityLevel::NoAuthNoPriv,
    AuthenticationType::None, "", Privacy::None, "" });      //new entry
  c.user_.emplace_back(User { "name2", Access::ReadWrite, SecurityLevel::NoAuthNoPriv,
    AuthenticationType::None, "", Privacy::None, "" });      //new entry
  EXPECT_TRUE(Validate(c).IsOk());

  c.user_.emplace_back(User { "name", Access::ReadWrite, SecurityLevel::AuthPriv,
    AuthenticationType::MD5, "12345678", Privacy::DES, "12345678" });   //duplicated entry

  auto s = Validate(c);

  EXPECT_EQ("duplicated user name: 'name'", s.GetStatuses().front().ToString());
}

TEST(SnmpParameterValidation, validHost) {
  ::std::string match_error;

  ::std::string valid_value = "aB-.";
  EXPECT_TRUE(IsValidHost(valid_value, match_error));
  EXPECT_TRUE(match_error.empty());
}

TEST(SnmpParameterValidation, invalidHost) {
  ::std::string match_error;

  ::std::string invalid_value = "()";
  EXPECT_FALSE(IsValidHost(invalid_value, match_error));
  EXPECT_EQ("character '(' is not allowed.", match_error);

  ::std::string too_long(256, '1');
  EXPECT_FALSE(IsValidHost(too_long, match_error));
  EXPECT_EQ("the string must be between 1 and 255 characters long.", match_error);

  ::std::string empty_value;
  EXPECT_FALSE(IsValidHost(empty_value, match_error));
  EXPECT_EQ("the string must be between 1 and 255 characters long.", match_error);

  ::std::string invalid_chars_umlauts = "abüö";
  EXPECT_FALSE(IsValidHost(invalid_chars_umlauts, match_error));
  EXPECT_EQ("character 'ü' is not allowed.", match_error);

}

TEST(SnmpParameterValidation, validUserAuthenticationPrivacyKey) {
  ::std::string match_error;

  ::std::string short_value(8, 'A');
  EXPECT_TRUE(IsValidUserName(short_value, match_error));
  EXPECT_TRUE(IsValidAuthenticationKey(short_value, match_error));
  EXPECT_TRUE(IsValidPrivacyKey(short_value, match_error));

  ::std::string long_value(32, 'A');
  EXPECT_TRUE(IsValidUserName(long_value, match_error));
  EXPECT_TRUE(IsValidAuthenticationKey(long_value, match_error));
  EXPECT_TRUE(IsValidPrivacyKey(long_value, match_error));

  ::std::string valid_values("aB09!()*~'._-");
  EXPECT_TRUE(IsValidUserName(valid_values, match_error));
  EXPECT_TRUE(IsValidAuthenticationKey(valid_values, match_error));
  EXPECT_TRUE(IsValidPrivacyKey(valid_values, match_error));

  ::std::string valid_empty_value;
  EXPECT_TRUE(IsValidPrivacyKey(valid_empty_value, match_error));
  EXPECT_TRUE(match_error.empty());
}

TEST(SnmpParameterValidation, invalidUserAuthenticationPrivacyKey) {
  ::std::string match_error;

  ::std::string to_short(7, 'b');
  EXPECT_FALSE(IsValidUserName(to_short, match_error));
  EXPECT_EQ("the string must be between 8 and 32 characters long.", match_error);
  EXPECT_FALSE(IsValidAuthenticationKey(to_short, match_error));
  EXPECT_EQ("the string must be between 8 and 32 characters long.", match_error);
  EXPECT_FALSE(IsValidPrivacyKey(to_short, match_error));
  EXPECT_EQ("the string must be between 8 and 32 characters long.", match_error);

  ::std::string to_long(33, 'b');
  EXPECT_FALSE(IsValidUserName(to_long, match_error));
  EXPECT_EQ("the string must be between 8 and 32 characters long.", match_error);
  EXPECT_FALSE(IsValidAuthenticationKey(to_long, match_error));
  EXPECT_EQ("the string must be between 8 and 32 characters long.", match_error);
  EXPECT_FALSE(IsValidPrivacyKey(to_long, match_error));
  EXPECT_EQ("the string must be between 8 and 32 characters long.", match_error);

  ::std::string invalid_value = "aaaaaaaa[]";
  EXPECT_FALSE(IsValidUserName(invalid_value, match_error));
  EXPECT_EQ("character '[' is not allowed.", match_error);
  EXPECT_FALSE(IsValidAuthenticationKey(invalid_value, match_error));
  EXPECT_EQ("character '[' is not allowed.", match_error);
  EXPECT_FALSE(IsValidPrivacyKey(invalid_value, match_error));
  EXPECT_EQ("character '[' is not allowed.", match_error);

  ::std::string invalid_chars_umlauts = "HalloMöve";
  EXPECT_FALSE(IsValidUserName(invalid_chars_umlauts, match_error));
  EXPECT_EQ("character 'ö' is not allowed.", match_error);
  EXPECT_FALSE(IsValidAuthenticationKey(invalid_chars_umlauts, match_error));
  EXPECT_EQ("character 'ö' is not allowed.", match_error);
  EXPECT_FALSE(IsValidPrivacyKey(invalid_chars_umlauts, match_error));
  EXPECT_EQ("character 'ö' is not allowed.", match_error);
}

TEST(SnmpParameterValidation, validateSnmpConfig) {

  SnmpConfig config;
  config.sys_name_ = "$";  //invalid
  config.sys_location_ = "b";  //valid
  config.sys_objectID_ = "0";  //invalid
  config.communities_.emplace_back(Community { "$", Access::ReadOnly });  //invalid
  config.communities_.emplace_back(Community { "a", Access::ReadOnly });  //valid
  config.communities_.emplace_back(Community { "$$", Access::ReadWrite });  //invalid
  config.trap_receivers_V1V2c_.emplace_back(TrapReceiverV1V2c { "$", "a", VersionV1V2c::V1 });  //invalid
  config.trap_receivers_V1V2c_.emplace_back(TrapReceiverV1V2c { "b", "$$", VersionV1V2c::V2c });  //invalid
  config.trap_receivers_V1V2c_.emplace_back(TrapReceiverV1V2c { "c", "d", VersionV1V2c::V2c });  //valid

  auto result = Validate(config);

  EXPECT_FALSE(result.IsOk());
  EXPECT_EQ(6, result.GetStatuses().size());
}

TEST(SnmpParameterValidation, validateTrapV3ConfigAuthPriv) {
  SnmpConfig config;
  config.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name1234", SecurityLevel::AuthPriv,
      AuthenticationType::MD5, "aaaaaaaa", Privacy::AES, "bbbbbbbb", "192.168.1.1" });
  auto result = Validate(config);
  EXPECT_TRUE(result.IsOk());
}

TEST(SnmpParameterValidation, validateTrapV3ConfigAuthNoPriv) {
  SnmpConfig config;
  config.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name1234", SecurityLevel::AuthNoPriv,
      AuthenticationType::MD5, "aaaaaaaa", Privacy::None, "", "192.168.1.1" });
  auto result = Validate(config);
  EXPECT_TRUE(result.IsOk());
}

TEST(SnmpParameterValidation, validateTrapV3ConfigNoAuthNoPriv) {
  SnmpConfig config;
  config.trap_receivers_V3_.emplace_back(TrapReceiverV3 { "name1234", SecurityLevel::NoAuthNoPriv,
      AuthenticationType::None, "", Privacy::None, "", "192.168.1.1" });
  auto result = Validate(config);
  EXPECT_TRUE(result.IsOk());
}

TEST(SnmpParameterValidation, validateUserV3ConfigAuthPriv) {
  SnmpConfig config;
  config.user_.emplace_back(User { "name1234", Access::ReadWrite, SecurityLevel::AuthPriv, AuthenticationType::MD5,
      "aaaaaaaa", Privacy::AES, "bbbbbbbb" });
  auto result = Validate(config);
  EXPECT_TRUE(result.IsOk());
}

TEST(SnmpParameterValidation, validateUserV3ConfigAuthNoPriv) {
  SnmpConfig config;
  config.user_.emplace_back(User { "name1234", Access::ReadWrite, SecurityLevel::AuthNoPriv, AuthenticationType::MD5,
      "aaaaaaaa", Privacy::None, "" });
  auto result = Validate(config);
  EXPECT_TRUE(result.IsOk());
}

TEST(SnmpParameterValidation, validateUserV3ConfigNoAuthNoPriv) {
  SnmpConfig config;
  config.user_.emplace_back(User { "name1234", Access::ReadWrite, SecurityLevel::NoAuthNoPriv,
      AuthenticationType::None, "", Privacy::None, "" });
  auto result = Validate(config);
  EXPECT_TRUE(result.IsOk());
}

}  // wago::util
