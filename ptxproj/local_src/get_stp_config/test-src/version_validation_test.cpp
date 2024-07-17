// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "stp_config_output.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <ostream>
#include <stp.hpp>
#include <string>


namespace wago::stp::get_stp_config {

using namespace wago::stp::lib;  // NOLINT google-build-using-namespace
using namespace ::testing;       // NOLINT google-build-using-namespace


TEST(version_validation_test, version_is_valid) {
  ::std::string version{"04.04.10(26)"};
  ASSERT_TRUE(is_valid_fw_version(version));
}

struct validation_suite : public ::testing::Test, ::testing::WithParamInterface<::std::string> {};

TEST_P(validation_suite, version_is_invalid) {
  auto version = GetParam();
  ASSERT_FALSE(is_valid_fw_version(version));
}

INSTANTIATE_TEST_CASE_P(version, validation_suite, Values(
  "", "4.03.10", "02.4.10", "04.04.1", "04.04.10", "04.04.10(x)", "04.04.10(100)", "0a.04.10", "04.04", "Test", "04.04.100"),
  [](const testing::TestParamInfo<validation_suite::ParamType>& param_info) {
    std::string name = param_info.param;
    if(name.empty()) {
      name = "empty_string";
    } else {
      for(auto& c : name) {
        if(!std::isalnum(c)) {
          c = '_';
        }
      }
    }
    return name;
  });

}  // namespace wago::stp::get_stp_config