// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "config_parser.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>

#include "gmock/gmock-matchers.h"
#include "stp.hpp"

namespace wago::stp::lib {

using ::testing::Matcher;
using ::testing::Field;


class config_parser_test : public testing::Test {
 public:
  ::std::string persistence_file_content = R"(#Generale stp/rstp parameters
version 1
stp br0 yes

#Bridge specific stp/rstp parameters
setforcevers br0 rstp
settreeprio br0 0 9
setfdelay br0 21
setmaxage br0 22
setmaxhops br0 23
sethello br0 3

#Port specific stp/rstp parameters
settreeportprio br0 ethX1 0 10
setportpathcost br0 ethX1 200
setbpduguard br0 ethX1 no
setportbpdufilter br0 ethX1 yes
setportadminedge br0 ethX1 no
setportrestrrole br0 ethX1 yes

settreeportprio br0 ethX2 0 11
setportpathcost br0 ethX2 201
setbpduguard br0 ethX2 yes
setportbpdufilter br0 ethX2 no
setportadminedge br0 ethX2 yes
setportrestrrole br0 ethX2 no

)";

::std::string mstpctl_batch_file_content = R"(#Bridge specific stp/rstp parameters
setforcevers br0 rstp
settreeprio br0 0 9
setmaxage br0 6
setfdelay br0 21
setmaxage br0 22
setmaxhops br0 23
sethello br0 3

#Port specific stp/rstp parameters
settreeportprio br0 ethX1 0 10
setportpathcost br0 ethX1 200
setbpduguard br0 ethX1 no
setportbpdufilter br0 ethX1 yes
setportadminedge br0 ethX1 no
setportrestrrole br0 ethX1 yes

settreeportprio br0 ethX2 0 11
setportpathcost br0 ethX2 201
setbpduguard br0 ethX2 yes
setportbpdufilter br0 ethX2 no
setportadminedge br0 ethX2 yes
setportrestrrole br0 ethX2 no

)";

  // clang-format off
  stp_config config{1,
                    true,
                    protocol_version::RSTP,
                    "br0",
                    9,
                    22,
                    23,
                    21,
                    3,
                    {{"ethX1", 10, 200, false, true, false, true},
                    {"ethX2", 11, 201, true, false, true, false}}};
  };
  // clang-format on

static std::vector<Matcher<stp_port_config>> get_expected_port_config(stp_config& config) {
  using ElementType = decltype(config.port_configs)::value_type;
  std::vector<Matcher<stp_port_config>> m_expected_port_config;

  // clang-format off
  for (auto&& p : config.port_configs) {
    m_expected_port_config.push_back(
        AllOf(
          Field(&ElementType::port, p.port),
          Field(&ElementType::path_cost, p.path_cost),
          Field(&ElementType::priority, p.priority),
          Field(&ElementType::bpdu_filter, p.bpdu_filter),
          Field(&ElementType::edge_port, p.edge_port),
          Field(&ElementType::bpdu_guard, p.bpdu_guard),
          Field(&ElementType::root_guard, p.root_guard)));
  }
  // clang-format on
  return m_expected_port_config;
}

TEST_F(config_parser_test, from_stream) {
  ::std::stringstream ss(persistence_file_content);
  stp_config c = from_stream(ss);
  EXPECT_EQ(config.enabled, c.enabled);
  EXPECT_EQ(config.bridge, c.bridge);
  EXPECT_EQ(config.forward_delay, c.forward_delay);
  EXPECT_EQ(config.hello_time, c.hello_time);
  EXPECT_EQ(config.max_age, c.max_age);
  EXPECT_EQ(config.max_hops, c.max_hops);
  EXPECT_EQ(config.priority, c.priority);
  EXPECT_EQ(config.version, c.version);

  EXPECT_THAT(c.port_configs, ElementsAreArray(get_expected_port_config(config)));
}

TEST_F(config_parser_test, to_stream) {
  ::std::stringstream ss = to_stream(config);

  EXPECT_EQ(persistence_file_content, ss.str());
}

TEST_F(config_parser_test, to_stream_only_mstp_config) {
  ::std::stringstream ss = to_stream(config,true);

  EXPECT_EQ(mstpctl_batch_file_content, ss.str());
}


}  // namespace wago::stp::lib