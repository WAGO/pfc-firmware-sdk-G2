// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommonTestDependencies.hpp"

#include <memory>

#include "JsonConverter.hpp"
#include "Status.hpp"
#include "TypesHelper.hpp"


namespace netconf {

class AJsonConfigConverter : public testing::Test {
 public:
  ::std::shared_ptr<JsonConverter> parser_;

  AJsonConfigConverter() = default;

  void SetUp() override {
    parser_ = ::std::make_shared<JsonConverter>();
  }

  void TearDown() override {
  }

  const ::std::string json_valid_config_ =
      R"(
  {
    "br0": [ "X1", "X2" ],
    "br1": [ "X11" ],
    "br2": [ "X12" ]
  }
  )";

  const ::std::string json_invalid_config_ =
      R"(
  {
    "br0": [ "X1", "X2" ],
    "br1": [  ,
    "br2": [ "X12" ]
  }
  )";

  BridgeConfig bridge_config_emptybridge = {
      {Interface::CreateBridge("br0"), {Interface::CreatePort("X1"), Interface::CreatePort("X2")}},
      {Interface::CreateBridge("br1"), {}},
      {Interface::CreateBridge("br2"), {}},
      {Interface::CreateBridge("br3"), {Interface::CreatePort("X12")}}};

  const ::std::string json_emptybridge_config_ =
      R"(
  {
    "br0": [ "X1", "X2" ],
    "br1": [  ],
    "br2": [  ],
    "br3": [ "X12" ]
  }
  )";

  const ::std::string json_emptybridgename_config_ =
      R"(
  {
    "br0": [ "X1", "X2" ],
    "": [ "X11" ],
    "br2": [ "X12" ]
  }
  )";

  const ::std::string json_invalid_config_duplicat_bridge_name_ =
      R"(
   {
     "br0": [ "X1" ],
     "br0": [ "X2" ]
   }
   )";

  IPConfigs one_ip_config_{{"br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0"}};
  const ::std::string json_one_ip_config_ =
      R"(
  {
    "br0": {
      "bcast": "192.168.1.255",
      "ipaddr": "192.168.1.17",
      "netmask": "255.255.255.0",
      "source": "static"
    }
  }
   )";

  IPConfigs two_ip_config_{{"br0", IPSource::STATIC, "192.168.1.17", "255.255.255.0"},
                           {"br1", IPSource::STATIC, "192.168.2.17", "255.255.0.0"}};

  const ::std::string json_two_ip_config_ =
      R"(
  {
    "br0": {
      "bcast": "192.168.1.255",
      "ipaddr": "192.168.1.17",
      "netmask": "255.255.255.0",
      "source": "static"
    },
    "br1": {
      "bcast": "192.168.255.255",
      "ipaddr": "192.168.2.17",
      "netmask": "255.255.0.0",
      "source": "static"
    }
  }
   )";

  IPConfigs one_ip_config_empty_elements_{{"", IPSource::NONE, "", "255.255.255.0"}};

  const ::std::string json_one_ip_config_empty_elements_ =
      R"(
  {
    "": {
      "bcast": "0.0.0.0",
      "ipaddr": "",
      "netmask": "255.255.255.0",
      "source": "none"
    }
  }
   )";
};

static void ExpectStringEqIgnoreNewlineAndBlank(::std::string expected, ::std::string value) {
  expected.erase(std::remove(expected.begin(), expected.end(), '\n'), expected.end());
  expected.erase(std::remove(expected.begin(), expected.end(), ' '), expected.end());

  value.erase(std::remove(value.begin(), value.end(), '\n'), value.end());
  value.erase(std::remove(value.begin(), value.end(), ' '), value.end());

  EXPECT_EQ(expected, value);
}

// Convert bridge configuration tests

TEST_F(AJsonConfigConverter, ConvertsAJsonConfiguration) {
  BridgeConfig bridge_config;
  Status status = parser_->FromJsonString(json_valid_config_, bridge_config);

  EXPECT_EQ(bridge_config[Interface::CreateBridge("br0")][0], Interface::CreatePort("X1"));
  EXPECT_EQ(bridge_config[Interface::CreateBridge("br0")][1], Interface::CreatePort("X2"));
  EXPECT_EQ(bridge_config[Interface::CreateBridge("br1")][0], Interface::CreatePort("X11"));
  EXPECT_EQ(bridge_config[Interface::CreateBridge("br2")][0], Interface::CreatePort("X12"));
  ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
}

TEST_F(AJsonConfigConverter, ConvertsAnInvalidJsonConfiguration) {
  BridgeConfig bridge_config;
  Status status = parser_->FromJsonString(json_invalid_config_, bridge_config);

  EXPECT_EQ(0, bridge_config.size());
  EXPECT_EQ(StatusCode::JSON_CONVERT, status.GetStatusCode());
}

TEST_F(AJsonConfigConverter, ConvertsAnEmptyJsonBridgeConfiguration) {
  BridgeConfig bridge_config;
  Status status = parser_->FromJsonString(json_emptybridge_config_, bridge_config);

  EXPECT_EQ(bridge_config[Interface::CreateBridge("br0")][0], Interface::CreatePort("X1"));
  EXPECT_EQ(bridge_config[Interface::CreateBridge("br0")][1], Interface::CreatePort("X2"));
  EXPECT_EQ(bridge_config[Interface::CreateBridge("br1")].size(), 0);
  EXPECT_EQ(bridge_config[Interface::CreateBridge("br2")].size(), 0);
  EXPECT_EQ(bridge_config[Interface::CreateBridge("br3")][0], Interface::CreatePort("X12"));
  ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
}

TEST_F(AJsonConfigConverter, ConvertsAnEmptyBridgeNameJsonConfiguration) {
  BridgeConfig bridge_config;
  Status status = parser_->FromJsonString(json_emptybridgename_config_, bridge_config);

  EXPECT_EQ(0, bridge_config.size());
  EXPECT_EQ(StatusCode::NAME_EMPTY, status.GetStatusCode()) << status.ToString();
}

TEST_F(AJsonConfigConverter, ConvertsAnInvalidJsonConfigurationWithTwoIdenticalNamedBridges) {
  BridgeConfig bridge_config;
  Status status = parser_->FromJsonString(json_invalid_config_duplicat_bridge_name_, bridge_config);

  EXPECT_EQ(0, bridge_config.size());
  EXPECT_EQ(StatusCode::JSON_CONVERT, status.GetStatusCode());
}

// Converts json configuration tests

TEST_F(AJsonConfigConverter, ConvertsABridgeConfiguration) {
  BridgeConfig bridge_config = {
      {Interface::CreateBridge("br0"),
      {Interface::CreatePort("X1"), Interface::CreatePort("X2")}},
      {Interface::CreateBridge("br1"), {Interface::CreatePort("X11")}},
      {Interface::CreateBridge("br2"), {Interface::CreatePort("X12")}}};

  ::std::string json = parser_->ToJsonString(bridge_config);

  ExpectStringEqIgnoreNewlineAndBlank(json_valid_config_, json);
}

TEST_F(AJsonConfigConverter, ConvertsABridgeConfigurationWithoutInterface) {
  BridgeConfig bridge_config = {
      {Interface::CreateBridge("br0"), {Interface::CreatePort("X1"), Interface::CreatePort("X2")}},
      {Interface::CreateBridge("br1"), {}},
      {Interface::CreateBridge("br2"), {}},
      {Interface::CreateBridge("br3"), {Interface::CreatePort("X12")}}};

  ::std::string json = parser_->ToJsonString(bridge_config);

  ExpectStringEqIgnoreNewlineAndBlank(json_emptybridge_config_, json);
}

TEST_F(AJsonConfigConverter, ConvertsBackAndForth) {
  BridgeConfig bridge_config1;
  Status status = parser_->FromJsonString(json_valid_config_, bridge_config1);

  ASSERT_EQ(StatusCode::OK, status.GetStatusCode());

  ::std::string json = parser_->ToJsonString(bridge_config1);

  BridgeConfig bridge_config2;
  status = parser_->FromJsonString(json, bridge_config2);

  EXPECT_TRUE(IsEqual(bridge_config1, bridge_config2));
  ASSERT_EQ(StatusCode::OK, status.GetStatusCode());
}

}  // namespace netconf
