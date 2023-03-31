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

#include "SnmpConfig.hpp"

namespace wago::snmp_config_lib {

TEST(SnmpConfigTest, compareSnmpConfigs ) {

  SnmpConfig c1;
  c1.snmp_enable_ = true;
  c1.communities_.emplace_back("name1",Access::ReadOnly);

  SnmpConfig c2;
  c2.snmp_enable_ = false;
  c2.communities_.emplace_back("name2",Access::ReadWrite);

  EXPECT_FALSE(c1 == c2);
  EXPECT_TRUE(c1 != c2);

  c2.snmp_enable_ = true;
  c2.communities_.clear();
  c2.communities_.emplace_back("name1",Access::ReadOnly);

  EXPECT_FALSE(c1 != c2);
  EXPECT_TRUE(c1 == c2);
}


}  // wago::util
