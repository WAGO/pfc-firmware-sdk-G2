// SPDX-License-Identifier: GPL-2.0-or-later

#include "CommonTestDependencies.hpp"
#include "IPLink.hpp"
#include "MockIEventManager.hpp"
#include "MockIIPConfigure.hpp"
#include "MockIGratuitousArp.hpp"
#include "MockIIPLinks.hpp"
#include "NetDev.hpp"
#include "BaseTypes.hpp"

namespace netconf {

TEST(IpLinkTest, GetAttributes) {

  int if_index = 1;
  Interface if_name = Interface::CreateOther("XXX");
  Address address = "1.1.1.1";
  Netmask netmask = "255.255.255.0";
  eth::InterfaceLinkState state = eth::InterfaceLinkState::Up;

  IPLink link(if_index, if_name, address, netmask, state);

  EXPECT_EQ(link.GetInterface(), if_name);
  EXPECT_EQ(link.GetIfIndex(), if_index);
  EXPECT_EQ(link.GetAddress(), address);
  EXPECT_FALSE(link.GetIPConfig().has_value());
  EXPECT_EQ(link.GetLinkState(), state);

  IPConfig c(if_name.GetName(), IPSource::NONE, address, netmask);
  EXPECT_EQ(link.GetCurrentIPConfig(), c);

}

TEST(IpLinkTest, SetGetIpConfig) {

  Interface if_name = Interface::CreateBridge("br0");
  IPLink link(1, if_name, "", "", eth::InterfaceLinkState::Up);

  IPSource source = IPSource::STATIC;
  Address address = "1.1.1.1";
  Netmask netmask = "255.255.255.0";
  IPConfig new_ip_config(if_name.GetName() ,source,address,netmask);
  link.SetIPConfig(new_ip_config);

  EXPECT_TRUE(link.GetIPConfig().has_value());
  EXPECT_EQ(link.GetIPConfig().value(), new_ip_config);
  EXPECT_EQ(link.GetSource(), IPSource::STATIC);
}

TEST(IpLinkTest, SetGetIP) {

  IPLink link(1, Interface::CreateBridge("br0"), "1.1.1.1", "255.0.0.0", eth::InterfaceLinkState::Up);
  auto expected_address = "3.3.3.3";
  auto expected_netmask = "255.255.255.0";
  link.SetAddress(expected_address, expected_netmask);

  EXPECT_EQ(link.GetCurrentIPConfig().address_, expected_address);
  EXPECT_EQ(link.GetCurrentIPConfig().netmask_, expected_netmask);
}

TEST(IpLinkTest, SetLinkState) {
  IPLink link(1, Interface::CreateBridge("br0"), "1.1.1.1", "255.0.0.0", eth::InterfaceLinkState::Up);
  EXPECT_EQ(link.GetLinkState(), eth::InterfaceLinkState::Up);

  link.SetLinkState(eth::InterfaceLinkState::Down);
  EXPECT_EQ(link.GetLinkState(), eth::InterfaceLinkState::Down);
}

}
