// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "SnmpConfig.hpp"
#include "snmp_config_handler.hpp"

namespace wago::snmp_config {

using namespace wago::snmp_config_lib;  // NOLINT google-build-using-namespace
using namespace ::testing;              // NOLINT google-build-using-namespace

// Following 4 parameters must be given together:
//       v1-v2c-trap-receiver-edit: add | delete
//       v1-v2c-trap-receiver-address-value: ip address format
//       v1-v2c-trap-receiver-community-name: string without spaces, min 1, max 32 characters
//       v1-v2c-trap-receiver-version-value: v1 | v2c

TEST(edit_v1v2c_trap_receiver, add_trap) {
  SnmpConfig config{};
  auto expected_trap_receiver    = TrapReceiverV1V2c{"host-address", "community-name", VersionV1V2c::V1};
  const parameter_map parameters = {
      {"v1-v2c-trap-receiver-edit", "add"},
      {"v1-v2c-trap-receiver-address", "host-address"},
      {"v1-v2c-trap-receiver-community-name", "community-name"},
      {"v1-v2c-trap-receiver-version", "v1"},
  };

  edit_v1v2c_trap_receiver(parameters, config);

  EXPECT_THAT(config.trap_receivers_V1V2c_, ElementsAre(expected_trap_receiver));
}

TEST(edit_v1v2c_trap_receiver, delete_trapreceiver_throws_if_parameter_is_missing) {
  SnmpConfig config{};
  TrapReceiverV1V2c existing {"host-address", "community-name", VersionV1V2c::V1};
  config.trap_receivers_V1V2c_.emplace_back(existing);


  const parameter_map parameters = {
      {"v1-v2c-trap-receiver-edit", "delete"},
      {"v1-v2c-trap-receiver-address", "host-address"},
  };

  EXPECT_THROW(
      {
        try {
          edit_v1v2c_trap_receiver(parameters, config);
        } catch (snmp_config_exception const& e) {
          EXPECT_STREQ("missing parameter: v1-v2c-trap-receiver-community-name", e.what());
          EXPECT_EQ(snmp_config_error::missing_parameter, e.error());
          throw;
        }
      },
      snmp_config_exception);

  EXPECT_THAT(config.trap_receivers_V1V2c_, ElementsAre(existing));
}

TEST(edit_v1v2c_trap_receiver, delete_trapreceiver) {
  SnmpConfig config{};
  config.trap_receivers_V1V2c_.emplace_back("host-address", "community-name", VersionV1V2c::V1);

  const parameter_map parameters = {
      {"v1-v2c-trap-receiver-edit", "delete"},
      {"v1-v2c-trap-receiver-address", "host-address"},
      {"v1-v2c-trap-receiver-community-name", "community-name"},
      {"v1-v2c-trap-receiver-version", "v1"},
  };
  edit_v1v2c_trap_receiver(parameters, config);

  EXPECT_THAT(config.trap_receivers_V1V2c_, IsEmpty());
}

TEST(edit_v1v2c_trap_receiver, add_throws_when_name_is_missing) {
  SnmpConfig config{};
  const parameter_map parameters = {
      {"v1-v2c-trap-receiver-edit", "add"},
      {"v1-v2c-trap-receiver-address", "host-address"},
      {"v1-v2c-trap-receiver-version", "v1"},
  };

  EXPECT_THROW(
      {
        try {
          edit_v1v2c_trap_receiver(parameters, config);
        } catch (snmp_config_exception const& e) {
          EXPECT_STREQ("missing parameter: v1-v2c-trap-receiver-community-name", e.what());
          EXPECT_EQ(snmp_config_error::missing_parameter, e.error());
          throw;
        }
      },
      snmp_config_exception);

  EXPECT_THAT(config.trap_receivers_V1V2c_, IsEmpty());
}

TEST(edit_v1v2c_trap_receiver, add_trap_throws_when_version_is_invalid) {
  SnmpConfig config{};
  const parameter_map parameters = {
      {"v1-v2c-trap-receiver-edit", "add"},
      {"v1-v2c-trap-receiver-address", "host-address"},
      {"v1-v2c-trap-receiver-community-name", "community-name"},
      {"v1-v2c-trap-receiver-version", "invalid-version"},
  };

  EXPECT_THROW(
      {
        try {
          edit_v1v2c_trap_receiver(parameters, config);
        } catch (snmp_config_exception const& e) {
          EXPECT_STREQ("invalid conversion: invalid-version", e.what());
          EXPECT_EQ(snmp_config_error::invalid_parameter, e.error());
          throw;
        }
      },
      snmp_config_exception);

  EXPECT_THAT(config.trap_receivers_V1V2c_, IsEmpty());
}

}  // namespace wago::snmp_config
