// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommandExecutor.hpp"
#include "CommonTestDependencies.hpp"

#include <string>

#include "DeviceTypeLabel.hpp"
#include "HostnameController.hpp"
#include "DefaultHostname.hpp"

namespace netconf::api {


TEST(Hostname_Target, SetGetHostnameAndGetDefaultHostname){
  auto executor = CommandExecutor{};
  auto dtl = DeviceTypeLabel{executor};

  auto hostname = GetKernelHostname();
  auto default_hostname = GetDefaultHostname(dtl.GetMac());
  EXPECT_EQ(hostname, default_hostname);

  ::std::string expected_hostname = hostname + "TEST";
  SetKernelHostname(expected_hostname);
  auto actual_hostname = GetKernelHostname();
  EXPECT_EQ(actual_hostname,expected_hostname);

  SetKernelHostname(default_hostname);

}



}  // namespace netconf
