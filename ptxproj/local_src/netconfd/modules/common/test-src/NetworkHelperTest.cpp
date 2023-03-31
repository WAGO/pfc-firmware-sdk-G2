// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommonTestDependencies.hpp"
#include "NetworkHelper.hpp"

#include "Types.hpp"

namespace netconf{


TEST(NetworkHelper, GetPrefix) {

  ::std::string netmask = "255.255.255.128";

  ::std::string prefix = GetPrefix(netmask);

  EXPECT_EQ("25", prefix);
}


TEST(NetworkHelper, GetNetmask) {

  uint32_t prefix = 17;

  ::std::string netmask = GetNetmask(prefix);

  EXPECT_EQ("255.255.128.0", netmask);
}

TEST(NetworkHelper, GetBroadcast) {

  Address address = "192.168.131.17";
  Netmask netmask = "255.255.128.0";

  Address broadcast = GetBroadcast(address, netmask);

  EXPECT_EQ("192.168.255.255", broadcast);
}

TEST(NetworkHelper, GetBroadcastOfZeroAddress) {

  Address address = ZeroIP;
  Netmask netmask = "255.255.255.0";

  Address broadcast = GetBroadcast(address, netmask);

  EXPECT_EQ(ZeroIP, broadcast);
}

TEST(NetworkHelper, GetBroadcastOfZeroNetmask) {

  Address address = "192.168.2.1";
  Netmask netmask = ZeroNetmask;

  Address broadcast = GetBroadcast(address, netmask);

  EXPECT_EQ(ZeroIP, broadcast);
}

}
