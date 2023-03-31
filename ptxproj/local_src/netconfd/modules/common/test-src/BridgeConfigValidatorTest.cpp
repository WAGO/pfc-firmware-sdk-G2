// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     InterfaceValidatorTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#include "CommonTestDependencies.hpp"
#include "BridgeConfigValidator.hpp"
#include <memory>


namespace netconf {

struct AnInterfaceValidator : public testing::Test {
};

TEST_F(AnInterfaceValidator, shouldRejectAnEmptyConfig) {

  BridgeConfig os_config { };
  Interfaces system_interfaces = { Interface::CreatePort("ethX1") };

  Status status = BridgeConfigValidator::Validate(os_config, system_interfaces);

  EXPECT_EQ(StatusCode::JSON_INCOMPLETE, status.GetStatusCode());
}


TEST_F(AnInterfaceValidator, checksAValidConfigurationContainingTwoBridges) {

  BridgeConfig os_config { { Interface::CreateBridge("br0"), { Interface::CreatePort("ethX1") } },
      { Interface::CreateBridge("br1"), { Interface::CreatePort("ethX2"), Interface::CreatePort("ethX3") } } };

  Interfaces system_interfaces = { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2"), Interface::CreatePort("ethX3") };

  Status status = BridgeConfigValidator::Validate(os_config, system_interfaces);

  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());
}

TEST_F(AnInterfaceValidator, checksAnInvalidConfigurationMissingInterface) {

  BridgeConfig os_config { { Interface::CreateBridge("br0"), { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2"), Interface::CreatePort("ethX11") } },
    { Interface::CreateBridge("br1"), { Interface::CreatePort("") } } };

  Interfaces system_interfaces = { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2") };

  Status status = BridgeConfigValidator::Validate(os_config, system_interfaces);

  EXPECT_EQ(StatusCode::INTERFACE_NOT_EXISTING, status.GetStatusCode());
}

TEST_F(AnInterfaceValidator, checksAnInvalidConfigurationContainingMultipleBridgesMissingInterface) {

  BridgeConfig os_config { { Interface::CreateBridge("br0"), { Interface::CreatePort("ethX1") } }, { Interface::CreateBridge("br1"),
      { Interface::CreatePort("ethX12"), Interface::CreatePort("ethX2") } }, { Interface::CreateBridge("br2"), { Interface::CreatePort("ethX3") } } };

  Interfaces system_interfaces = { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2"), Interface::CreatePort("ethX3") };

  Status status = BridgeConfigValidator::Validate(os_config, system_interfaces);

  EXPECT_EQ(StatusCode::INTERFACE_NOT_EXISTING, status.GetStatusCode());
}

TEST_F(AnInterfaceValidator, checksAnInvalidConfigurationInterfacesAssignedServeralTimes) {

  BridgeConfig os_config { { Interface::CreateBridge("br0"), { Interface::CreatePort("ethX1") } },
      { Interface::CreateBridge("br1"), { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2") } } };

  Interfaces system_interfaces = { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2") };

  Status status = BridgeConfigValidator::Validate(os_config, system_interfaces);

  EXPECT_EQ(StatusCode::ENTRY_DUPLICATE, status.GetStatusCode());
}

TEST_F(AnInterfaceValidator, checksAnInvalidConfigurationInterfacesAssignedServeralTimesToSameBridge) {

  BridgeConfig os_config { { Interface::CreateBridge("br0"), { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX1") } },
      { Interface::CreateBridge("br1"), { Interface::CreatePort("ethX2") } } };

  Interfaces system_interfaces = { Interface::CreatePort("ethX1"), Interface::CreatePort("eth2") };

  Status status = BridgeConfigValidator::Validate(os_config, system_interfaces);

  EXPECT_EQ(StatusCode::ENTRY_DUPLICATE, status.GetStatusCode());
}

TEST_F(AnInterfaceValidator, checksAnInvalidBridgeNameContainingBlanks) {

  BridgeConfig os_config { { Interface::CreateBridge("br1 0"), { Interface::CreatePort("ethX3") } }, { Interface::CreateBridge("br0"), { Interface::CreatePort("ethX1") } }, {
      Interface::CreateBridge("br1"), { Interface::CreatePort("ethX2") } } };

  Interfaces system_interfaces = { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2"), Interface::CreatePort("ethX3") };

  Status status = BridgeConfigValidator::Validate(os_config, system_interfaces);

  EXPECT_EQ(StatusCode::BRIDGE_NAME_INVALID, status.GetStatusCode());
}

TEST_F(AnInterfaceValidator, checksMaximumBridgeNameLengthOf15Characters) {

  BridgeConfig os_config { { Interface::CreateBridge("br0123456789123"), { Interface::CreatePort("ethX3") } }, { Interface::CreateBridge("br0"), {
      Interface::CreatePort("ethX1") } }, { Interface::CreateBridge("br1"), { Interface::CreatePort("ethX2") } } };

  Interfaces system_interfaces = { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2"), Interface::CreatePort("ethX3") };

  Status status = BridgeConfigValidator::Validate(os_config, system_interfaces);

  EXPECT_EQ(StatusCode::OK, status.GetStatusCode());
}

TEST_F(AnInterfaceValidator, checksMaximumBridgeNameLengthGreaterThan15Characters) {

  BridgeConfig os_config { { Interface::CreateBridge("br01234567891234"), { Interface::CreatePort("ethX3") } }, { Interface::CreateBridge("br0"), { Interface::CreatePort("ethX1") } }, {
      Interface::CreateBridge("br1"), { Interface::CreatePort("ethX2") } } };

  Interfaces system_interfaces = { Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2"), Interface::CreatePort("ethX3") };

  Status status = BridgeConfigValidator::Validate(os_config, system_interfaces);

  EXPECT_EQ(StatusCode::BRIDGE_NAME_INVALID, status.GetStatusCode());
}

} /* namespace netconf */
