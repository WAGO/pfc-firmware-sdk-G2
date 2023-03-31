// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommonTestDependencies.hpp"

#include <string>

#include "BaseTypes.hpp"

#include "Status.hpp"


namespace netconf{

TEST(BaseTypes, ConvertInterfaceToLabel) {

  EXPECT_EQ("eth0", Interface::CreateEthernet("eth0").GetLabel());

  EXPECT_EQ("X1", Interface::CreatePort("ethX1").GetLabel());
  EXPECT_EQ("X2", Interface::CreatePort("ethX2").GetLabel());
  EXPECT_EQ("X11", Interface::CreatePort("ethX11").GetLabel());
  EXPECT_EQ("X12", Interface::CreatePort("ethX12").GetLabel());

  EXPECT_EQ("br0", Interface::CreateBridge("br0").GetLabel());
  EXPECT_EQ("br3", Interface::CreateBridge("br3").GetLabel());

  EXPECT_EQ("wwan0", Interface::CreateWwan("wwan0").GetLabel());

}

TEST(BaseTypes, CreateInterfaceFromLabel) {

  EXPECT_TRUE("X1" == Interface::CreatePort("X1").GetLabel());
  EXPECT_TRUE("ethX1" == Interface::CreatePort("X1").GetName());

  EXPECT_TRUE("X1" == Interface::CreatePort("ethX1").GetLabel());
  EXPECT_TRUE("ethX1" == Interface::CreatePort("ethX1").GetName());

}
}
