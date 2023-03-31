// SPDX-License-Identifier: GPL-2.0-or-later

#include <memory>

#include "CommonTestDependencies.hpp"
#include "IEthernetInterfaceFactory.hpp"
#include "InterfaceConfigManager.hpp"
#include "InterfaceConfigManagerBaseTest.h"
#include "LinkInfo.hpp"
#include "MockIEthernetInterface.hpp"
#include "MockINetDevManager.hpp"
#include "MockIPersistencePortConfigs.hpp"

using testing::_;
using testing::NiceMock;
using testing::Return;

namespace netconf {

class InterfaceConfigManagerValidateTest : public InterfaceConfigManagerBaseTest, public testing::Test {
 public:
  NiceMock<MockINetDevManager> netdev_manager_;
  NiceMock<MockIPersistencePortConfigs> persist_portconfig_mock_;
  ::std::unique_ptr<InterfaceConfigManager> sut_;

  ::std::unique_ptr<FakeEthernetInterfaceFactory> fake_fac_;

  NetDevs netdevs_;

  void SetUp() override {
    netdevs_.insert(netdevs_.begin(),
                    // FIXME(JSo): how to create LinkInfo for tests?
                    // was: NetDev(0, Interface{"ethX1"}, DeviceType::Port, false, eth::InterfaceLinkState::Down)
                    {
                        ::std::make_shared<NetDev>(LinkInfo{1, "ethX1", "ethernet"}),
                        ::std::make_shared<NetDev>(LinkInfo{2, "ethX2", "ethernet"}),
                    });

    ON_CALL(netdev_manager_, GetNetDevs(_)).WillByDefault(Return(netdevs_));
    ON_CALL(netdev_manager_, GetNetDevs()).WillByDefault(Return(netdevs_));
    ON_CALL(persist_portconfig_mock_, Write(_)).WillByDefault(Return(Status{StatusCode::OK}));
    ON_CALL(persist_portconfig_mock_, Read(_)).WillByDefault(Return(Status{StatusCode::PERSISTENCE_READ}));

    fake_fac_ = ::std::make_unique<FakeEthernetInterfaceFactory>(*this);
  }

  void InstantiateSut() {
    sut_ = ::std::make_unique<InterfaceConfigManager>(netdev_manager_, persist_portconfig_mock_, *fake_fac_);
    sut_->InitializePorts();
    EXPECT_EQ(netdevs_.size(), created_ethernet_interfaces.size());
  }
};

TEST_F(InterfaceConfigManagerValidateTest, ValidatePortConfiguration) {
  EXPECT_CALL(persist_portconfig_mock_, Read(_)).WillOnce(Return(Status{StatusCode::OK}));

  InstantiateSut();

  Status status;
  InterfaceConfigs configs;

  configs = InterfaceConfigs{
      InterfaceConfig{Interface::CreatePort("ethX1"), InterfaceState::UP, Autonegotiation::OFF, 100, Duplex::FULL}};
  status = sut_->Configure(configs);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());

  configs = InterfaceConfigs{
      InterfaceConfig{Interface::CreatePort("X0815"), InterfaceState::UP, Autonegotiation::OFF, 100, Duplex::FULL}};
  status = sut_->Configure(configs);
  EXPECT_EQ(StatusCode::INTERFACE_NOT_EXISTING, status.GetStatusCode());

  configs = InterfaceConfigs{
      InterfaceConfig{Interface::CreatePort("ethX1"), InterfaceState::UP, Autonegotiation::OFF, 777, Duplex::FULL}};
  status = sut_->Configure(configs);
  EXPECT_EQ(StatusCode::LINK_MODE_NOT_SUPPORTED, status.GetStatusCode());

  /* The user is allowed to set only speed or duplex, if auto negotiation is off */

  configs = InterfaceConfigs{
      InterfaceConfig{Interface::CreatePort("ethX1"), InterfaceState::UP, Autonegotiation::OFF, 100, Duplex::UNKNOWN}};
  status = sut_->Configure(configs);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());

  configs =
      InterfaceConfigs{InterfaceConfig{Interface::CreatePort("ethX1"), InterfaceState::UP, Autonegotiation::OFF, -1, Duplex::FULL}};
  status = sut_->Configure(configs);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());

  /* If Autoneg is on, speed and duplex are disregarded */

  configs = InterfaceConfigs{
      InterfaceConfig{Interface::CreatePort("ethX1"), InterfaceState::UP, Autonegotiation::ON, -1, Duplex::UNKNOWN}};
  status = sut_->Configure(configs);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());

  configs = InterfaceConfigs{
      InterfaceConfig{Interface::CreatePort("ethX1"), InterfaceState::UP, Autonegotiation::ON, 100, Duplex::UNKNOWN}};
  status = sut_->Configure(configs);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());
}

}  // namespace netconf
