// SPDX-License-Identifier: GPL-2.0-or-later

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <linux/ethtool.h>

#include "BaseTypes.hpp"
#include "CommonTestDependencies.hpp"
#include "InterfaceConfigurationValidator.hpp"
#include "InterfaceInformation.hpp"
#include "LinkMode.hpp"
#include "Types.hpp"

namespace netconf {

class InterfaceConfigurationValidatorTest : public testing::Test {
 public:
  InterfaceConfigurationValidatorTest() {
    itf_x1_ = Interface::CreateEthernet("ethX1");
    itf_x2_ = Interface::CreateEthernet("ethX2");

    itf_configs_.emplace_back(InterfaceConfig::DefaultConfig(itf_x1_));

    iis_.emplace_back(InterfaceInformation{
        itf_x1_, false, AutonegotiationSupported::YES,
        LinkModes{{10, Duplex::FULL}, {100, Duplex::FULL}, {1000, Duplex::FULL}, {10, Duplex::HALF}, {100, Duplex::HALF}}});
    iis_.emplace_back(InterfaceInformation{
        itf_x2_, false, AutonegotiationSupported::YES,
        LinkModes{{10, Duplex::FULL}, {100, Duplex::FULL}, {1000, Duplex::FULL}, {10, Duplex::HALF}, {100, Duplex::HALF}}});
  }

  void SetUp() override {
  }

  void TearDown() override {
  }

  InterfaceConfigurationValidator validator_;
  Interface itf_x1_;
  Interface itf_x2_;
  InterfaceConfigs itf_configs_;
  InterfaceInformations iis_;
};

TEST_F(InterfaceConfigurationValidatorTest, CheckPortExists) {
  Status status = validator_.Validate(itf_configs_, iis_);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::OK);

  auto itf_foo = Interface::CreateEthernet("foo");
  InterfaceConfigs itf_configs_for_unknown_interface;
  itf_configs_for_unknown_interface.emplace_back(InterfaceConfig::DefaultConfig(itf_foo));

  status = validator_.Validate(itf_configs_for_unknown_interface, iis_);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_NOT_EXISTING);
}

TEST_F(InterfaceConfigurationValidatorTest, CheckAutonegotiationIsNotSupported) {
  InterfaceInformations iis_no_autoneg;
  iis_no_autoneg.emplace_back(InterfaceInformation{itf_x1_, false, AutonegotiationSupported::NO, LinkModes{}});

  Status status = validator_.Validate(itf_configs_, iis_no_autoneg);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::AUTONEGOTIATION_NOT_SUPPORTED);
}

TEST_F(InterfaceConfigurationValidatorTest, CheckManualSpeedDuplexConfiguration) {

  InterfaceConfigs itf_configs_manual;
  itf_configs_manual.emplace_back(InterfaceConfig{itf_x1_,InterfaceState::UP, Autonegotiation::OFF, 100, Duplex::FULL});
  itf_configs_manual.emplace_back(InterfaceConfig{itf_x2_,InterfaceState::UP, Autonegotiation::OFF, 10, Duplex::HALF});

  Status status = validator_.Validate(itf_configs_manual, iis_);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::OK);

  InterfaceConfigs itf_configs_unsupported_link_mode;
  itf_configs_unsupported_link_mode.emplace_back(InterfaceConfig{itf_x1_,InterfaceState::UP, Autonegotiation::OFF, 25, Duplex::FULL});
  status = validator_.Validate(itf_configs_unsupported_link_mode, iis_);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::LINK_MODE_NOT_SUPPORTED);
}

TEST_F(InterfaceConfigurationValidatorTest, RejectManualConfigurationOfSpeed1000) {
  InterfaceConfigs itf_configs_GBit;
  itf_configs_GBit.emplace_back(InterfaceConfig{itf_x1_,InterfaceState::UP, Autonegotiation::OFF, 1000, Duplex::FULL});
  
  Status status = validator_.Validate(itf_configs_GBit, iis_);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::LINK_MODE_NOT_CONFIGURABLE);

}

}  // namespace netconf
