// SPDX-License-Identifier: GPL-2.0-or-later

#include <gmock/gmock-spec-builders.h>
#include <gtest/gtest.h>

#include "BaseTypes.hpp"
#include "CommonTestDependencies.hpp"
#include "Redundancy.hpp"
#include "STPMock.hpp"
#include "stp.hpp"

using testing::_;
using testing::Return;
using testing::SetArgReferee;
using testing::DoAll;

namespace netconf {

TEST(RedundancyTest, DisableSTPConfigByRemovingBridge) {
  wago::stp::lib::MockSTP mock_stp;
  BridgeConfig bc{{Interface::CreateBridge("br1"), {Interface::CreatePort("ethX11")}}};
  Redundancy red{};

  ::wago::stp::lib::stp_config config{};
  config.enabled = true;
  config.bridge  = "br0";
  config.port_configs.emplace_back(wago::stp::lib::stp_port_config{"ethX11"});

  EXPECT_CALL(mock_stp, get_persisted(_)).WillOnce(DoAll(SetArgReferee<0>(config), Return(wago::stp::lib::status{})));

  ::wago::stp::lib::stp_config expected_config = config;
  expected_config.enabled                      = false;

  EXPECT_CALL(mock_stp, configure(expected_config)).WillOnce(Return(wago::stp::lib::status{}));

  red.OnBridgeRemove(Interface::CreateBridge("br0"));
}

TEST(RedundancyTest, AdjustingSTPPortConfigByRemovingAPort) {
  wago::stp::lib::MockSTP mock_stp;
  Redundancy red{};

  ::wago::stp::lib::stp_config config{};
  config.enabled = true;
  config.bridge  = "br0";
  config.port_configs.emplace_back(wago::stp::lib::stp_port_config{"ethX1"});
  config.port_configs.emplace_back(wago::stp::lib::stp_port_config{"ethX11"});
  config.port_configs.emplace_back(wago::stp::lib::stp_port_config{"ethX12"});
  config.port_configs.emplace_back(wago::stp::lib::stp_port_config{"ethX2"});

  EXPECT_CALL(mock_stp, get_persisted(_)).WillOnce(DoAll(SetArgReferee<0>(config), Return(wago::stp::lib::status{})));

  ::wago::stp::lib::stp_config expected_config = config;
  expected_config.port_configs.clear();
  expected_config.port_configs.emplace_back(wago::stp::lib::stp_port_config{"ethX1"});
  expected_config.port_configs.emplace_back(wago::stp::lib::stp_port_config{"ethX2"});

  EXPECT_CALL(mock_stp, configure(expected_config)).WillOnce(Return(wago::stp::lib::status{}));

  BridgeConfig bc{{Interface::CreateBridge("br0"), {Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2")}}};
  auto bridge = Interface::CreateBridge("br0");
  auto itfs = {Interface::CreatePort("ethX1"), Interface::CreatePort("ethX2")};
  red.OnBridgeAddOrPortChange(bridge, itfs);
}

}  // namespace netconf

//---- End of source file ------------------------------------------------------
