// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "SnmpConfig.hpp"
#include "snmp_config_handler.hpp"

namespace wago::snmp_config {
using namespace wago::snmp_config_lib;  // NOLINT google-build-using-namespace
using namespace ::testing;              // NOLINT google-build-using-namespace

// "Following 7 parameters must be given together:",
// "v3-edit: \"add\" | \"delete\"",
// "v3-auth-name-value: string without spaces, min 1, max 32 characters",
// "v3-auth-type-value: \"none\" | \"MD5\" | \"SHA\""
// "v3-auth-key-value: string without spaces, min 8 characters"
// "v3-privacy-value: \"none\" | \"DES\" | \"AES\" "
// "v3-privacy-key-value: string without spaces, min 8 characters; optional - if it is not given, netsnmp will use "
// "auth type value"

TEST(handle_v3_add_or_delete_user, implicit_add_user) {
  SnmpConfig config{};

  const parameter_map parameters = {{"v3-edit", "add"},
                                    {"v3-auth-name", "auth-name"},
                                    {"v3-auth-type", "sha"},
                                    {"v3-auth-key", "auth-key"},
                                    {"v3-privacy", "aes"},
                                    {"v3-privacy-key", "privacy-key"},
                                    {"v3-user-access", "read-only"}};

  User expected_user = {"auth-name",  Access::ReadOnly, SecurityLevel::AuthPriv, AuthenticationType::SHA, "auth-key",
                        Privacy::AES, "privacy-key"};

  edit_v3_user_or_trap(parameters, config);

  EXPECT_THAT(config.user_, ElementsAre(expected_user));
}

TEST(handle_v3_add_or_delete_user, add_user) {
  SnmpConfig config{};

  const parameter_map parameters = {{"v3-user-edit", "add"},        {"v3-auth-name", "auth-name"},
                                    {"v3-auth-type", "sha"},        {"v3-auth-key", "auth-key"},
                                    {"v3-privacy", "aes"},          {"v3-privacy-key", "privacy-key"},
                                    {"v3-user-access", "read-only"}};

  User expected_user = {"auth-name",  Access::ReadOnly, SecurityLevel::AuthPriv, AuthenticationType::SHA, "auth-key",
                        Privacy::AES, "privacy-key"};

  edit_v3_user_or_trap(parameters, config);

  EXPECT_THAT(config.user_, ElementsAre(expected_user));
}

TEST(handle_v3_add_or_delete_user, add_user_with_default_access) {
  SnmpConfig config{};

  const parameter_map parameters = {{"v3-edit", "add"},      {"v3-auth-name", "auth-name"},
                                    {"v3-auth-type", "sha"}, {"v3-auth-key", "auth-key"},
                                    {"v3-privacy", "aes"},   {"v3-privacy-key", "privacy-key"}};

  User expected_user = {"auth-name",  Access::ReadWrite, SecurityLevel::AuthPriv, AuthenticationType::SHA, "auth-key",
                        Privacy::AES, "privacy-key"};

  edit_v3_user_or_trap(parameters, config);

  EXPECT_THAT(config.user_, ElementsAre(expected_user));
}

TEST(handle_v3_add_or_delete_user, add_user_when_notification_receiver_is_empty) {
  SnmpConfig config{};

  const parameter_map parameters = {{"v3-edit", "add"},
                                    {"v3-auth-name", "auth-name"},
                                    {"v3-auth-type", "sha"},
                                    {"v3-auth-key", "auth-key"},
                                    {"v3-privacy", "aes"},
                                    {"v3-privacy-key", "privacy-key"},
                                    {"v3-user-access", "read-only"},
                                    {"v3-notification-receiver", ""}};

  User expected_user = {"auth-name",  Access::ReadOnly, SecurityLevel::AuthPriv, AuthenticationType::SHA, "auth-key",
                        Privacy::AES, "privacy-key"};

  edit_v3_user_or_trap(parameters, config);

  EXPECT_THAT(config.user_, ElementsAre(expected_user));
  EXPECT_THAT(config.trap_receivers_V3_, IsEmpty());
}

TEST(handle_v3_add_or_delete_user, implicit_delete_user) {
  SnmpConfig config{};

  config.user_.emplace_back("auth-name", Access::ReadWrite, SecurityLevel::AuthPriv, AuthenticationType::SHA,
                            "auth-key", Privacy::AES, "privacy-key");

  const parameter_map parameters = {{"v3-edit", "delete"}, {"v3-auth-name", "auth-name"}};

  edit_v3_user_or_trap(parameters, config);

  EXPECT_THAT(config.user_, IsEmpty());
}

TEST(handle_v3_add_or_delete_user, delete_user) {
  SnmpConfig config{};

  config.user_.emplace_back("auth-name", Access::ReadWrite, SecurityLevel::AuthPriv, AuthenticationType::SHA,
                            "auth-key", Privacy::AES, "privacy-key");

  const parameter_map parameters = {{"v3-user-edit", "delete"}, {"v3-auth-name", "auth-name"}};

  edit_v3_user_or_trap(parameters, config);

  EXPECT_THAT(config.user_, IsEmpty());
}

TEST(handle_v3_add_or_delete_user, delete_user_throws_when_name_is_missing) {
  SnmpConfig config{};
  auto user = config.user_.emplace_back("auth-name", Access::ReadWrite, SecurityLevel::AuthPriv,
                                        AuthenticationType::SHA, "auth-key", Privacy::AES, "privacy-key");

  const parameter_map parameters = {{"v3-user-edit", "delete"}};

  EXPECT_THROW(
      {
        try {
          edit_v3_user_or_trap(parameters, config);
        } catch (snmp_config_exception const& e) {
          EXPECT_STREQ("missing parameter: v3-auth-name", e.what());
          EXPECT_EQ(snmp_config_error::missing_parameter, e.error());
          throw;
        }
      },
      snmp_config_exception);

  EXPECT_THAT(config.user_, ElementsAre(user));
}

TEST(handle_v3_add_or_delete_user, add_user_throws_when_name_is_missing) {
  SnmpConfig config{};
  const parameter_map parameters = {{"v3-user-edit", "add"},
                                    {"v3-auth-type", "sha"},
                                    {"v3-auth-key", "auth-key"},
                                    {"v3-privacy", "aes"},
                                    {"v3-privacy-key", "privacy-key"}};

  EXPECT_THROW(
      {
        try {
          edit_v3_user_or_trap(parameters, config);
        } catch (snmp_config_exception const& e) {
          EXPECT_STREQ("missing parameter: v3-auth-name", e.what());
          EXPECT_EQ(snmp_config_error::missing_parameter, e.error());
          throw;
        }
      },
      snmp_config_exception);

  EXPECT_THAT(config.user_, IsEmpty());
}

TEST(handle_v3_add_or_delete_user, add_user_throws_when_auth_type_is_invalud) {
  SnmpConfig config{};
  const parameter_map parameters = {{"v3-edit", "add"},
                                    {"v3-auth-name", "auth-name"},
                                    {"v3-auth-type", "invalid-auth-type"},
                                    {"v3-auth-key", "auth-key"},
                                    {"v3-privacy", "aes"},
                                    {"v3-privacy-key", "privacy-key"},
                                    {"v3-user-access", "read-only"}};

  EXPECT_THROW(
      {
        try {
          edit_v3_user_or_trap(parameters, config);
        } catch (snmp_config_exception const& e) {
          EXPECT_STREQ("invalid conversion: invalid-auth-type", e.what());
          EXPECT_EQ(snmp_config_error::invalid_parameter, e.error());
          throw;
        }
      },
      snmp_config_exception);

  EXPECT_THAT(config.user_, IsEmpty());
}

}  // namespace wago::snmp_config
