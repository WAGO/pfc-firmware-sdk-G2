// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
#include "CommonTestDependencies.hpp"

#include <vector>
#include <string>
#include <algorithm>

#include "alphanum.hpp"

using std::vector;
using std::string;

TEST(StringSort, WithAVectorOfPortNames)
{
  vector<string> initalOrder = {"ethX2", "ethX12", "ethX1", "ethX11"};
  vector<string> expectedOrder = {"ethX1", "ethX2", "ethX11", "ethX12"};

  std::sort(initalOrder.begin(), initalOrder.end(), ::doj::alphanum_less<string>());

  EXPECT_THAT(initalOrder, testing::ElementsAreArray(expectedOrder));

}

//---- End of source file ------------------------------------------------------

