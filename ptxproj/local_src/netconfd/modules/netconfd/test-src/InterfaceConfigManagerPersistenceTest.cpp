// SPDX-License-Identifier: GPL-2.0-or-later

#include "CommonTestDependencies.hpp"

#include <memory>

#include "IEthernetInterfaceFactory.hpp"
#include "InterfaceConfigManager.hpp"
#include "InterfaceConfigManagerBaseTest.h"
#include "LinkInfo.hpp"
#include "MockIEthernetInterface.hpp"
#include "MockINetDevManager.hpp"
#include "MockIPersistencePortConfigs.hpp"

namespace netconf {

using testing::NiceMock;
using testing::_;
using testing::Return;
using testing::NotNull;
using testing::DoAll;
using testing::ContainerEq;


class InterfaceConfigManagerPersistenceTest : public InterfaceConfigManagerBaseTest, public testing::Test {
 public:
  NiceMock<MockINetDevManager> netdev_manager_;
  NiceMock<MockIPersistencePortConfigs> persist_portconfig_mock;
  MockIEthernetInterface ethernet_interface_mock;
  std::unique_ptr<InterfaceConfigManager> sut;

  std::unique_ptr<FakeEthernetInterfaceFactory> fake_fac_;

  NetDevs netdevs_;

  InterfaceConfigs persisted_matching_port_config;
  InterfaceConfigs persisted_missing_port_config;
  InterfaceConfigs persisted_oversized_port_config;

  InterfaceConfigs expected_port_config_oversized;
  InterfaceConfigs expected_port_config_missing;

  InterfaceConfigs new_port_configs_full;

  InterfaceConfigs new_port_configs_partial;
  InterfaceConfigs persisted_new_port_configs_partial;

  /*
   ::std::string device_name_;
   InterfaceState state_;
   Autonegotiation autoneg_;
   int speed_;
   Duplex duplex_;
   */
  void SetUp() override {
    persisted_matching_port_config.emplace_back(Interface::CreatePort("X1"), InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL, MacLearning::ON);
    persisted_matching_port_config.emplace_back(Interface::CreatePort("X2"), InterfaceState::UP, Autonegotiation::OFF, 10, Duplex::HALF, MacLearning::ON);

    persisted_missing_port_config.emplace_back(Interface::CreatePort("X1"), InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL, MacLearning::ON);

    expected_port_config_missing.emplace_back(Interface::CreatePort("X1"), InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL, MacLearning::ON);
    expected_port_config_missing.emplace_back(Interface::CreatePort("X2"), InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL, MacLearning::ON);

    persisted_oversized_port_config.emplace_back(Interface::CreatePort("X1"), InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL, MacLearning::ON);
    persisted_oversized_port_config.emplace_back(Interface::CreatePort("X2"), InterfaceState::UP, Autonegotiation::OFF, 10, Duplex::HALF, MacLearning::ON);
    persisted_oversized_port_config.emplace_back(Interface::CreatePort("X12"), InterfaceState::UP, Autonegotiation::ON, 10, Duplex::FULL, MacLearning::ON);
    persisted_oversized_port_config.emplace_back(Interface::CreatePort("X11"), InterfaceState::DOWN, Autonegotiation::OFF, 100, Duplex::FULL, MacLearning::ON);

    expected_port_config_oversized.emplace_back(Interface::CreatePort("X1"), InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL, MacLearning::ON);
    expected_port_config_oversized.emplace_back(Interface::CreatePort("X2"), InterfaceState::UP, Autonegotiation::OFF, 10, Duplex::HALF, MacLearning::ON);

    new_port_configs_partial.emplace_back(Interface::CreatePort("X2"), InterfaceState::UP, Autonegotiation::ON, 10, Duplex::FULL, MacLearning::ON);

    persisted_new_port_configs_partial.emplace_back(Interface::CreatePort("X1"), InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL, MacLearning::ON);
    persisted_new_port_configs_partial.emplace_back(Interface::CreatePort("X2"), InterfaceState::UP, Autonegotiation::ON, 10, Duplex::FULL, MacLearning::ON);

    new_port_configs_full.emplace_back(Interface::CreatePort("X1"), InterfaceState::DOWN, Autonegotiation::OFF, 100, Duplex::HALF, MacLearning::ON);
    new_port_configs_full.emplace_back(Interface::CreatePort("X2"), InterfaceState::DOWN, Autonegotiation::ON, 10, Duplex::FULL, MacLearning::ON);

    netdevs_.insert(
        netdevs_.begin(),
        {
            ::std::make_shared<NetDev>(LinkInfo{1, "ethX1", "ethernet"}),
            ::std::make_shared<NetDev>(LinkInfo{2, "ethX2", "ethernet"}),
        });

    ON_CALL(persist_portconfig_mock, Write(_)).WillByDefault(Return(Status{StatusCode::OK}));
    ON_CALL(persist_portconfig_mock, Read(_)).WillByDefault(Return(Status{StatusCode::PERSISTENCE_READ}));

    fake_fac_ = ::std::make_unique<FakeEthernetInterfaceFactory>(*this);
  }

  void InstantiateSut() {
    sut = ::std::make_unique<InterfaceConfigManager>(netdev_manager_, persist_portconfig_mock, *fake_fac_);
    sut->InitializePorts();
    EXPECT_EQ(netdevs_.size(), created_ethernet_interfaces.size());
  }
};

// TEST list
// 1. Starten ohne Persistenz Datei
// 2. Starten mit passender Persistenz Datei
// 3. Starten mit zu vielen PortConfigs in der Persistenz Datei
// 4. Starten mit zu wenig PortConfigs in der Persistenz Datei
// 6. Änderungen werden Persistiert

TEST_F(InterfaceConfigManagerPersistenceTest, StartWithoutPersistenceData) {
  EXPECT_CALL(netdev_manager_, GetNetDevs(_)).WillOnce(Return(netdevs_));
  EXPECT_CALL(persist_portconfig_mock, Read(_)).WillOnce(Return(Status{StatusCode::FILE_READ}));

  InstantiateSut();

  auto itf_X1 = FindCreatedEthernetInterface("ethX1");
  auto itf_X2 = FindCreatedEthernetInterface("ethX2");

  ASSERT_THAT(itf_X1, NotNull());
  ASSERT_THAT(itf_X2, NotNull());

  EXPECT_TRUE(itf_X1->committed_);
  EXPECT_TRUE(itf_X2->committed_);

  EXPECT_EQ(eth::Autoneg::On, itf_X1->autoneg_);
  EXPECT_EQ(eth::Autoneg::On, itf_X2->autoneg_);

  EXPECT_EQ(eth::DeviceState::Up, itf_X1->state_);
  EXPECT_EQ(eth::DeviceState::Up, itf_X2->state_);
}

ACTION_P(FillPortConfigs, port_configs) { //NOLINT [clang-diagnostic-deprecated]
  InterfaceConfigs& dst_port_cfg = arg0;
  dst_port_cfg.insert(dst_port_cfg.begin(), port_configs.begin(), port_configs.end());
}

TEST_F(InterfaceConfigManagerPersistenceTest, StartWithMatchingPersistenceData) {
  EXPECT_CALL(netdev_manager_, GetNetDevs(_)).WillOnce(Return(netdevs_));

  EXPECT_CALL(persist_portconfig_mock, Read(_))
      .WillOnce(DoAll(FillPortConfigs(persisted_matching_port_config), Return(Status{StatusCode::OK})));

  InstantiateSut();

  auto itf_X1 = FindCreatedEthernetInterface("ethX1");
  auto itf_X2 = FindCreatedEthernetInterface("ethX2");

  ASSERT_THAT(itf_X1, NotNull());
  ASSERT_THAT(itf_X2, NotNull());

  EXPECT_TRUE(itf_X1->committed_);
  EXPECT_TRUE(itf_X2->committed_);

  EXPECT_EQ(eth::Autoneg::On, itf_X1->autoneg_);
  EXPECT_EQ(eth::Autoneg::Off, itf_X2->autoneg_);

  EXPECT_EQ(eth::DeviceState::Up, itf_X1->state_);
  EXPECT_EQ(eth::DeviceState::Up, itf_X2->state_);

  EXPECT_EQ(100, itf_X1->speed_);
  EXPECT_EQ(10, itf_X2->speed_);

  EXPECT_EQ(eth::Duplex::Full, itf_X1->duplex_);
  EXPECT_EQ(eth::Duplex::Half, itf_X2->duplex_);
}

TEST_F(InterfaceConfigManagerPersistenceTest, StartWithOversizedPersistenceData) {
  EXPECT_CALL(netdev_manager_, GetNetDevs(_)).WillOnce(Return(netdevs_));
  EXPECT_CALL(persist_portconfig_mock, Read(_))
      .WillOnce(DoAll(FillPortConfigs(persisted_oversized_port_config), Return(Status{StatusCode::OK})));

  InstantiateSut();

  EXPECT_THAT(sut->GetPortConfigs(), ContainerEq(expected_port_config_oversized));

  EXPECT_EQ(2u, created_ethernet_interfaces.size());

  auto itf_X1 = FindCreatedEthernetInterface("ethX1");
  auto itf_X2 = FindCreatedEthernetInterface("ethX2");

  ASSERT_THAT(itf_X1, NotNull());
  ASSERT_THAT(itf_X2, NotNull());

  EXPECT_TRUE(itf_X1->committed_);
  EXPECT_TRUE(itf_X2->committed_);

  EXPECT_EQ(eth::Autoneg::On, itf_X1->autoneg_);
  EXPECT_EQ(eth::Autoneg::Off, itf_X2->autoneg_);

  EXPECT_EQ(eth::DeviceState::Up, itf_X1->state_);
  EXPECT_EQ(eth::DeviceState::Up, itf_X2->state_);

  EXPECT_EQ(100, itf_X1->speed_);
  EXPECT_EQ(10, itf_X2->speed_);

  EXPECT_EQ(eth::Duplex::Full, itf_X1->duplex_);
  EXPECT_EQ(eth::Duplex::Half, itf_X2->duplex_);
}

TEST_F(InterfaceConfigManagerPersistenceTest, StartMissingPortConfigsInPersistenceData) {
  EXPECT_CALL(netdev_manager_, GetNetDevs(_)).WillOnce(Return(netdevs_));
  EXPECT_CALL(persist_portconfig_mock, Read(_))
      .WillOnce(DoAll(FillPortConfigs(persisted_missing_port_config), Return(Status{StatusCode::OK})));

  InstantiateSut();

  EXPECT_THAT(sut->GetPortConfigs(), ContainerEq(expected_port_config_missing));

  auto itf_X1 = FindCreatedEthernetInterface("ethX1");
  auto itf_X2 = FindCreatedEthernetInterface("ethX2");

  ASSERT_THAT(itf_X1, NotNull());
  ASSERT_THAT(itf_X2, NotNull());

  EXPECT_TRUE(itf_X1->committed_);
  EXPECT_TRUE(itf_X2->committed_);

  EXPECT_EQ(eth::Autoneg::On, itf_X1->autoneg_);
  EXPECT_EQ(eth::Autoneg::On, itf_X2->autoneg_);

  EXPECT_EQ(eth::DeviceState::Up, itf_X1->state_);
  EXPECT_EQ(eth::DeviceState::Up, itf_X2->state_);

  EXPECT_EQ(100, itf_X1->speed_);
  EXPECT_EQ(100, itf_X2->speed_);

  EXPECT_EQ(eth::Duplex::Full, itf_X1->duplex_);
  EXPECT_EQ(eth::Duplex::Full, itf_X2->duplex_);
}

ACTION_P(CopyToVector, vector) {  //NOLINT [clang-diagnostic-deprecated]
  std::copy(arg0.begin(), arg0.end(), std::back_inserter(vector.get()));
}

TEST_F(InterfaceConfigManagerPersistenceTest, PersistPartialNewConfig) {
  EXPECT_CALL(netdev_manager_, GetNetDevs()).WillRepeatedly(Return(netdevs_));
  EXPECT_CALL(netdev_manager_, GetNetDevs(_)).WillOnce(Return(netdevs_));
  EXPECT_CALL(persist_portconfig_mock, Read(_))
      .WillOnce(DoAll(FillPortConfigs(persisted_matching_port_config), Return(Status{StatusCode::OK})));

  InstantiateSut();

  InterfaceConfigs written_configs;
  EXPECT_CALL(persist_portconfig_mock, Write(_))
      .WillOnce(DoAll(CopyToVector(std::ref(written_configs)), Return(Status{StatusCode::OK})));
  sut->Configure(new_port_configs_partial);

  EXPECT_THAT(written_configs, ContainerEq(persisted_new_port_configs_partial));

  written_configs.clear();

  EXPECT_CALL(persist_portconfig_mock, Write(_))
      .WillOnce(DoAll(CopyToVector(std::ref(written_configs)), Return(Status{StatusCode::OK})));
  sut->Configure(new_port_configs_full);
  EXPECT_THAT(written_configs, ContainerEq(new_port_configs_full));
}

}  // namespace netconf
