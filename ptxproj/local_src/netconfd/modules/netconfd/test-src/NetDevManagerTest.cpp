// SPDX-License-Identifier: GPL-2.0-or-later

#include <cstring>
#include <limits>
#include <memory>

#include "BaseTypes.hpp"
#include "CommonTestDependencies.hpp"
#include "DeviceType.hpp"
#include "LinkInfo.hpp"
#include "MockIDeviceTypeLabel.hpp"
#include "MockIEventManager.hpp"
#include "MockIInterfaceMonitor.hpp"
#include "MockIMacDistributor.hpp"
#include "MockINetDevConstruction.hpp"
#include "MockINetlinkLink.hpp"
#include "NetDevManager.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using testing::_;
using testing::NiceMock;
using testing::Return;

namespace netconf {

class NetDevManagerTest : public ::testing::Test {
 public:
  MockIDeviceTypeLabel mock_device_type_label_;
  NiceMock<MockIInterfaceMonitor> mock_itf_monitor_;
  ::std::unique_ptr<NetDevManager> netdev_manager_;
  ::std::shared_ptr<IInterfaceMonitor> shared_itf_monitor_;
  NiceMock<MockIMacDistributor> mock_mac_distributor_;
  NiceMock<MockIEventManager> mock_event_manager_;
  NiceMock<MockINetDevEvents> mock_netdev_event_;
  NiceMock<MockINetlinkLink> mock_netlink_link_;

  void SetUp() override {
    CreateNetdevManager(Links{});
  }

  void TearDown() override {
  }

  void CreateNetdevManager(Links links){
    shared_itf_monitor_ = ::std::shared_ptr<IInterfaceMonitor>(&mock_itf_monitor_, [](IInterfaceMonitor*) {
    });
    EXPECT_CALL(mock_netlink_link_, GetLinkInfo()).WillOnce(Return(::std::move(links)));
    EXPECT_CALL(mock_itf_monitor_, RegisterEventHandler(_));
    EXPECT_CALL(mock_netlink_link_, SetInterfaceUp("eth0"));
    netdev_manager_ = ::std::make_unique<NetDevManager>( shared_itf_monitor_, mock_event_manager_, mock_netlink_link_ );
    netdev_manager_->RegisterForNetDevConstructionEvents(mock_netdev_event_);
  }
};

TEST_F(NetDevManagerTest, ConstuctorCreatesNetDevsForEachInterface) {
  Links links = { LinkInfo { 1, "ethX1", }, LinkInfo { 2, "ethX2", }, LinkInfo { 3, "br0", "bridge" }, LinkInfo { 4,
      "dummy0", "dummy" }, LinkInfo { 5, "eth0", } };

  CreateNetdevManager(links);

  auto netdev = netdev_manager_->GetByInterface(Interface::CreatePort("ethX1"));
  EXPECT_NE(nullptr, netdev.get());
  netdev = netdev_manager_->GetByInterface(Interface::CreatePort("ethX2"));
  EXPECT_NE(nullptr, netdev.get());
  netdev = netdev_manager_->GetByInterface(Interface::CreateBridge("br0"));
  EXPECT_NE(nullptr, netdev.get());
  netdev = netdev_manager_->GetByInterface(Interface::CreateDummy("dummy0"));
  EXPECT_NE(nullptr, netdev.get());
  netdev = netdev_manager_->GetByInterface(Interface::CreateEthernet("eth0"));
  EXPECT_NE(nullptr, netdev.get());

}

TEST_F(NetDevManagerTest, ReturnsInterfaceByIndex) {
  Links links = { LinkInfo { 1, "ethX3", "ethernet" }, LinkInfo { 2, "eth0", "ethernet" } , LinkInfo { 3, "br0", "bridge" }};

  CreateNetdevManager(links);

  auto netdev = netdev_manager_->GetByIfIndex(3);
  EXPECT_NE(nullptr, netdev.get());
  EXPECT_EQ(Interface::CreateBridge("br0"), netdev->GetInterface());

}

TEST_F(NetDevManagerTest, ReturnsInterfaceByName) {
  Links links = { LinkInfo { 1, "ethX3", "ethernet" }, LinkInfo { 2, "eth0", "ethernet" } , LinkInfo { 3, "br0", "bridge" }};

  CreateNetdevManager(links);

  ::std::string port_name{"ethX3"};
  auto netdev = netdev_manager_->GetByName(port_name);
  EXPECT_NE(nullptr, netdev.get());
  EXPECT_EQ(Interface::CreatePort("ethX3"), netdev->GetInterface());

}

TEST_F(NetDevManagerTest, ReturnsBridgeNetDevs) {
  Links links = { LinkInfo { 1, "ethX3", "ethernet" }, LinkInfo { 3, "br0", "bridge" }, LinkInfo { 2, "eth0", "ethernet" } , LinkInfo { 5, "br1", "bridge" }};

  CreateNetdevManager(links);

  auto netdevs = netdev_manager_->GetNetDevs({DeviceType::Bridge});

  EXPECT_EQ(2, netdevs.size());
  EXPECT_EQ(DeviceType::Bridge, netdevs.front()->GetDeviceType());
}

TEST_F(NetDevManagerTest, ReturnsAllPortNetDevs) {
  Links links = { LinkInfo { 3, "ethX3", "ethernet" }, LinkInfo { 5, "eth0", "ethernet" }, LinkInfo { 4, "br0", "bridge" } };

  CreateNetdevManager(links);

  auto netdevs = netdev_manager_->GetNetDevs({DeviceType::Port});

  EXPECT_EQ(1, netdevs.size());
  EXPECT_EQ(Interface::CreatePort("ethX3"), netdevs.front()->GetInterface());
}

TEST_F(NetDevManagerTest, ReturnsNetDevs) {
  Links links = { LinkInfo { 1, "ethX3", "ethernet" }, LinkInfo { 3, "dummy0", "dummy" }, LinkInfo { 5, "br1", "bridge" }};

  CreateNetdevManager(links);

  auto netdevs = netdev_manager_->GetNetDevs();
  EXPECT_EQ(3, netdevs.size());

  netdevs = netdev_manager_->GetNetDevs({DeviceType::Bridge, DeviceType::Dummy});
  EXPECT_EQ(2, netdevs.size());

  netdevs = netdev_manager_->GetNetDevs({DeviceType::Dummy});
  EXPECT_EQ(1, netdevs.size());
  EXPECT_EQ(DeviceType::Dummy, netdevs.front()->GetDeviceType());
}

TEST_F(NetDevManagerTest, GetBridgesWithAssignetPort) {
  Links links = { LinkInfo { 1, "ethX1", "ethernet", "", 0, 3 }, LinkInfo { 2, "ethX2", "ethernet", "", 0, 3 },
                  LinkInfo { 3, "br0", "bridge" }, LinkInfo { 5, "br1", "bridge" }, LinkInfo { 6, "br2", "bridge" },
                  LinkInfo { 1, "dummy", "dummy", "", 0, 6 }};

  CreateNetdevManager(links);

  Interfaces bridges = netdev_manager_->GetBridgesWithAssignetPort();
  EXPECT_EQ(1, bridges.size());
  EXPECT_EQ(DeviceType::Bridge, bridges.front().GetType());
  EXPECT_EQ("br0", bridges.front().GetName());

}

TEST_F(NetDevManagerTest, ReturnsInterfaces) {
  Links links = { LinkInfo { 1, "ethX3", "ethernet" }, LinkInfo { 3, "dummy0", "dummy" }, LinkInfo { 5, "br1", "bridge" }};

  CreateNetdevManager(links);

  auto itfs = netdev_manager_->GetInterfaces();
  EXPECT_EQ(3, itfs.size());

  itfs = netdev_manager_->GetInterfacesByDeviceType({DeviceType::Bridge});
  EXPECT_EQ(Interface::CreateBridge("br1"), itfs.front());
}

TEST_F(NetDevManagerTest, DoesNotCopyNetdevObjects) {
  Links links = { LinkInfo { 3, "ethX1", "ethernet" } };

  CreateNetdevManager(links);

  auto get_by_interface = netdev_manager_->GetByInterface(Interface::CreatePort("ethX1"));
  auto get_ethernet = netdev_manager_->GetNetDevs({DeviceType::Port}).front();

  EXPECT_EQ(get_by_interface.get(), get_ethernet.get());
  EXPECT_EQ(3, get_by_interface.use_count());
}

TEST_F(NetDevManagerTest, GetParentChildRelations) {
  Links links = { LinkInfo { 1, "ethX1", "ethernet", "", 0, 3 }, LinkInfo { 2, "ethX2", "ethernet", "", 0, 3 },
                  LinkInfo { 5, "ethX3", "ethernet", "", 0, 0 }, LinkInfo { 3, "br0", "bridge" }};

  CreateNetdevManager(links);

  ::std::string ethX1{"ethX1"};
  auto netdev_ethX1 = netdev_manager_->GetByName(ethX1);
  EXPECT_NE(nullptr, netdev_ethX1.get());
  auto netdev_br0 = netdev_manager_->GetParent(netdev_ethX1);
  EXPECT_NE(nullptr, netdev_br0.get());
  EXPECT_EQ(Interface::CreateBridge("br0"), netdev_br0->GetInterface());

  ::std::string ethX2{"ethX2"};
  auto netdev_ethX2 = netdev_manager_->GetByName(ethX2);
  EXPECT_NE(nullptr, netdev_ethX2.get());
  netdev_br0 = netdev_manager_->GetParent(netdev_ethX2);
  EXPECT_NE(nullptr, netdev_br0.get());
  EXPECT_EQ(Interface::CreateBridge("br0"), netdev_br0->GetInterface());

  ::std::string br0{"br0"};
  netdev_br0 = netdev_manager_->GetByName(br0);
  EXPECT_NE(nullptr, netdev_br0.get());
  auto netdevs = netdev_manager_->GetChildren(netdev_br0);
  EXPECT_EQ(2, netdevs.size());

  EXPECT_THAT(netdevs, testing::ElementsAre(netdev_ethX1, netdev_ethX2));

  Interfaces itfs = netdev_manager_->GetPorts(Interface::CreateBridge("br0"));
  EXPECT_THAT(itfs, testing::ElementsAre(Interface::CreatePort(ethX1),Interface::CreatePort(ethX2)));
}

TEST_F(NetDevManagerTest, GetChildLinkUpInformation) {
  ::std::string br0{"br0"};

  Links links = { LinkInfo { 1, "ethX1", "ethernet", "", IFF_LOWER_UP, 3 }, LinkInfo { 2, "ethX2", "ethernet", "", 0, 3 }, LinkInfo { 3, "br0", "bridge" }};
  CreateNetdevManager(links);
  auto netdev_br0 = netdev_manager_->GetByName(br0);
  EXPECT_TRUE(netdev_manager_->IsAnyChildUp(netdev_br0));

  links = { LinkInfo { 1, "ethX1", "ethernet", "", IFF_LOWER_UP, 3 }, LinkInfo { 2, "ethX2", "ethernet", "", IFF_LOWER_UP, 3 }, LinkInfo { 3, "br0", "bridge" }};
  CreateNetdevManager(links);
  netdev_br0 = netdev_manager_->GetByName(br0);
  EXPECT_TRUE(netdev_manager_->IsAnyChildUp(netdev_br0));

  links = { LinkInfo { 1, "ethX1", "ethernet", "", 0, 3 }, LinkInfo { 2, "ethX2", "ethernet", "", 0, 3 }, LinkInfo { 3, "br0", "bridge", "", IFF_LOWER_UP }};
  CreateNetdevManager(links);
  netdev_br0 = netdev_manager_->GetByName(br0);
  EXPECT_FALSE(netdev_manager_->IsAnyChildUp(netdev_br0));
}

TEST_F(NetDevManagerTest, AddAndSetInterfaceUp) {
  LinkInfo link{ 1, "br99", "bridge", "", IFF_LOWER_UP };

  EXPECT_CALL(mock_netlink_link_, AddBridge("br99")).WillOnce(Return(Status{}));
  EXPECT_CALL(mock_netlink_link_, SetInterfaceUp("br99")).Times(1);
  EXPECT_CALL(mock_netlink_link_, GetLinkInfo(testing::Matcher<const ::std::string&>("br99"))).WillOnce(Return(link));

  Interface itf = Interface::CreateBridge("br99");
  netdev_manager_->AddInterface(itf);

  auto netdev_br99 = netdev_manager_->GetByInterface(itf);
  EXPECT_NE(nullptr, netdev_br99.get());
  EXPECT_TRUE(netdev_br99->IsLinkStateUp());

}

TEST_F(NetDevManagerTest, AddMultipleInterfaces) {

  EXPECT_CALL(mock_netlink_link_, AddBridge("br99")).WillOnce(Return(Status{}));
  EXPECT_CALL(mock_netlink_link_, AddDummy("dummy99")).WillOnce(Return(Status{}));
  EXPECT_CALL(mock_netlink_link_, AddVLAN("vlan99", 100, 1)).WillOnce(Return(Status{}));

  EXPECT_CALL(mock_netlink_link_, SetInterfaceUp(_)).Times(3);

  EXPECT_CALL(mock_netlink_link_, GetLinkInfo("br99")).WillOnce(Return(LinkInfo{1, "br99", "bridge"}));
  EXPECT_CALL(mock_netlink_link_, GetLinkInfo("dummy99")).WillOnce(Return(LinkInfo{2, "dummy99", "dummy"}));
  EXPECT_CALL(mock_netlink_link_, GetLinkInfo("vlan99")).WillOnce(Return(LinkInfo{3, "vlan99", "vlan"}));

  netdev_manager_->AddInterfaces({Interface::CreateBridge("br99"), Interface::CreateDummy("dummy99"), Interface::CreateVLAN("vlan99", 100, "br99")});

}

TEST_F(NetDevManagerTest, AddInterfaceRejectsUnsupportedDeviceTypes) {
  Status status = netdev_manager_->AddInterfaces({Interface::CreatePort("ethX1")});
  EXPECT_EQ(StatusCode::GENERIC_ERROR, status.GetStatusCode());
}

TEST_F(NetDevManagerTest, AddInterfaceRejectsVLANWhenLinkInterfaceDoesNotExist) {
  Status status = netdev_manager_->AddInterfaces({Interface::CreateVLAN("vlan1", 100, "br0")});
  EXPECT_EQ(StatusCode::GENERIC_ERROR, status.GetStatusCode());
}


TEST_F(NetDevManagerTest, DeleteDeletableInterface) {
  LinkInfo vlan0;
  vlan0.name_ = "vlan0";
  vlan0.type_ = "vlan";
  vlan0.vlanid_ = 111;
  vlan0.name_link_ = "br0";
  Links links = { LinkInfo { 1, "ethX1", "ethernet"}, LinkInfo { 3, "br0", "bridge" },
      LinkInfo { 4, "dummy0", "dummy" }, vlan0 };

  CreateNetdevManager(links);

  EXPECT_CALL(mock_netlink_link_, Delete("br0")).Times(1);
  netdev_manager_->Delete(Interface::CreateBridge("br0"));
  EXPECT_EQ(3, netdev_manager_->GetNetDevs().size());

  EXPECT_CALL(mock_netlink_link_, Delete("dummy0")).Times(1);
  netdev_manager_->Delete(Interface::CreateDummy("dummy0"));
  EXPECT_EQ(2, netdev_manager_->GetNetDevs().size());

  EXPECT_CALL(mock_netlink_link_, Delete("vlan0")).Times(1);
  netdev_manager_->Delete(Interface::CreateVLAN("vlan0", 111, "br0"));
  EXPECT_EQ(1, netdev_manager_->GetNetDevs().size());

  EXPECT_CALL(mock_netlink_link_, Delete(_)).Times(0);
  netdev_manager_->Delete(Interface::CreatePort("ethX1"));
  EXPECT_EQ(1, netdev_manager_->GetNetDevs().size());
}

TEST_F(NetDevManagerTest, DeleteMultipleInterfaces) {
  LinkInfo vlan0;
  vlan0.name_ = "vlan0";
  vlan0.type_ = "vlan";
  vlan0.vlanid_ = 111;
  vlan0.name_link_ = "br0";
  Links links = { LinkInfo { 1, "ethX1", "ethernet"}, LinkInfo { 3, "br0", "bridge" },
      LinkInfo { 4, "dummy0", "dummy" }, vlan0 };

  CreateNetdevManager(links);

  EXPECT_CALL(mock_netlink_link_, Delete("br0")).Times(1);
  EXPECT_CALL(mock_netlink_link_, Delete("dummy0")).Times(1);
  EXPECT_CALL(mock_netlink_link_, Delete("vlan0")).Times(1);

  netdev_manager_->Delete({Interface::CreateDummy("dummy0"),
                          Interface::CreateBridge("br0"),
                          Interface::CreateVLAN("vlan0", 111, "br0")});

  EXPECT_EQ(1, netdev_manager_->GetNetDevs().size());
}

TEST_F(NetDevManagerTest, SetInterfaceUp) {
  EXPECT_CALL(mock_netlink_link_, SetInterfaceUp("br0")).Times(1);
  netdev_manager_->SetUp(Interface::CreateBridge("br0"));
}

TEST_F(NetDevManagerTest, SetInterfaceDown) {
  EXPECT_CALL(mock_netlink_link_, SetInterfaceDown("br0")).Times(1);
  netdev_manager_->SetDown(Interface::CreateBridge("br0"));

}

TEST_F(NetDevManagerTest, BridgePortLeave) {

  Links links = { LinkInfo { 1, "ethX1", "ethernet", "", 0, 3 }, LinkInfo { 2, "ethX2", "ethernet", "", 0, 3 },
                  LinkInfo { 3, "br0", "bridge" }};

  CreateNetdevManager(links);

  EXPECT_CALL(mock_netlink_link_, DeleteParent(1)).Times(1);
  EXPECT_CALL(mock_netlink_link_, GetLinkInfo(testing::Matcher<int>(1))).WillOnce(Return(LinkInfo{1,"ethX1","ethernet"}));
  NetDevPtr netdev;
  EXPECT_CALL(mock_netdev_event_, OnBridgePortsChange(_)).WillOnce(::testing::SaveArg<0>(&netdev));

  netdev_manager_->BridgePortLeave(Interface::CreatePort("ethX1"));
  EXPECT_EQ(Interface::CreateBridge("br0"),netdev->GetInterface());
}

TEST_F(NetDevManagerTest, BridgePortJoin) {
  int32_t ethX2_index = 2;
  int32_t br0_index = 3;
  Links links = { LinkInfo { 1, "ethX1", "ethernet" }, LinkInfo { ethX2_index, "ethX2", "ethernet" },
                  LinkInfo { br0_index, "br0", "bridge" }};

  CreateNetdevManager(links);

  EXPECT_CALL(mock_netlink_link_, SetParent(ethX2_index, br0_index)).Times(1);
  EXPECT_CALL(mock_netlink_link_, GetLinkInfo(testing::Matcher<int>(ethX2_index))).WillOnce(Return(LinkInfo{ethX2_index,"ethX2","ethernet", "", 0, br0_index}));
  NetDevPtr netdev;
  EXPECT_CALL(mock_netdev_event_, OnBridgePortsChange(_)).WillOnce(::testing::SaveArg<0>(&netdev));

  netdev_manager_->BridgePortJoin(Interface::CreatePort("ethX2"), Interface::CreateBridge("br0"));
  EXPECT_EQ(Interface::CreateBridge("br0"),netdev->GetInterface());  

  auto netdev_port = netdev_manager_->GetByIfIndex(ethX2_index);
  EXPECT_EQ(br0_index, netdev_port->GetParentIndex());
}

TEST_F(NetDevManagerTest, BridgePortJoinFailesWhenNetdevDoesNotExist) {

  Status status = netdev_manager_->BridgePortJoin(Interface::CreatePort("ethX2"), Interface::CreateBridge("br0"));
  EXPECT_EQ(StatusCode::GENERIC_ERROR, status.GetStatusCode());
}

TEST_F(NetDevManagerTest, SetInterfaceMAC) {
  ::std::string test_mac = "11:22:33:44:55:66";
  Links links = { LinkInfo { 1, "ethX1", "ethernet", ""}};

  CreateNetdevManager(links);

  EXPECT_CALL(mock_netlink_link_, SetMac("ethX1", test_mac)).Times(1);
  EXPECT_CALL(mock_netlink_link_, GetLinkInfo(testing::Matcher<int>(1))).WillOnce(Return(LinkInfo{1,"ethX1","ethernet", "11:22:33:44:55:66"}));

  auto netdev = netdev_manager_->GetByIfIndex(1);
  netdev_manager_->SetMac(netdev, test_mac);

  EXPECT_EQ(test_mac,netdev->GetMac().ToString());
}

TEST_F(NetDevManagerTest, LinkChangeActionCHANGE) {
  Links links = { LinkInfo { 1, "br0", "bridge" }};

  CreateNetdevManager(links);

  auto new_linkinfo = LinkInfo { 1, "br0", "bridge", "11:22:33:44:55:66", IFF_LOWER_UP };

  NetDevPtr netdev;
  EXPECT_CALL(mock_netdev_event_, OnLinkChange(_)).WillOnce(::testing::SaveArg<0>(&netdev));

  netdev_manager_->LinkChange(new_linkinfo, InterfaceEventAction::CHANGE);
  EXPECT_NE(nullptr, netdev.get());
  EXPECT_EQ("11:22:33:44:55:66", netdev->GetMac().ToString());
}

MATCHER_P( IsInterface, interface, "" ) //NOLINT
{
    if ( !arg.has_value( ) )
    {
        return false;
    }
    return arg.value() == interface;
}

TEST_F(NetDevManagerTest, LinkChangeActionNEW) {
  Links links = { };

  CreateNetdevManager(links);

  auto new_linkinfo = LinkInfo { 1, "br0", "bridge" };

  NetDevPtr netdev;
  EXPECT_CALL(mock_netdev_event_, OnNetDevCreated(_)).WillOnce(::testing::SaveArg<0>(&netdev));
  EXPECT_CALL(mock_event_manager_, NotifyNetworkChanges(EventLayer::EVENT_FOLDER, IsInterface(Interface::CreateBridge("br0")))).Times(1);

  netdev_manager_->LinkChange(new_linkinfo, InterfaceEventAction::NEW);
  EXPECT_EQ("br0", netdev->GetName());
}

TEST_F(NetDevManagerTest, LinkChangeActionDEL) {
  Links links = { LinkInfo { 1, "br0", "bridge" }};

  CreateNetdevManager(links);

  auto new_linkinfo = LinkInfo { 1, "br0", "bridge" };

  NetDevPtr netdev;
  EXPECT_CALL(mock_netdev_event_, OnNetDevRemoved(_)).WillOnce(::testing::SaveArg<0>(&netdev));
  EXPECT_CALL(mock_event_manager_, NotifyNetworkChanges(EventLayer::EVENT_FOLDER, IsInterface(Interface::CreateBridge("br0")))).Times(1);

  netdev_manager_->LinkChange(new_linkinfo, InterfaceEventAction::DEL);
  EXPECT_EQ("br0", netdev->GetName());
}

TEST_F(NetDevManagerTest, NetdevExistsByInterface) {

  Links links = { LinkInfo { 1, "ethX1", "ethernet" }, LinkInfo { 2, "ethX2", "ethernet"},
                  LinkInfo { 3, "br0", "bridge" }};
  CreateNetdevManager(links);
  auto netdevs = netdev_manager_->GetNetDevs();

  EXPECT_TRUE(NetDevManager::ExistsByInterface(Interface::CreateBridge("br0"), netdevs));
  EXPECT_FALSE(NetDevManager::ExistsByInterface(Interface::CreateBridge("br1"), netdevs));

  EXPECT_TRUE(NetDevManager::DoesNotExistByInterface(Interface::CreateBridge("br1"), netdevs));
  EXPECT_FALSE(NetDevManager::DoesNotExistByInterface(Interface::CreateBridge("br0"), netdevs));
}

}  // namespace netconf
