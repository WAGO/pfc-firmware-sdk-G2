// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     DeviceTypeLabelQuerierTest.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#include "CommonTestDependencies.hpp"

#include "MockCommandExecutor.hpp"
#include "DeviceTypeLabel.hpp"


using testing::_;
using testing::SetArgReferee;
using testing::Return;


namespace netconf {

class ADeviceTypeLabelProvider_Host : public testing::Test {
 public:
  ADeviceTypeLabelProvider_Host() = default;

  void SetUp() override {
  }

  void TearDown() override {
  }

  MockCommandExecutor mock_executor_;

  ::std::string type_label_ini_ =
      R"(
    MAC=00:30:DE:11:11:11
    MAC_ID_INC=5
    ORDER=750-8215
  )";
  ::std::string type_label_ini_mac_overflow_first_byte_ =
      R"(
    MAC=00:30:DE:11:11:FF
    MAC_ID_INC=5
    ORDER=750-8215
  )";
  ::std::string type_label_ini_mac_overflow_second_byte_ =
      R"(
    MAC=00:30:DE:11:FF:FF
    MAC_ID_INC=5
    ORDER=750-8215
  )";

  ::std::string type_label_ini_missing_mac_id_inc =
      R"(
    MAC=00:30:DE:11:11:11
    ORDER=750-8215
  )";

  ::std::string type_label_ini_missing_value_ = R"(
    DISPLAYRES_V=0
  )";

  ::std::string type_label_ini_corrupt_ =
      R"(
    DISPLAYRES_V
    DISPLAYRES_H=0
  )";
};

TEST_F(ADeviceTypeLabelProvider_Host, GetsAnOrderNumber) {
  EXPECT_CALL(mock_executor_, Execute(_, _))
      .WillOnce(DoAll(SetArgReferee<1>(type_label_ini_), Return(Status(StatusCode::OK))));

  auto type_label_provider = ::std::make_unique<DeviceTypeLabel>(mock_executor_);

  ::std::string value = type_label_provider->GetOrderNumber();
  EXPECT_EQ("750-8215", value);
}

TEST_F(ADeviceTypeLabelProvider_Host, GetsAMacAddress) {
  EXPECT_CALL(mock_executor_, Execute(_, _))
      .WillOnce(DoAll(SetArgReferee<1>(type_label_ini_), Return(Status(StatusCode::OK))));

  auto type_label_provider = ::std::make_unique<DeviceTypeLabel>(mock_executor_);

  MacAddress value = type_label_provider->GetMac();
  EXPECT_EQ("00:30:DE:11:11:11", value.ToString());
}

TEST_F(ADeviceTypeLabelProvider_Host, IsNotAbleToExecuteTypelabelToolSuccessfully) {
  EXPECT_CALL(mock_executor_, Execute(_, _)).WillOnce(Return(Status(StatusCode::SYSTEM_EXECUTE)));

  EXPECT_THROW(::std::make_unique<DeviceTypeLabel>(mock_executor_), ::std::runtime_error);
}

TEST_F(ADeviceTypeLabelProvider_Host, FailedToReadMissingTypelabelValue) {
  EXPECT_CALL(mock_executor_, Execute(_, _))
      .WillOnce(DoAll(SetArgReferee<1>(type_label_ini_missing_value_), Return(Status(StatusCode::OK))));

  EXPECT_THROW(::std::make_unique<DeviceTypeLabel>(mock_executor_), ::std::runtime_error);
}

TEST_F(ADeviceTypeLabelProvider_Host, FailedToReadCorruptTypelabelIniFile) {
  EXPECT_CALL(mock_executor_, Execute(_, _))
      .WillOnce(DoAll(SetArgReferee<1>(type_label_ini_corrupt_), Return(Status(StatusCode::OK))));

  EXPECT_THROW(::std::make_unique<DeviceTypeLabel>(mock_executor_), ::std::runtime_error);
}

} /* namespace netconf */
