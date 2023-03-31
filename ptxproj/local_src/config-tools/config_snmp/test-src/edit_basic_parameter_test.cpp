// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "SnmpConfig.hpp"
#include "snmp_config_handler.hpp"

namespace wago::snmp_config {
using namespace wago::snmp_config_lib;  // NOLINT google-build-using-namespace
using namespace ::testing;              // NOLINT google-build-using-namespace

// set-snmp-value: enabled | disabled"
TEST(edit_basic_parameter, snmp_enable) {
  SnmpConfig config{};
  config.snmp_enable_ = false;
  auto param          = parameter_map{
      {"set-snmp", "enabled"},
  };

  edit_basic_parameter(param, config);

  EXPECT_TRUE(config.snmp_enable_);
}

TEST(edit_basic_parameter, snmp_disable) {
  SnmpConfig config{};
  config.snmp_enable_ = true;

  auto param = parameter_map{
      {"set-snmp", "disabled"},
  };

  edit_basic_parameter(param, config);

  EXPECT_FALSE(config.snmp_enable_);
}

// "device-name-value: any string"
TEST(edit_basic_parameter, set_sys_name_) {
  SnmpConfig config{};

  auto param = parameter_map{
      {"device-name", "device-1337"},
  };

  edit_basic_parameter(param, config);

  EXPECT_EQ("device-1337", config.sys_name_);
}

TEST(edit_basic_parameter, clear_sys_name_) {
  SnmpConfig config{};
  config.sys_name_ = "device-1337";

  auto param = parameter_map{
      {"device-name", ""},
  };

  edit_basic_parameter(param, config);

  EXPECT_EQ("", config.sys_name_);
}

// "description-value: any string"
TEST(edit_basic_parameter, set_sys_descr_) {
  SnmpConfig config{};

  auto param = parameter_map{
      {"description", "1337-description"},
  };

  edit_basic_parameter(param, config);

  EXPECT_EQ("1337-description", config.sys_descr_);
}

TEST(edit_basic_parameter, clear_sys_descr_) {
  SnmpConfig config{};
  config.sys_descr_ = "1337-description";

  auto param = parameter_map{
      {"description", ""},
  };

  edit_basic_parameter(param, config);

  EXPECT_EQ("", config.sys_descr_);
}

// "physical-location-value: any string",
TEST(edit_basic_parameter, set_sys_location_) {
  SnmpConfig config{};

  auto param = parameter_map{
      {"physical-location", "1337 physical location"},
  };

  edit_basic_parameter(param, config);

  EXPECT_EQ("1337 physical location", config.sys_location_);
}

TEST(edit_basic_parameter, clear_sys_location_) {
  SnmpConfig config{};
  config.sys_location_ = "1337 physical location";

  auto param = parameter_map{
      {"physical-location", ""},
  };

  edit_basic_parameter(param, config);

  EXPECT_EQ("", config.sys_location_);
}

// "contact-value: any string",
TEST(edit_basic_parameter, set_sys_contact_) {
  SnmpConfig config{};

  auto param = parameter_map{
      {"contact", "abc123@wago.com"},
  };

  edit_basic_parameter(param, config);

  EXPECT_EQ("abc123@wago.com", config.sys_contact_);
}

TEST(edit_basic_parameter, clear_sys_contact_) {
  SnmpConfig config{};
  config.sys_contact_ = "1337-contact";

  auto param = parameter_map{
      {"contact", ""},
  };

  edit_basic_parameter(param, config);

  EXPECT_EQ("", config.sys_contact_);
}

// "objectID: any OID which represents the sysObjectID"
TEST(edit_basic_parameter, set_sys_object_id_) {
  SnmpConfig config{};

  auto param = parameter_map{
      {"objectID", "1.3.3.7.0"},
  };

  edit_basic_parameter(param, config);

  EXPECT_EQ("1.3.3.7.0", config.sys_objectID_);
}

TEST(edit_basic_parameter, clear_sys_object_id_) {
  SnmpConfig config{};
  config.sys_objectID_ = "1337-objectID";

  auto param = parameter_map{
      {"objectID", ""},
  };

  edit_basic_parameter(param, config);

  EXPECT_EQ("", config.sys_objectID_);
}

TEST(edit_basic_parameter, set_multiple_basic_parameter) {
  SnmpConfig config{};

  auto param = parameter_map{
      {"set-snmp", "enabled"},
      {"device-name", "device-1337"},
      {"description", "descrition of device"},
      {"physical-location", "location of device: 123%&/"},
      {"contact", "abc1234@wago.de"},
      {"objectID", "0.1.2.3.4.555"},
  };

  edit_basic_parameter(param, config);

  EXPECT_EQ(true, config.snmp_enable_);
  EXPECT_EQ("device-1337", config.sys_name_);
  EXPECT_EQ("descrition of device", config.sys_descr_);
  EXPECT_EQ("location of device: 123%&/", config.sys_location_);
  EXPECT_EQ("abc1234@wago.de", config.sys_contact_);
  EXPECT_EQ("0.1.2.3.4.555", config.sys_objectID_);
}

TEST(edit_basic_parameter, override_existing_community) {
  SnmpConfig config{};
  config.communities_.emplace_back("community-to-replace", Access::ReadOnly);

  auto param = parameter_map{
      {"v1-v2c-community-name", "new-community"},
  };

  edit_basic_parameter(param, config);

  EXPECT_THAT(config.communities_, ElementsAre(Community{"new-community", Access::ReadWrite}));
}

TEST(edit_basic_parameter, add_new_community) {
  SnmpConfig config{};

  auto param = parameter_map{
      {"v1-v2c-community-name", "new-community"},
  };

  edit_basic_parameter(param, config);

  EXPECT_THAT(config.communities_, ElementsAre(Community{"new-community", Access::ReadWrite}));
}

TEST(edit_basic_parameter, delete_existing_community) {
  SnmpConfig config{};
  config.communities_.emplace_back("community-to-delete", Access::ReadOnly);

  auto param = parameter_map{
      {"v1-v2c-community-name", ""},
  };

  edit_basic_parameter(param, config);

  EXPECT_THAT(config.communities_, IsEmpty());
}

// "v1-v2c-state-value: \"disabled\" | \"enabled\", in case of enabled, v1-v2c-community-name must be given",
// "v1-v2c-community-name-value: string without spaces, min 1, max 32 characters",

}  // namespace wago::snmp_config
