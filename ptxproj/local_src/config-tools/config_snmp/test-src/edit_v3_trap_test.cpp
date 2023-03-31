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

TEST(edit_v3_user_or_trap, implicit_add_trap) {
  SnmpConfig config{};

  const parameter_map parameters = {
      {"v3-edit", "add"},
      {"v3-auth-name", "auth-name"},
      {"v3-auth-type", "sha"},
      {"v3-auth-key", "auth-key"},
      {"v3-privacy", "aes"},
      {"v3-privacy-key", "privacy-key"},
      {"v3-notification-receiver", "notification-receiver"},
  };

  TrapReceiverV3 expected_trap = {"auth-name",  SecurityLevel::AuthPriv, AuthenticationType::SHA, "auth-key",
                                  Privacy::AES, "privacy-key",           "notification-receiver"};

  edit_v3_user_or_trap(parameters, config);

  EXPECT_THAT(config.trap_receivers_V3_, ElementsAre(expected_trap));
}

TEST(edit_v3_user_or_trap, implicit_delete_trap) {
  SnmpConfig config{};

  config.trap_receivers_V3_.emplace_back("auth-name", SecurityLevel::AuthPriv, AuthenticationType::SHA, "auth-key",
                                         Privacy::AES, "privacy-key", "host-address");

  const parameter_map parameters = {
      {"v3-edit", "delete"},
      {"v3-notification-receiver", "host-address"},
      {"v3-auth-name", "auth-name"},
  };

  edit_v3_user_or_trap(parameters, config);

  EXPECT_THAT(config.trap_receivers_V3_, IsEmpty());
}

TEST(edit_v3_user_or_trap, add_trap_receiver) {
  SnmpConfig config{};

  const parameter_map parameters = {
      {"v3-trap-receiver-edit", "add"},
      {"v3-auth-name", "auth-name"},
      {"v3-auth-type", "sha"},
      {"v3-auth-key", "auth-key"},
      {"v3-privacy", "aes"},
      {"v3-privacy-key", "privacy-key"},
      {"v3-notification-receiver", "notification-receiver"},
  };

  TrapReceiverV3 expected_trap = {"auth-name",  SecurityLevel::AuthPriv, AuthenticationType::SHA, "auth-key",
                                  Privacy::AES, "privacy-key",           "notification-receiver"};

  edit_v3_user_or_trap(parameters, config);

  EXPECT_THAT(config.trap_receivers_V3_, ElementsAre(expected_trap));
}

TEST(edit_v3_user_or_trap, delete_trap_receiver) {
  SnmpConfig config{};

  config.trap_receivers_V3_.emplace_back("auth-name", SecurityLevel::AuthPriv, AuthenticationType::SHA, "auth-key",
                                         Privacy::AES, "privacy-key", "host-address");

  const parameter_map parameters = {
      {"v3-trap-receiver-edit", "delete"},
      {"v3-auth-name", "auth-name"},
      {"v3-notification-receiver", "host-address"},
  };

  edit_v3_user_or_trap(parameters, config);

  EXPECT_THAT(config.trap_receivers_V3_, IsEmpty());
}

TEST(edit_v3_user_or_trap, add_throws_when_name_is_missing) {
  SnmpConfig config{};
  const parameter_map parameters = {
      {"v3-trap-receiver-edit", "add"},  {"v3-auth-type", "sha"},
      {"v3-auth-key", "auth-key"},       {"v3-privacy", "aes"},
      {"v3-privacy-key", "privacy-key"}, {"v3-notification-receiver", "notification-receiver"},
  };

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

  EXPECT_THAT(config.trap_receivers_V3_, IsEmpty());
}

TEST(edit_v3_user_or_trap, add_trap_throws_when_privacy_is_invalid) {
  SnmpConfig config{};
  const parameter_map parameters = {
      {"v3-trap-receiver-edit", "add"},  {"v3-auth-name", "auth-name"},
      {"v3-auth-type", "md5"},           {"v3-auth-key", "auth-key"},
      {"v3-privacy", "invalid-privacy"}, {"v3-privacy-key", "privacy-key"},
      {"v3-user-access", "read-only"},   {"v3-notification-receiver", "notification-receiver"},
  };

  EXPECT_THROW(
      {
        try {
          edit_v3_user_or_trap(parameters, config);
        } catch (snmp_config_exception const& e) {
          EXPECT_STREQ("invalid conversion: invalid-privacy", e.what());
          EXPECT_EQ(snmp_config_error::invalid_parameter, e.error());
          throw;
        }
      },
      snmp_config_exception);

  EXPECT_THAT(config.trap_receivers_V3_, IsEmpty());
}

}  // namespace wago::snmp_config
