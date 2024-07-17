// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>
#include <BaseTypes.hpp>
#include <IPConfig.hpp>
#include <iostream>
#include "IPConfigHandler.hpp"
#include "IPConfigMock.hpp"
#include "gmock/gmock-more-actions.h"

namespace network_config {

using testing::Return;
using testing::_;
using testing::DoAll;

template<typename ... Args>
auto CreateProgramArgs(Args ... args) -> ::std::array<const char*, sizeof...(args)>
{
  return ::std::array<const char*, sizeof...(args)>{{args...}};
}

class IPConfigHandlerTest : public ::testing::Test {
 public:
  netconf::api::MockIPConfig ip_config_mock_;
  OptionParser opt_parser;

  void SetUp() override{

  }

  template<typename ... Args>
  IPConfigHandler CreateWithParams(Args ... args) {
    auto arg_array = CreateProgramArgs("program", args...);
    opt_parser.Parse(arg_array.size(), arg_array.data());
    return IPConfigHandler(opt_parser);
  }

};


TEST_F(IPConfigHandlerTest, SetWithBridgeDuplicateFails)
{
  auto sut = CreateWithParams("--ip-config","-fjson", "--set", R"( {"br0": {"bcast": "192.168.1.255","ipaddr": "192.168.1.17","netmask": "255.255.255.0","source": "static"},"br0": {"bcast": "192.168.255.255","ipaddr": "192.168.2.17","netmask": "255.255.0.0","source": "static"}})");
  EXPECT_CALL(ip_config_mock_, SetIPConfigs(_)).Times(0);
  EXPECT_ANY_THROW(sut.Execute());

}

TEST_F(IPConfigHandlerTest, SetWithTwoBridges)
{
  auto sut = CreateWithParams("--ip-config","-fjson", "--set", R"( {"br0": {"bcast": "192.168.1.255","ipaddr": "192.168.1.17","netmask": "255.255.255.0","source": "static"},"br1": {"bcast": "192.168.255.255","ipaddr": "192.168.2.17","netmask": "255.255.0.0","source": "static"}})");

  EXPECT_CALL(ip_config_mock_, SetIPConfigs(_)).WillOnce(Return(netconf::Status{}));
  EXPECT_NO_THROW(sut.Execute());
  

}

TEST_F(IPConfigHandlerTest, GetDHCPClientID)
{
  auto sut = CreateWithParams("--ip-config", "--get", "--device=br1");
  testing::internal::CaptureStdout();

  netconf::api::IPConfigs ip_configs;
  netconf::api::MakeIPConfigs(R"({"br1": {"bcast": "0.0.0.0","ipaddr": "0.0.0.0","netmask": "0.0.0.0","source": "dhcp","dhcp-client-id":"testclientid"}, "br0": {"bcast": "192.168.1.255","ipaddr": "192.168.1.17","netmask": "255.255.255.0","source": "static"}})",ip_configs);
  EXPECT_CALL(ip_config_mock_, GetIPConfigs(_)).WillOnce(DoAll(testing::SetArgReferee<0>(ip_configs),Return(netconf::Status{})));
  EXPECT_NO_THROW(sut.Execute());

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_THAT(output, testing::HasSubstr("bcast=0.0.0.0"));
  EXPECT_THAT(output, testing::HasSubstr("ipaddr=0.0.0.0"));
  EXPECT_THAT(output, testing::HasSubstr("netmask=0.0.0.0"));
  EXPECT_THAT(output, testing::HasSubstr("source=dhcp"));
  EXPECT_THAT(output, testing::HasSubstr("dhcp-client-id=testclientid"));
}



}
