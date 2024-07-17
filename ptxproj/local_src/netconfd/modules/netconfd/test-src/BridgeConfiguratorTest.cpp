// SPDX-License-Identifier: GPL-2.0-or-later
#include "BaseTypes.hpp"

#include "BridgeConfigurator.hpp"
#include "MockIBridgeChangeEvent.hpp"
#include "MockINetDevManager.hpp"
#include "Types.hpp"


namespace netconf {

class BridgeConfiguratorTest : public testing::Test {
 public:
  testing::StrictMock<MockINetDevManager> mock_netdev_manager_;
  testing::StrictMock<MockIBridgeChangeEvent> mock_bridge_change_event_;
  ::std::shared_ptr<BridgeConfigurator> bridge_configurator_;

  BridgeConfiguratorTest() {
    bridge_configurator_ = ::std::make_shared<BridgeConfigurator>(mock_netdev_manager_, mock_bridge_change_event_);
  }

  void SetUp() override {
  }

  void TearDown() override {
  }
};

TEST_F(BridgeConfiguratorTest, GetBridgeAssignedInterfaces) {

  BridgeConfig expected = { {Interface::CreateBridge("br0"), { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2") }},
                            {Interface::CreateBridge("br1"), { Interface::CreatePort("ethX11")}},
                            {Interface::CreateBridge("br2"), { Interface::CreatePort("ethX12")}}};

  Interfaces bridge_itfs = {Interface::CreateBridge("br0"), Interface::CreateBridge("br1"), Interface::CreateBridge("br2")};
  EXPECT_CALL(mock_netdev_manager_, GetInterfacesByDeviceType(::std::vector<DeviceType>{DeviceType::Bridge})).WillRepeatedly(::testing::Return(bridge_itfs));

  Interfaces br0_itfs = { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2") };
  EXPECT_CALL(mock_netdev_manager_, GetPorts(Interface::CreateBridge("br0"))).WillRepeatedly(::testing::Return(br0_itfs));

  Interfaces br1_itfs = { Interface::CreatePort("ethX11") };
  EXPECT_CALL(mock_netdev_manager_, GetPorts(Interface::CreateBridge("br1"))).WillRepeatedly(::testing::Return(br1_itfs));

  Interfaces br2_itfs = { Interface::CreatePort("ethX12") };
  EXPECT_CALL(mock_netdev_manager_, GetPorts(Interface::CreateBridge("br2"))).WillRepeatedly(::testing::Return(br2_itfs));

  //GetBridgeAssignedInterfaces
  Interfaces port_itfs = { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2"),
                           Interface::CreatePort("ethX11"), Interface::CreatePort("ethX12")};
  EXPECT_EQ(port_itfs, bridge_configurator_->GetBridgeAssignedInterfaces());
}

TEST_F(BridgeConfiguratorTest, ConfigureBridgeConfiguration_RemoveBridges) {

  Interfaces bridge_itfs = {Interface::CreateBridge("br0")};
  EXPECT_CALL(mock_netdev_manager_, GetBridgesWithAssignetPort()).WillRepeatedly(::testing::Return(bridge_itfs));
  EXPECT_CALL(mock_netdev_manager_, SetDown(Interface::CreateBridge("br0"))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, Delete(Interface::CreateBridge("br0"))).Times(1);
  EXPECT_CALL(mock_bridge_change_event_, OnBridgeRemove(Interface::CreateBridge("br0"))).Times(1);

  BridgeConfig expected = { };
  EXPECT_TRUE(bridge_configurator_->Configure(expected).IsOk());
}

TEST_F(BridgeConfiguratorTest, ConfigureBridgeConfiguration_BridgePortLeave) {

  Interfaces bridge_itfs = {Interface::CreateBridge("br0")};
  EXPECT_CALL(mock_netdev_manager_, GetBridgesWithAssignetPort()).WillRepeatedly(::testing::Return(bridge_itfs));
  Interfaces br0_itfs = { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2") };
  EXPECT_CALL(mock_netdev_manager_, GetPorts(Interface::CreateBridge("br0"))).WillRepeatedly(::testing::Return(br0_itfs));

  EXPECT_CALL(mock_netdev_manager_, BridgePortLeave(Interface::CreatePort("ethX2"))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetUp(Interface::CreateBridge("br0"))).Times(1);

  BridgeConfig expected = { {Interface::CreateBridge("br0"), { Interface::CreatePort("ethX1") }}};
  EXPECT_CALL(mock_bridge_change_event_, OnBridgeAddOrPortChange(expected.begin()->first,expected.begin()->second)).Times(1);

  EXPECT_TRUE(bridge_configurator_->Configure(expected).IsOk());
}

TEST_F(BridgeConfiguratorTest, ConfigureBridgeConfiguration_BridgePortJoin) {

  Interfaces bridge_itfs = {Interface::CreateBridge("br0")};
  EXPECT_CALL(mock_netdev_manager_, GetBridgesWithAssignetPort()).WillRepeatedly(::testing::Return(bridge_itfs));
  Interfaces br0_itfs = { Interface::CreatePort("ethX1")};
  EXPECT_CALL(mock_netdev_manager_, GetPorts(Interface::CreateBridge("br0"))).WillRepeatedly(::testing::Return(br0_itfs));

  EXPECT_CALL(mock_netdev_manager_, BridgePortJoin(Interface::CreatePort("ethX2"), Interface::CreateBridge("br0"))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetUp(Interface::CreateBridge("br0"))).Times(1);

  BridgeConfig expected = { {Interface::CreateBridge("br0"), { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2") }}};

  EXPECT_TRUE(bridge_configurator_->Configure(expected).IsOk());
}

TEST_F(BridgeConfiguratorTest, ConfigureBridgeConfiguration_AddBridge) {

  Interfaces bridge_itfs = {};
  EXPECT_CALL(mock_netdev_manager_, GetBridgesWithAssignetPort()).WillRepeatedly(::testing::Return(bridge_itfs));

  EXPECT_CALL(mock_netdev_manager_, AddInterface(Interface::CreateBridge("br0"))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, BridgePortJoin(Interface::CreatePort("ethX1"), Interface::CreateBridge("br0"))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, BridgePortJoin(Interface::CreatePort("ethX2"), Interface::CreateBridge("br0"))).Times(1);
  EXPECT_CALL(mock_netdev_manager_, SetUp(Interface::CreateBridge("br0"))).Times(1);

  BridgeConfig expected = { {Interface::CreateBridge("br0"), { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2") }}};
  EXPECT_CALL(mock_bridge_change_event_, OnBridgeAddOrPortChange(expected.begin()->first,expected.begin()->second)).Times(1);

  EXPECT_TRUE(bridge_configurator_->Configure(expected).IsOk());
}

}  // namespace netconf

//---- End of source file ------------------------------------------------------
