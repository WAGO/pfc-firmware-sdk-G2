// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommonTestDependencies.hpp"

#include "InterfaceInformation.hpp"
#include "JsonConverter.hpp"
#include <nlohmann/json.hpp>

//#include "JsonHelper.hpp"

namespace netconf {


TEST(JsonConverterInterfaceInformationTest, ConvertBackAndForth){
  InterfaceInformation ii{Interface::CreateBridge("mybridge"), true};

  JsonConverter jc;
  auto ii_json = jc.ToJsonString(ii);

  InterfaceInformation out_obj;
  auto status = jc.FromJsonString(ii_json, out_obj);

  EXPECT_EQ(StatusCode::OK, status.GetStatusCode()) << ii_json << "\n"<< status.ToString();

  EXPECT_EQ("mybridge", out_obj.GetInterface().GetName());
  EXPECT_EQ("mybridge", out_obj.GetInterface().GetLabel());
  EXPECT_EQ(DeviceType::Bridge, out_obj.GetType());
  EXPECT_EQ(true, out_obj.IsIpConfigurationReadonly());
}

TEST(JsonConverterInterfaceInformationTest, FailOnMissingKeys){
  JsonConverter jc;
  auto ii_json = R"({"name":"mydev","label":"mylabel"})";

  InterfaceInformation out_obj;
  auto status = jc.FromJsonString(ii_json, out_obj);

  EXPECT_EQ(StatusCode::JSON_KEY_MISSING, status.GetStatusCode()) << status.ToString();

}


TEST(JsonConverterInterfaceInformationTest, AllTypes){
  JsonConverter jc;
  auto ii_json_ethernet = R"({"name":"mydev","label":"mylabel","type":"ethernet","ipreadonly":true})";
  auto ii_json_bridge = R"({"name":"mydev","label":"mylabel","type":"bridge","ipreadonly":true})";
  auto ii_json_port = R"({"name":"mydev","label":"mylabel","type":"port","ipreadonly":true})";

  auto ii_json_dummy = R"({"name":"mydev","label":"mylabel","type":"dummy","ipreadonly":true})";
  auto ii_json_vlan = R"({"name":"mydev","label":"mylabel","type":"vlan","vlanid":100,"link":"br0","ipreadonly":true})";
  auto ii_json_loopback = R"({"name":"mydev","label":"mylabel","type":"loopback","ipreadonly":true})";

  auto ii_json_wwan = R"({"name":"mydev","label":"mylabel","type":"wwan","ipreadonly":true})";
  auto ii_json_other = R"({"name":"mydev","label":"mylabel","type":"wwan","ipreadonly":true})";

  InterfaceInformation out_obj;
  auto status = jc.FromJsonString(ii_json_ethernet, out_obj);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode()) << status.ToString();
  status = jc.FromJsonString(ii_json_bridge, out_obj);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode()) << status.ToString();
  status = jc.FromJsonString(ii_json_loopback, out_obj);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode()) << status.ToString();
  status = jc.FromJsonString(ii_json_dummy, out_obj);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode()) << status.ToString();
  status = jc.FromJsonString(ii_json_vlan, out_obj);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode()) << status.ToString();
  status = jc.FromJsonString(ii_json_port, out_obj);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode()) << status.ToString();
  status = jc.FromJsonString(ii_json_wwan, out_obj);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode()) << status.ToString();
  status = jc.FromJsonString(ii_json_other, out_obj);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode()) << status.ToString();

}

TEST(JsonConverterInterfaceInformationTest, MultipleInformationObjects){
  JsonConverter jc;
  auto ii_json_ethernet =R"([{"name":"dev1","label":"dev1","type":"ethernet","ipreadonly":false},{"name":"dev2","label":"dev2","type":"bridge","ipreadonly":true}])";

  InterfaceInformations iis;
  auto status = jc.FromJsonString(ii_json_ethernet, iis);
  EXPECT_EQ(StatusCode::OK, status.GetStatusCode()) << status.ToString();
  EXPECT_EQ(2, iis.size());

  auto j_str = jc.ToJsonString(iis);
  EXPECT_EQ(nlohmann::json::parse(j_str).dump(), nlohmann::json::parse(ii_json_ethernet).dump());

}


TEST(JsonConverterInterfaceInformationTest, ConvertInterfaceInformationBackAndForthWithLinkModes){

  InterfaceInformation itf_informationX1{Interface::CreatePort("ethX1"), false, AutonegotiationSupported::NO, {{10, Duplex::FULL}, {10, Duplex::HALF}}};
  InterfaceInformation itf_informationX2{Interface::CreatePort("ethX2"), false, AutonegotiationSupported::YES, {{100, Duplex::HALF}, {100, Duplex::FULL}}};
  InterfaceInformations expected_itf_informations{itf_informationX1, itf_informationX2};

  InterfaceInformations actual_itf_informations{};

  JsonConverter jc;
  ::std::string json_string = jc.ToJsonString(expected_itf_informations);
  jc.FromJsonString(json_string, actual_itf_informations);

  EXPECT_EQ(expected_itf_informations, actual_itf_informations);


}

}  // namespace netconf
