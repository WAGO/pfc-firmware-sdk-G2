// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommonTestDependencies.hpp"

#include <string>

#include <nlohmann/json.hpp>
#include "InterfaceConfig.hpp"
#include "JsonConverter.hpp"
#include "Status.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace netconf {
namespace api {

using nlohmann::json;

struct InterfaceConfigsTest : public ::testing::Test {

  InterfaceInformations CreateInterfaceInformations(){
    const std::string iis_json = R"([
{"ipreadonly":false,"label":"br0","name":"br0","type":"bridge"},
{"ipreadonly":false,"label":"br1","name":"br1","type":"bridge"},
{"ipreadonly":false,"label":"br2","name":"br2","type":"bridge"},
{"ipreadonly":false,"label":"br3","name":"br3","type":"bridge"},
{"ipreadonly":true,"label":"eth0","name":"eth0","type":"ethernet"},
{"autonegsupported":true,"ipreadonly":true,"label":"X1","name":"ethX1","supportedlinkmodes":[{"duplex":"half","speed":10},{"duplex":"full","speed":10},{"duplex":"half","speed":100},{"duplex":"full","speed":100}],"type":"port"},
{"autonegsupported":true,"ipreadonly":true,"label":"X2","name":"ethX2","supportedlinkmodes":[{"duplex":"half","speed":10},{"duplex":"full","speed":10},{"duplex":"half","speed":100},{"duplex":"full","speed":100}],"type":"port"},
{"autonegsupported":true,"ipreadonly":true,"label":"X11","name":"ethX11","supportedlinkmodes":[{"duplex":"half","speed":10},{"duplex":"full","speed":10},{"duplex":"half","speed":100},{"duplex":"full","speed":100}],"type":"port"},
{"autonegsupported":true,"ipreadonly":true,"label":"X12","name":"ethX12","supportedlinkmodes":[{"duplex":"half","speed":10},{"duplex":"full","speed":10},{"duplex":"half","speed":100},{"duplex":"full","speed":100}],"type":"port"}
])";
    JsonConverter jc;
    InterfaceInformations iis;
    auto conver_status = jc.FromJsonString(iis_json, iis);
    EXPECT_EQ(StatusCode::OK, conver_status.GetStatusCode());
    return iis;
  }

  InterfaceConfigs CreateConfigsFromJson(std::string& config_json){
    InterfaceConfigs configs;
    auto conver_status = MakeInterfaceConfigs(config_json, configs);
    EXPECT_EQ(conver_status.GetStatusCode(), StatusCode::OK);
    return configs;
  }

  void ExpectValidateResult(StatusCode expected_result, std::string config_json)
  {
    Status result = ValidateInterfaceConfigs(CreateConfigsFromJson(config_json), CreateInterfaceInformations());
    EXPECT_EQ(result.GetStatusCode(), expected_result);
  }
};

TEST_F(InterfaceConfigsTest, MacAddressToString)
{
  const uint8_t macbytes[] = {0x01,0x02,0x03,0x04,0x05,0x06};
  MacAddress mac(macbytes);

  EXPECT_EQ("01:02:03:04:05:06", mac.ToString());
  EXPECT_EQ("01-02-03-04-05-06", mac.ToString('-'));
}


TEST_F(InterfaceConfigsTest, InterfaceUpDownTest_Target){
  const ::std::string interface{"ethX2"};
  InterfaceState current_state;

  auto status = SetInterfaceState(interface,  InterfaceState::DOWN);
  EXPECT_TRUE(status.IsOk());
  status = GetInterfaceState(interface,  current_state);
  EXPECT_TRUE(status.IsOk());
  EXPECT_EQ(InterfaceState::DOWN, current_state);

  status = SetInterfaceState(interface,  InterfaceState::UP);
  EXPECT_TRUE(status.IsOk());
  status = GetInterfaceState(interface,  current_state);
  EXPECT_TRUE(status.IsOk());
  EXPECT_EQ(InterfaceState::UP, current_state);


}

TEST_F(InterfaceConfigsTest, ValidateConfigs){

  ExpectValidateResult(StatusCode::OK, R"([{"autonegotiation":"on","device":"X1","duplex":"full","speed":100,"state":"up"},{"autonegotiation":"on","device":"X2","duplex":"full","speed":100,"state":"up"},{"autonegotiation":"on","device":"X11","duplex":"full","speed":100,"state":"up"},{"autonegotiation":"on","device":"X12","duplex":"full","speed":100,"state":"up"}])");
  ExpectValidateResult(StatusCode::INTERFACE_NOT_EXISTING, R"([{"autonegotiation":"on","device":"X1123","duplex":"full","speed":100,"state":"up"},{"autonegotiation":"on","device":"X2","duplex":"full","speed":100,"state":"up"},{"autonegotiation":"on","device":"X11","duplex":"full","speed":100,"state":"up"},{"autonegotiation":"on","device":"X12","duplex":"full","speed":100,"state":"up"}])");
  ExpectValidateResult(StatusCode::OK, R"([{"autonegotiation":"on","device":"X1","duplex":"full","speed":10000,"state":"up"},{"autonegotiation":"on","device":"X2","duplex":"full","speed":100,"state":"up"},{"autonegotiation":"on","device":"X11","duplex":"full","speed":100,"state":"up"},{"autonegotiation":"on","device":"X12","duplex":"full","speed":100,"state":"up"}])");
  ExpectValidateResult(StatusCode::LINK_MODE_NOT_SUPPORTED, R"([{"autonegotiation":"off","device":"X1","duplex":"full","speed":10000,"state":"up"},{"autonegotiation":"on","device":"X2","duplex":"full","speed":100,"state":"up"},{"autonegotiation":"on","device":"X11","duplex":"full","speed":100,"state":"up"},{"autonegotiation":"on","device":"X12","duplex":"full","speed":100,"state":"up"}])");
}

TEST_F(InterfaceConfigsTest, InterfaceConfigToStringInCaseOfFormatText){

  InterfaceConfig ic{Interface::CreatePort("ethX1"), InterfaceState::UP, Autonegotiation::ON, 100, Duplex::HALF, MacLearning::ON};

  auto text = ToString(ic);
  ::std::string expected_text = "device=X1 state=up autonegotiation=on duplex=half speed=100 maclearning=on";

  EXPECT_EQ(expected_text, text);

}

}  // namespace api
}  // namespace netconf
