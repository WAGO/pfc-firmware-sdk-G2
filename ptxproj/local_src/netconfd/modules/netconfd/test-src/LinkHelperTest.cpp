// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "LinkHelper.hpp"
#include "BaseTypes.hpp"

namespace netconf {

TEST(LinkHelper, LinkToInterface) {

  Interface lo = LinkHelper::LinkToInterface( { 1, "loopback", "lo" });
  EXPECT_EQ(Interface::CreateLoopback("loopback"), lo);

  Interface port = LinkHelper::LinkToInterface( { 1, "ethX1", "" });
  EXPECT_EQ(Interface::CreatePort("ethX1"), port);

  Interface bridge = LinkHelper::LinkToInterface( { 1, "br0", "bridge" });
  EXPECT_EQ(Interface::CreateBridge("br0"), bridge);

  Interface dummy = LinkHelper::LinkToInterface( { 1, "dummy0", "dummy" });
  EXPECT_EQ(Interface::CreateDummy("dummy0"), dummy);

  Interface ethernet = LinkHelper::LinkToInterface( { 1, "eth0", "" });
  EXPECT_EQ(Interface::CreateEthernet("eth0"), ethernet);

  Interface wwan = LinkHelper::LinkToInterface( { 1, "wwan0", "" });
  EXPECT_EQ(Interface::CreateWwan("wwan0"), wwan);

  Interface vlan = LinkHelper::LinkToInterface( { 1, "vlan0", "vlan", "", 0, ::std::nullopt, ::std::nullopt,
      ::std::nullopt, "name_link", ::std::nullopt, 100 });
  EXPECT_EQ(Interface::CreateVLAN("vlan0", 100, "name_link"), vlan);

  //unsupported device type
  Interface bond = LinkHelper::LinkToInterface( { 1, "bond0", "bond" });
  EXPECT_EQ(Interface::CreateOther("bond0"), bond);

}

TEST(LinkHelper, InvalidVlanLinkToInterface) {
  Interface vlan_invalid_vlanid = LinkHelper::LinkToInterface( { 1, "vlan0", "vlan", "", 0, ::std::nullopt, ::std::nullopt,
      ::std::nullopt, "name_link", ::std::nullopt, ::std::nullopt });
  EXPECT_EQ(Interface::CreateOther("vlan0"), vlan_invalid_vlanid);

  Interface vlan_invalid_link = LinkHelper::LinkToInterface( { 1, "vlan0", "vlan", "", 0, ::std::nullopt, ::std::nullopt,
      ::std::nullopt, ::std::nullopt, ::std::nullopt, 100 });
  EXPECT_EQ(Interface::CreateOther("vlan0"), vlan_invalid_link);
}

}
