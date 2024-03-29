// SPDX-License-Identifier: GPL-2.0-or-later

#include "CommonTestDependencies.hpp"

#include "HostConfFileParser.hpp"

namespace netconf {

class HostConfFileParserTest : public testing::Test {
 public:
  void SetUp() override {
  }

  void TearDown() override {
  }
};

TEST_F(HostConfFileParserTest, parseContent) {
  ::std::string content =
      R"(
# comment 132325425342345
# do not edit this file
HOSTNAME=hostname
DNSDOMAIN=domain
)";

  HostConfFileParser parser;
  parser.Parse(content);

  EXPECT_EQ(parser.GetHostname(), "hostname");
  EXPECT_EQ(parser.GetDomain(), "domain");
}

TEST_F(HostConfFileParserTest, parseContentWithEmptyHostname) {
  ::std::string content = R"(
# comment 132325425342345
# do not edit this file
HOSTNAME=
DNSDOMAIN=domain
)";

  HostConfFileParser parser;
  parser.Parse(content);

  EXPECT_EQ(parser.GetHostname(), "");
  EXPECT_EQ(parser.GetDomain(), "domain");
}

TEST_F(HostConfFileParserTest, parseContentWithMissingValues) {
  ::std::string content =
      R"(
# comment 132325425342345
# do not edit this file
BLA=yyy
BLUB=xxx
)";

  HostConfFileParser parser;
  parser.Parse(content);

  EXPECT_EQ(parser.GetHostname(), "");
  EXPECT_EQ(parser.GetDomain(), "");
}

}  // namespace netconf
