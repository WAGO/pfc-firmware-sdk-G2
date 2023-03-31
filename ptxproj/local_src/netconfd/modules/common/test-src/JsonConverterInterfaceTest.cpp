// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommonTestDependencies.hpp"

#include <algorithm>
#include <cctype>
#include <memory>

#include "JsonConverter.hpp"
#include "Status.hpp"
#include "TypesHelper.hpp"

namespace netconf {

class JsonConverterInterfaceTest : public testing::Test {
 public:
  ::std::shared_ptr<JsonConverter> parser_;

  JsonConverterInterfaceTest() = default;

  void SetUp() override {
    parser_ = ::std::make_shared<JsonConverter>();
  }

  void TearDown() override {
  }

  Interfaces all_types { { Interface::CreateBridge("br0") }, { Interface::CreateDummy("dummy0") }, {
      Interface::CreatePort("ethX1") }, { Interface::CreateVLAN("br0.100", 100, "br0") }, };
  ::std::string const json_all_types_ =
    R"(
    [
      {
        "name": "br0",
        "type": "bridge"
      },
      {
        "name": "dummy0",
        "type": "dummy"
      },
      {
        "name": "X1",
        "type": "port"
      },
      {
        "link": "br0",
        "name": "br0.100",
        "type": "vlan",
        "vlanid": 100
      }
    ]
    )";

  Interfaces no_interface_ { };
  ::std::string const json_no_interfaces_ =
    R"(
    []
    )";

  ::std::string const json_unsupported_device_type_interface_ =
    R"(
      [{
        "name": "other0",
        "type": "wwan"}]
    )";

  ::std::string const json_unknown_device_type_interface_ =
    R"(
    [{
      "name": "test0",
      "type": "unknown"}
      ]
    )";

  ::std::string const json_unspecified_device_type_interface_ =
    R"(
    [{
      "name": "test0",
      "type": "unspecified"}]
    )";

  ::std::string const json_empty_device_type_interface_ =
    R"(
      [{
        "name": "test0",
        "type": ""}]
    )";

  Interfaces empty_interface_ { {Interface::CreateOther("test0")},{Interface::CreateOther("test1")} };
  ::std::string const json_empty_interface_ =
    R"(
      [{
        "name": "test0"
      },
      {
        "name": "test1"
      }]
    )";

  Interfaces without_device_type_interface_ { {Interface::CreateOther("dummy0")},{Interface::CreateOther("dummy1")},{Interface::CreateOther("dummy2")}, };
  ::std::string const json_without_device_type_interface_ =
    R"(
      [{
        "name": "dummy0"
      },
      {
        "name": "dummy1"
      },
      {
        "name": "dummy2"
      }]
    )";

  ::std::string const json_without_interface_name_ =
    R"(
      [{
        "type": "dummy"}]
    )";

  ::std::string const json_interface_without_array =
    R"(
      {
        "name": "dummy0",
        "type": "dummy"
      }
    )";
};

static std::string& RemoveWhitespaces(std::string &str) {
  auto f = [](unsigned char const c) {
    return std::isspace(c);
  };
  str.erase(std::remove_if(str.begin(), str.end(), f), str.end());
  return str;
}

static void ExpectStringEqIgnoreNewlineAndBlank(::std::string expected, ::std::string value) {
  RemoveWhitespaces(expected);
  RemoveWhitespaces(value);

  EXPECT_EQ(expected, value);
}

TEST_F(JsonConverterInterfaceTest, EmptyInterfaceListToJsonString) {
  auto json = parser_->ToJsonString(no_interface_);
  ExpectStringEqIgnoreNewlineAndBlank(json_no_interfaces_, json);
}

TEST_F(JsonConverterInterfaceTest, AllInterfaceTypesToJsonString) {
  auto json = parser_->ToJsonString(all_types, JsonFormat::PRETTY);
  ExpectStringEqIgnoreNewlineAndBlank(json_all_types_, json);
}

TEST_F(JsonConverterInterfaceTest, EmptyInterfaceListToInterfaces) {
  Interfaces interfaces;
  Status status = parser_->FromJsonString(json_no_interfaces_, interfaces);
  EXPECT_TRUE(status.IsOk());
  EXPECT_EQ(no_interface_, interfaces);
}

TEST_F(JsonConverterInterfaceTest, AllInterfaceTypesToInterfaces) {
  Interfaces interfaces;
  auto status = parser_->FromJsonString(json_all_types_ , interfaces);
  EXPECT_TRUE(status.IsOk());
  if (not status.IsOk()) {
    printf("%s\n\n", status.ToString().c_str());
  }
  EXPECT_THAT(interfaces, testing::UnorderedElementsAreArray(all_types));
}

TEST_F(JsonConverterInterfaceTest, UnknownDeviceTypeInterfaceToJsonString) {
  Interfaces interfaces;
  Status status = parser_->FromJsonString(json_unknown_device_type_interface_, interfaces);
  EXPECT_FALSE(status.IsOk());
  EXPECT_TRUE(interfaces.empty());
  EXPECT_EQ(StatusCode::DEVICE_TYPE_NOT_SOPPORTED, status.GetStatusCode());
  printf("%s\n\n", status.ToString().c_str());
}

TEST_F(JsonConverterInterfaceTest, EmptyDeviceTypeInterfaceToJsonString) {
  Interfaces interfaces;
  Status status = parser_->FromJsonString(json_without_device_type_interface_, interfaces);
  EXPECT_TRUE(status.IsOk());
  EXPECT_EQ(interfaces.at(1).GetType(), DeviceType::Other);
}

TEST_F(JsonConverterInterfaceTest, WithoutDeviceTypeInterfaceToJsonString) {
  Interfaces interfaces;
  Status status = parser_->FromJsonString(json_without_device_type_interface_, interfaces);
  EXPECT_TRUE(status.IsOk());
  EXPECT_EQ(without_device_type_interface_, interfaces);
}

TEST_F(JsonConverterInterfaceTest, WithEmptyInterfacePropertiesToJsonString) {
  Interfaces interfaces;
  Status status = parser_->FromJsonString(json_empty_interface_, interfaces);
  EXPECT_TRUE(status.IsOk());
  EXPECT_EQ(empty_interface_, interfaces);
}

TEST_F(JsonConverterInterfaceTest, WithoutInterfaceNameToJsonString) {
  Interfaces interfaces;
  Status status = parser_->FromJsonString(json_without_interface_name_, interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::JSON_KEY_MISSING);
  EXPECT_THAT(interfaces, testing::SizeIs(0));
}

TEST_F(JsonConverterInterfaceTest, WithInterfaceArrayToJsonString) {
  Interfaces interfaces;
  Status status = parser_->FromJsonString(json_interface_without_array, interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::JSON_CONVERT);
  EXPECT_THAT(interfaces, testing::SizeIs(0));
}

}  // namespace netconf
