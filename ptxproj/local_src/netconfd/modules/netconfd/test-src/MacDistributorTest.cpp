// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     MacDistributorTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#include "CommonTestDependencies.hpp"
#include "MacDistributor.hpp"
#include <memory>
#include <unistd.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockINetDevManager.hpp"

using testing::Eq;
using testing::_;
using testing::ElementsAre;
using testing::Return;

namespace netconf {

namespace{

void SetNetDevMac(const NetDevPtr& netdev, MacAddress mac){
  auto link_info = netdev->GetLinkInfo();
  link_info.mac_ = mac.ToString();
  netdev->SetLinkInfo(link_info);
}

}

using namespace std::string_literals;

ACTION_P(PutToAssignmentList, list){
  const NetDevPtr& netdev = arg0;
  MacAddress mac = MacAddress::FromString(arg1);
  list.get().push_back( {netdev->GetInterface(), mac});
}

class AMacDistributor : public testing::Test {
 public:

  MockINetDevManager mock_netdev_manager_;

  ::std::unique_ptr<MacDistributor> mac_distributor_;
  NetDevs netdevs;

  MacAddress base_mac_ = MacAddress { { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 } };
  using AssigmentPair = std::pair<Interface, ::netconf::MacAddress>;
  std::list<AssigmentPair> mac_assingment_list;

  NetDevPtr br0 = ::std::make_shared<NetDev>(LinkInfo{1, "br0", "bridge"});
  NetDevPtr br1 = ::std::make_shared<NetDev>(LinkInfo{2, "br1", "bridge"});
  NetDevPtr br2 = ::std::make_shared<NetDev>(LinkInfo{3, "br2", "bridge"});
  NetDevPtr br3 = ::std::make_shared<NetDev>(LinkInfo{4, "br3", "bridge"});
  NetDevPtr wwan0 = ::std::make_shared<NetDev>(LinkInfo{5, "wwan0", ""});
  NetDevPtr ethX1 = ::std::make_shared<NetDev>(LinkInfo{6, "ethX1", ""});
  NetDevPtr ethX2 = ::std::make_shared<NetDev>(LinkInfo{7, "ethX2", ""});
  NetDevPtr ethX11 = ::std::make_shared<NetDev>(LinkInfo{8, "ethX11", ""});
  NetDevPtr ethX12 = ::std::make_shared<NetDev>(LinkInfo{9, "ethX12", ""});

  void SetUp() override {
    mac_assingment_list.clear();
  }

  void CreateDistributorWithMacInc(uint32_t mac_in_inc) {
    mac_distributor_ = ::std::make_unique<MacDistributor>(base_mac_, mac_in_inc, mock_netdev_manager_);
  }

  void TearDown() override {
  }

  void AssignPortsToBridge(NetDevPtr& bridge, ::std::initializer_list<NetDevPtr> ports) {
    for (auto& port : ports) {
      auto link_info = port->GetLinkInfo();
      link_info.parent_index_ = bridge->GetIndex();
      port->SetLinkInfo(link_info);
    }
  }

};

TEST_F(AMacDistributor, AssignsOnlyOneMac) {
  CreateDistributorWithMacInc(1);
  AssignPortsToBridge(br0, { ethX1, ethX2, ethX11, ethX12 });

  netdevs.assign({ br0, ethX1, ethX2, ethX11, ethX12 });

  EXPECT_CALL(mock_netdev_manager_, SetMac(_, Eq(base_mac_.ToString()))).Times(static_cast<int>(netdevs.size()));

  mac_distributor_->AssignMacs(netdevs);

}

TEST_F(AMacDistributor, AssignsBridgesSameMacs) {

  CreateDistributorWithMacInc(5);

  AssignPortsToBridge(br0, { ethX1, ethX2 });
  AssignPortsToBridge(br1, { ethX11, ethX12 });
  netdevs.assign( { br0, br1, ethX1, ethX2, ethX11, ethX12 });

  EXPECT_CALL(mock_netdev_manager_, SetMac(br0, Eq(base_mac_.ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(br1, Eq(base_mac_.ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX1, Eq(base_mac_.Increment(1).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX2, Eq(base_mac_.Increment(2).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX11, Eq(base_mac_.Increment(3).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX12, Eq(base_mac_.Increment(4).ToString()))).Times(1);

  mac_distributor_->AssignMacs(netdevs);

}

TEST_F(AMacDistributor, AssignsBridgesAndPortsDifferentMacs) {

  CreateDistributorWithMacInc(6);

  AssignPortsToBridge(br0, { ethX1, ethX2 });
  AssignPortsToBridge(br1, { ethX11, ethX12 });
  netdevs.assign( { br0, br1, ethX1, ethX2, ethX11, ethX12 });

  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX1)).WillOnce(Return(br0));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX2)).WillOnce(Return(br0));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX11)).WillOnce(Return(br1));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX12)).WillOnce(Return(br1));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br0)).WillRepeatedly(Return(NetDevs{ethX1, ethX2}));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br1)).WillRepeatedly(Return(NetDevs{ethX11, ethX12}));

  EXPECT_CALL(mock_netdev_manager_, SetMac(br0, Eq(base_mac_.ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(br1, Eq(base_mac_.Increment(1).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX1, Eq(base_mac_.Increment(2).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX2, Eq(base_mac_.Increment(3).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX11, Eq(base_mac_.Increment(4).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX12, Eq(base_mac_.Increment(5).ToString()))).Times(1);

  mac_distributor_->AssignMacs(netdevs);

}

TEST_F(AMacDistributor, AssignsBridgesWithSinglePorts) {

  CreateDistributorWithMacInc(6);

  AssignPortsToBridge(br0, { ethX1 });
  AssignPortsToBridge(br1, { ethX2 });
  AssignPortsToBridge(br2, { ethX11 });
  AssignPortsToBridge(br3, { ethX12 });
  netdevs.assign( { br0, br1, br2, br3, ethX1, ethX2, ethX11, ethX12 });

  SetNetDevMac(br0, base_mac_);
  SetNetDevMac(br1, base_mac_.Increment(1));
  SetNetDevMac(br2, base_mac_.Increment(2));
  SetNetDevMac(br3, base_mac_.Increment(3));

  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX1)).WillOnce(Return(br0));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX2)).WillOnce(Return(br1));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX11)).WillOnce(Return(br2));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX12)).WillOnce(Return(br3));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br0)).WillRepeatedly(Return(NetDevs{ethX1}));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br1)).WillRepeatedly(Return(NetDevs{ethX2}));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br2)).WillRepeatedly(Return(NetDevs{ethX11}));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br3)).WillRepeatedly(Return(NetDevs{ethX12}));

  EXPECT_CALL(mock_netdev_manager_, SetMac(br0, Eq(base_mac_.ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(br1, Eq(base_mac_.Increment(1).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(br2, Eq(base_mac_.Increment(2).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(br3, Eq(base_mac_.Increment(3).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX1, Eq(base_mac_.ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX2, Eq(base_mac_.Increment(1).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX11, Eq(base_mac_.Increment(2).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX12, Eq(base_mac_.Increment(3).ToString()))).Times(1);

  mac_distributor_->AssignMacs(netdevs);

}

TEST_F(AMacDistributor, DoesNotAssignWwan) {

  CreateDistributorWithMacInc(6);

  AssignPortsToBridge(br0, { ethX1 });
  AssignPortsToBridge(br1, { ethX2 });
  AssignPortsToBridge(br2, { ethX11 });
  AssignPortsToBridge(br3, { ethX12 });
  netdevs.assign( { br0, br1, br2, br3, ethX1, ethX2, ethX11, ethX12, wwan0 });

  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX1)).WillOnce(Return(br0));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX2)).WillOnce(Return(br1));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX11)).WillOnce(Return(br2));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX12)).WillOnce(Return(br3));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br0)).WillRepeatedly(Return(NetDevs{ethX1}));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br1)).WillRepeatedly(Return(NetDevs{ethX2}));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br2)).WillRepeatedly(Return(NetDevs{ethX11}));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br3)).WillRepeatedly(Return(NetDevs{ethX12}));

  EXPECT_CALL(mock_netdev_manager_, SetMac(_, _)).Times(8);
  EXPECT_CALL(mock_netdev_manager_, SetMac(wwan0, _)).Times(0);

  mac_distributor_->AssignMacs(netdevs);

}

TEST_F(AMacDistributor, AssignsPortsWithoutBridges) {

  CreateDistributorWithMacInc(6);

  netdevs.assign( { ethX1, ethX2, ethX11, ethX12 });

  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX1)).WillOnce(Return(nullptr));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX2)).WillOnce(Return(nullptr));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX11)).WillOnce(Return(nullptr));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX12)).WillOnce(Return(nullptr));

  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX1, Eq(base_mac_.Increment(0).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX2, Eq(base_mac_.Increment(1).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX11, Eq(base_mac_.Increment(2).ToString()))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetMac(ethX12, Eq(base_mac_.Increment(3).ToString()))).Times(1);

  mac_distributor_->AssignMacs(netdevs);

}



TEST_F(AMacDistributor, AssignsInitialMacsInInitialSwitchMode) {

  CreateDistributorWithMacInc(6);
  EXPECT_CALL(mock_netdev_manager_, SetMac(_, _)).WillRepeatedly(PutToAssignmentList(std::ref(mac_assingment_list)));

  // Assign without br1
  AssignPortsToBridge(br0, { ethX1, ethX2, ethX11, ethX12 });
  netdevs.assign( { br0, ethX1, ethX2, ethX11, ethX12 });

  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX1)).WillOnce(Return(br0));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX2)).WillOnce(Return(br0));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX11)).WillOnce(Return(br0));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX12)).WillOnce(Return(br0));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br0)).WillRepeatedly(Return(NetDevs{ethX1, ethX2, ethX11, ethX12}));

  mac_distributor_->AssignMacs(netdevs);

  EXPECT_EQ(5, mac_assingment_list.size());
  EXPECT_THAT(mac_assingment_list, ElementsAre(AssigmentPair { Interface::CreateBridge("br0"), base_mac_.Increment(0) },
                                               AssigmentPair { Interface::CreatePort("ethX1"), base_mac_.Increment(1) },
                                               AssigmentPair { Interface::CreatePort("ethX2"), base_mac_.Increment(2) },
                                               AssigmentPair { Interface::CreatePort("ethX11"), base_mac_.Increment(3) },
                                               AssigmentPair { Interface::CreatePort("ethX12"), base_mac_.Increment(4) }
                                               ));

}

TEST_F(AMacDistributor, AssignsAlwaysSameMacsToBridges) {

  CreateDistributorWithMacInc(6);
  EXPECT_CALL(mock_netdev_manager_, SetMac(_, _)).WillRepeatedly(PutToAssignmentList(std::ref(mac_assingment_list)));

  // Assign without br1
  AssignPortsToBridge(br0, { ethX1, ethX2 });
  AssignPortsToBridge(br2, { ethX11, ethX12 });
  netdevs.assign( { br0, br2, ethX1, ethX2, ethX11, ethX12 });

  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX1)).WillOnce(Return(br0));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX2)).WillOnce(Return(br0));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX11)).WillOnce(Return(br2));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX12)).WillOnce(Return(br2));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br0)).WillRepeatedly(Return(NetDevs{ethX1, ethX2}));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br2)).WillRepeatedly(Return(NetDevs{ethX11, ethX12}));

  mac_distributor_->AssignMacs(netdevs);

  EXPECT_EQ(6, mac_assingment_list.size());
  EXPECT_THAT(mac_assingment_list, ElementsAre(AssigmentPair { Interface::CreateBridge("br0"), base_mac_.Increment(0) },
                                               AssigmentPair { Interface::CreateBridge("br2"), base_mac_.Increment(2) },
                                               AssigmentPair { Interface::CreatePort("ethX1"), base_mac_.Increment(1) },
                                               AssigmentPair { Interface::CreatePort("ethX2"), base_mac_.Increment(3) },
                                               AssigmentPair { Interface::CreatePort("ethX11"), base_mac_.Increment(4) },
                                               AssigmentPair { Interface::CreatePort("ethX12"), base_mac_.Increment(5) }
                                               ));

  mac_assingment_list.clear();
  AssignPortsToBridge(br0, { ethX1 });
  AssignPortsToBridge(br1, { ethX2 });
  AssignPortsToBridge(br2, { ethX11, ethX12 });
  netdevs.assign( { br0, br1, br2, ethX1, ethX2, ethX11, ethX12 });

  SetNetDevMac(br0, base_mac_);
  SetNetDevMac(br1, base_mac_.Increment(1));
  SetNetDevMac(br2, base_mac_.Increment(2));

  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX1)).WillOnce(Return(br0));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX2)).WillOnce(Return(br1));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX11)).WillOnce(Return(br2));
  EXPECT_CALL(mock_netdev_manager_, GetParent(ethX12)).WillOnce(Return(br2));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br0)).WillRepeatedly(Return(NetDevs{ethX1}));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br1)).WillRepeatedly(Return(NetDevs{ethX2}));
  EXPECT_CALL(mock_netdev_manager_, GetChildren(br2)).WillRepeatedly(Return(NetDevs{ethX11, ethX12}));

  mac_distributor_->AssignMacs(netdevs);

  EXPECT_EQ(7, mac_assingment_list.size());

  EXPECT_THAT(mac_assingment_list, ElementsAre(AssigmentPair { Interface::CreateBridge("br0"), base_mac_.Increment(0) },
                                               AssigmentPair { Interface::CreateBridge("br1"), base_mac_.Increment(1) },
                                               AssigmentPair { Interface::CreateBridge("br2"), base_mac_.Increment(2) },
                                               AssigmentPair { Interface::CreatePort("ethX1"), base_mac_.Increment(0) },
                                               AssigmentPair { Interface::CreatePort("ethX2"), base_mac_.Increment(1) },
                                               AssigmentPair { Interface::CreatePort("ethX11"), base_mac_.Increment(3) },
                                               AssigmentPair { Interface::CreatePort("ethX12"), base_mac_.Increment(4) }
                                               ));
}

} /* namespace netconf */
