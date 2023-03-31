// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "SnmpConfig.hpp"
#include "snmp_config_handler.hpp"

namespace wago::snmp_config {

using namespace wago::snmp_config_lib;  // NOLINT google-build-using-namespace
using namespace ::testing;              // NOLINT google-build-using-namespace

//  Following 3 parameters must be given together:
//       v1-v2c-community-edit: add | delete
//       v1-v2c-community-name: string without spaces, min 1, max 32 characters
//       v1-v2c-community-access: read-only | read-write

TEST(edit_community, add_community) {
  SnmpConfig config{};
  const parameter_map parameters = {
      {"v1-v2c-community-edit", "add"},
      {"v1-v2c-community-name", "community-name"},
      {"v1-v2c-community-access", "read-only"},
  };

  auto expected_community = Community{"community-name", Access::ReadOnly};

  edit_v1v2c_community(parameters, config);

  EXPECT_THAT(config.communities_, ElementsAre(expected_community));
}

TEST(edit_community, delete_community) {
  SnmpConfig config{};
  config.communities_.emplace_back("to-delete", Access::ReadOnly);
  config.communities_.emplace_back("to-keep", Access::ReadWrite);

  const parameter_map parameters = {
      {"v1-v2c-community-edit", "delete"},
      {"v1-v2c-community-name", "to-delete"},
  };

  edit_v1v2c_community(parameters, config);

  EXPECT_THAT(config.communities_, ElementsAre(Community{"to-keep", Access::ReadWrite}));
}

TEST(edit_community, delete_last_community) {
  SnmpConfig config{};
  config.communities_.emplace_back("to-delete", Access::ReadOnly);

  const parameter_map parameters = {
      {"v1-v2c-community-edit", "delete"},
      {"v1-v2c-community-name", "to-delete"},
  };

  edit_v1v2c_community(parameters, config);

  EXPECT_THAT(config.communities_, IsEmpty());
}


TEST(edit_community, edit_basic_parameter_should_not_interfere_with_add_community) {
  SnmpConfig config{};
  const parameter_map parameters = {
      {"v1-v2c-community-edit", "add"},
      {"v1-v2c-community-name", "community-name"},
      {"v1-v2c-community-access", "read-only"},
  };

  auto expected_community = Community{"community-name", Access::ReadOnly};

  edit_basic_parameter(parameters, config);
  edit_v1v2c_community(parameters, config);

  EXPECT_THAT(config.communities_, ElementsAre(expected_community));
}

TEST(edit_community, add_community_throws_when_name_is_missing) {
  SnmpConfig config{};
  const parameter_map parameters = {
      {"v1-v2c-community-edit", "add"},
      {"v1-v2c-community-access", "read-only"},
  };

  EXPECT_THROW(
      {
        try {
          edit_v1v2c_community(parameters, config);
        } catch (snmp_config_exception const& e) {
          EXPECT_STREQ("missing parameter: v1-v2c-community-name", e.what());
          EXPECT_EQ(snmp_config_error::missing_parameter, e.error());
          throw;
        }
      },
      snmp_config_exception);

  EXPECT_THAT(config.communities_, IsEmpty());
}

TEST(edit_community, add_community_throws_when_access_is_invalid) {
  SnmpConfig config{};
  const parameter_map parameters = {
      {"v1-v2c-community-edit", "add"},
      {"v1-v2c-community-name", "community-name"},
      {"v1-v2c-community-access", "invalid-access"},
  };

  EXPECT_THROW(
      {
        try {
          edit_v1v2c_community(parameters, config);
        } catch (snmp_config_exception const& e) {
          EXPECT_STREQ("invalid conversion: invalid-access", e.what());
          EXPECT_EQ(snmp_config_error::invalid_parameter, e.error());
          throw;
        }
      },
      snmp_config_exception);

  EXPECT_THAT(config.communities_, IsEmpty());
}

}  // namespace wago::snmp_config
