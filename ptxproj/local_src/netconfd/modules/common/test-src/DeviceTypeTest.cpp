// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommonTestDependencies.hpp"

#include "DeviceType.hpp"
#include "CollectionUtils.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace netconf {

TEST(DeviceTypeTest, DeviceTypeIsAnyOf) {

  DeviceType type = DeviceType::Dummy;
  EXPECT_TRUE(DeviceTypeIsAnyOf(type, DeviceType::Bridge, DeviceType::Port, DeviceType::Dummy));

  type = DeviceType::Vlan;
  EXPECT_FALSE(DeviceTypeIsAnyOf(type, DeviceType::Bridge, DeviceType::Port, DeviceType::Dummy));

}

TEST(DeviceTypeTest, IsIpAddressable) {

  EXPECT_TRUE(IsIpAddressable(DeviceType::Bridge));
  EXPECT_TRUE(IsIpAddressable(DeviceType::Vlan));
  EXPECT_TRUE(IsIpAddressable(DeviceType::Dummy));
  EXPECT_TRUE(IsIpAddressable(DeviceType::Wwan));

  EXPECT_FALSE(IsIpAddressable(DeviceType::Port));
  EXPECT_FALSE(IsIpAddressable(DeviceType::Loopback));
}

TEST(DeviceTypeTest, IsDeletable) {

  EXPECT_TRUE(IsDeletable(DeviceType::Bridge));
  EXPECT_TRUE(IsDeletable(DeviceType::Vlan));
  EXPECT_TRUE(IsDeletable(DeviceType::Dummy));

  EXPECT_FALSE(IsDeletable(DeviceType::Port));
  EXPECT_FALSE(IsDeletable(DeviceType::Loopback));
}

TEST(DeviceTypeTest, DeviceTypeFromString) {

  ::std::string str = "port";
  DeviceTypes types = DeviceTypeFromString(str);
  EXPECT_EQ(1, types.size());
  EXPECT_TRUE(IsIncluded(DeviceType::Port, types));

  str = "bridge|dummy|vlan|wwan";
  types = DeviceTypeFromString(str);
  EXPECT_EQ(4, types.size());
  EXPECT_TRUE(IsIncluded(DeviceType::Bridge, types));
  EXPECT_TRUE(IsIncluded(DeviceType::Dummy, types));
  EXPECT_TRUE(IsIncluded(DeviceType::Vlan, types));
  EXPECT_TRUE(IsIncluded(DeviceType::Wwan, types));

}

TEST(DeviceTypeTest, DeviceToString) {

  EXPECT_EQ("bridge", ToString(DeviceType::Bridge));
  EXPECT_EQ("port", ToString(DeviceType::Port));
  EXPECT_EQ("vlan", ToString(DeviceType::Vlan));
}


}  // namespace netconf
