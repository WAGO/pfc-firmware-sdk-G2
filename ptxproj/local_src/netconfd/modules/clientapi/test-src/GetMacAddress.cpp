// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommonTestDependencies.hpp"

#include <string>

#include "InterfaceConfig.hpp"

namespace netconf {
namespace api {


TEST(GetMacAddress_Target, GetMac){

  auto mac = GetMacAddress("br0");

  EXPECT_THAT(mac, ::testing::StartsWith("00:30:DE:"));

}

}  // namespace api
}  // namespace netconf
