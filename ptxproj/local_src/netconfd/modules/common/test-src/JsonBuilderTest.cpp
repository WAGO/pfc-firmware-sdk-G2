// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommonTestDependencies.hpp"

#include "JsonBuilder.hpp"
#include <nlohmann/json.hpp>

namespace netconf {

TEST(JsonBuilderTest, BuildInterfaceConfig) {

  JsonBuilder jb;

  nlohmann::json expected_json =
      nlohmann::json::parse(
          R"({"interface-config":[
      {"autonegotiation":"on", "device":"br0","duplex":"full", "speed":100, "state":"up", "maclearning":"on"},
      {"autonegotiation":"off", "device":"br1","duplex":"half", "speed":10, "state":"up", "maclearning":"off"}
]})");

  InterfaceConfigs itfcfg { InterfaceConfig{ Interface::CreateBridge("br0"), InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL, MacLearning::ON }, InterfaceConfig{ Interface::CreateBridge("br1"),
      InterfaceState::UP, Autonegotiation::OFF, 10, Duplex::HALF, MacLearning::OFF } };

  jb.Append("interface-config", itfcfg);

  EXPECT_EQ(expected_json.dump(), jb.ToString());

}

TEST(JsonBuilderTest, BuildInterfaceConfig_lt_fw22) {

  JsonBuilder jb;

  nlohmann::json expected_json =
      nlohmann::json::parse(
          R"({"interface-config":[
      {"autonegotiation":"on", "device":"br0","duplex":"full", "speed":100, "state":"up"},
      {"autonegotiation":"off", "device":"br1","duplex":"half", "speed":10, "state":"up"}
]})");

  InterfaceConfigs itfcfg { InterfaceConfig{ Interface::CreateBridge("br0"), InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL, MacLearning::ON }, InterfaceConfig{ Interface::CreateBridge("br1"),
      InterfaceState::UP, Autonegotiation::OFF, 10, Duplex::HALF, MacLearning::OFF } };

  jb.Append("interface-config", itfcfg, FirmwareVersion { 3, 9, 4, 21});

  EXPECT_EQ(expected_json.dump(), jb.ToString());

}

TEST(JsonBuilderTest, BuildInterfaceConfigFw15) {

  JsonBuilder jb;

  nlohmann::json expected_json =
      nlohmann::json::parse(
          R"({"interface-config":[
      {"autonegotiation":true, "device":"br0","duplex":"full", "speed":100, "state":"up"},
      {"autonegotiation":false, "device":"br1","duplex":"half", "speed":10, "state":"up"}
]})");

  InterfaceConfigs itfcfg { InterfaceConfig{ Interface::CreateBridge("br0"), InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL, MacLearning::OFF }, InterfaceConfig{ Interface::CreateBridge("br1"),
      InterfaceState::UP, Autonegotiation::OFF, 10, Duplex::HALF, MacLearning::ON } };

  jb.Append("interface-config", itfcfg, FirmwareVersion { 3, 3, 10, 15 });

  EXPECT_EQ(expected_json.dump(), jb.ToString());
}

TEST(JsonBuilderTest, BuildBridgeConfig) {

  nlohmann::json expected_json_fw15 = nlohmann::json::parse(R"({"bridge-config":{"br0":["X1","X2"],"br1":[]}})");
  nlohmann::json expected_json = nlohmann::json::parse(R"({"bridge-config":{"br0":["X1","X2"]}})");

  nlohmann::json expected_json_br1 = nlohmann::json::parse(R"({"bridge-config":{"br0":["X1","X2"], "br1":["X11","X12"]}})");

  BridgeConfig config { { Interface::CreateBridge("br0"), { Interface::CreatePort("X1"), Interface::CreatePort("X2") } } };
  BridgeConfig config_br1 { { Interface::CreateBridge("br0"), { Interface::CreatePort("X1"), Interface::CreatePort("X2") } }, {Interface::CreateBridge("br1"),{Interface::CreatePort("X11"), Interface::CreatePort("X12")}} };

  {
    JsonBuilder jb;
    jb.Append("bridge-config", config, FirmwareVersion { 3, 3, 10, 15 });
    EXPECT_EQ(expected_json_fw15.dump(), jb.ToString());
  }
  {
    JsonBuilder jb;
    jb.Append("bridge-config", config);
    EXPECT_EQ(expected_json.dump(), jb.ToString());
  }
  {
    JsonBuilder jb;
    jb.Append("bridge-config", config_br1, FirmwareVersion { 3, 3, 10, 15 });
    EXPECT_EQ(expected_json_br1.dump(), jb.ToString());
  }
  {
    JsonBuilder jb;
    jb.Append("bridge-config", config_br1);
    EXPECT_EQ(expected_json_br1.dump(), jb.ToString());
  }

}

}  // namespace netconf
