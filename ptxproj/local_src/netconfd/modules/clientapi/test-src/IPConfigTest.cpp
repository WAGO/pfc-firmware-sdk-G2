// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommonTestDependencies.hpp"

#include <string>

#include <nlohmann/json.hpp>
#include "IPConfig.hpp"
#include "JsonConverter.hpp"

namespace netconf {
namespace api {

using nlohmann::json;

class IPConfigTest : public testing::Test {};

TEST_F(IPConfigTest, AddIpConfig) {
  IPConfigs ip_configs{};

  IPConfig expected{"itf", IPSource::STATIC, "192.168.1.1", "255.255.255.0"};
  ip_configs.AddIPConfig(expected);

  auto actual = ip_configs.GetIPConfig("itf");

  EXPECT_EQ(expected, *actual);
}

TEST_F(IPConfigTest, ReplaceIpConfig) {
  IPConfigs ip_configs{};

  IPConfig existing{"itf", IPSource::STATIC, "192.168.1.1", "255.255.255.0"};
  ip_configs.AddIPConfig(existing);
  IPConfig expected{"itf", IPSource::DHCP, "192.168.2.1", "255.255.255.0"};
  ip_configs.AddIPConfig(expected);

  auto actual = ip_configs.GetIPConfig("itf");

  EXPECT_EQ(expected, *actual);
}

TEST_F(IPConfigTest, CreateAndGetWithSquareBracketOperator) {

  IPConfigs ip_configs{};
  ip_configs["br0"];

  EXPECT_EQ(1, ip_configs.GetConfig().size());
  IPConfig ipconfig = ip_configs.GetConfig()[0];
  EXPECT_EQ("br0", ipconfig.interface_);

  ipconfig = ip_configs["br0"];
  EXPECT_EQ(1, ip_configs.GetConfig().size());
  EXPECT_EQ("br0", ipconfig.interface_);
}

TEST_F(IPConfigTest, RemoveIpConfig) {
  IPConfigs ip_configs{};

  IPConfig existing{"itf", IPSource::STATIC, "192.168.1.1", "255.255.255.0"};
  ip_configs.AddIPConfig(existing);

  ip_configs.RemoveIPConfig("itf");

  IPConfig expected{};
  auto actual = ip_configs.GetIPConfig("itf");

  EXPECT_FALSE(actual);
}

TEST_F(IPConfigTest, CreateFromJsonString) {
  IPConfigs config;
  auto status = MakeIPConfigs(R"({ "itf" : { "source": "dhcp" }})", config);

  auto actual = config.GetIPConfig("itf");
  IPConfig expected{"itf", IPSource::DHCP, ZeroIP, ZeroNetmask};

  EXPECT_EQ(expected, actual);
}

TEST_F(IPConfigTest, IPConfigsToJson) {
  auto json_str = R"({ "itf" : {  "ipaddr": "", "netmask": "", "bcast" :"", "source": "dhcp"  }})";
  auto expected = json::parse(json_str);

  IPConfigs config;
  auto status = MakeIPConfigs(json_str, config);

  auto actual_str = ToJson(config);
  auto actual_pretty_str = ToPrettyJson(config);

  auto actual = json::parse(actual_str);
  EXPECT_EQ(expected.dump(), actual.dump());
  actual = json::parse(actual_pretty_str);
  EXPECT_EQ(expected.dump(), actual.dump());
}

TEST_F(IPConfigTest, IPConfigToJson) {

}

TEST(IPSource, ToString) {
  using namespace std::literals;
  EXPECT_EQ("dhcp"s, ToString(IPSource::DHCP));
  EXPECT_EQ("static", ToString(IPSource::STATIC));
  EXPECT_EQ("bootp", ToString(IPSource::BOOTP));
  EXPECT_EQ("none", ToString(IPSource::NONE));
}

TEST_F(IPConfigTest, CalculateBroadcast) {
  IPConfig ipcfg{"br123", IPSource::STATIC, "192.168.123.12", "255.255.255.0"};

  EXPECT_EQ("192.168.123.0", CalculateBroadcast(ipcfg));
  ipcfg.netmask_="255.255.0.0";
  EXPECT_EQ("192.168.0.0", CalculateBroadcast(ipcfg));

  ipcfg.netmask_="255.555.0.0";
  EXPECT_EQ("", CalculateBroadcast(ipcfg));

}

TEST_F(IPConfigTest, ValidateIpConfig) {
  auto json_str = R"({"br0":{"source":"static","ipaddr":"123.0.1.1","netmask":"255.255.255.0","bcast":"123.0.1.255"},"br1":{"source":"static","ipaddr":"123.1.1.1","netmask":"255.255.255.0","bcast":"123.1.1.255"},"br2":{"source":"static","ipaddr":"123.2.1.1","netmask":"255.255.255.0","bcast":"123.2.1.255"},"br3":{"source":"static","ipaddr":"123.3.1.1","netmask":"255.255.255.0","bcast":"123.3.1.255"}})";
  auto initial_switched = R"({"br0":{"source":"static","ipaddr":"123.1.1.1","netmask":"255.255.255.0","bcast":"123.1.1.255"},"br1":{"bcast":"0.0.0.0","ipaddr":"0.0.0.0","netmask":"0.0.0.0","source":"none"},"br2":{"bcast":"0.0.0.0","ipaddr":"0.0.0.0","netmask":"0.0.0.0","source":"none"},"br3":{"bcast":"0.0.0.0","ipaddr":"0.0.0.0","netmask":"0.0.0.0","source":"none"},"ethX1":{"bcast":"0.0.0.0","ipaddr":"0.0.0.0","netmask":"0.0.0.0","source":"none"},"ethX11":{"bcast":"0.0.0.0","ipaddr":"0.0.0.0","netmask":"0.0.0.0","source":"none"},"ethX12":{"bcast":"0.0.0.0","ipaddr":"0.0.0.0","netmask":"0.0.0.0","source":"none"},"ethX2":{"bcast":"0.0.0.0","ipaddr":"0.0.0.0","netmask":"0.0.0.0","source":"none"}})";
  IPConfigs config;
  IPConfigs current;
  InterfaceInformations iis {
    InterfaceInformation{Interface::CreateBridge("br0")},
    InterfaceInformation{Interface::CreateBridge("br1")},
    InterfaceInformation{Interface::CreateBridge("br2")},
    InterfaceInformation{Interface::CreateBridge("br3")}};
  auto status = MakeIPConfigs(json_str, config);
  ASSERT_TRUE(status.IsOk());
  status = MakeIPConfigs(initial_switched, current);
  ASSERT_TRUE(status.IsOk());

  Status result = ValidateIpConfigs(config, current, iis);
  EXPECT_EQ(StatusCode::OK, result.GetStatusCode());

}



}  // namespace api
}  // namespace netconf
