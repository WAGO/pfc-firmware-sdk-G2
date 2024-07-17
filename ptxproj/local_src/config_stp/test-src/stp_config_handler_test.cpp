// Copyright (c) 2024 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "stp_config_handler.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nlohmann/json.hpp>
#include <stdexcept>
#include <vector>

#include "stp.hpp"

namespace wago::stp::config_stp {

using namespace wago::stp::lib;  // NOLINT google-build-using-namespace
using namespace ::testing;       // NOLINT google-build-using-namespace


namespace {
  void add_default_port_config_for(const ::std::string& port_name, stp_config& config) {
    stp_port_config port_config;
    port_config.port = port_name;
    config.port_configs.push_back(port_config);
  }
}

TEST(edit_parameters, bridge_parameter) {
  auto json_string =
    R"({"enabled": true, "version": 1, "protocol":"stp", "bridge": "br1", "priority": 10, "max-age": 5, "max-hops": 6, "forward-delay": 7, "hello-time": 3})";

  stp_config config{};
  edit_parameters(json_string, config);

  EXPECT_THAT(config.enabled, true);
  EXPECT_THAT(config.protocol, protocol_version::STP);
  EXPECT_THAT(config.bridge, "br1");
  EXPECT_THAT(config.priority, 10);
  EXPECT_THAT(config.max_age, 5);
  EXPECT_THAT(config.max_hops, 6);
  EXPECT_THAT(config.forward_delay, 7);
  EXPECT_THAT(config.hello_time, 3);
}

TEST(edit_parameters, empty_port_configs) {
  auto json_string =
    R"({"enabled": true, "version": 1, "port-configs": []})";

  stp_config config{};
  edit_parameters(json_string, config);

  EXPECT_THAT(config.enabled, true);
}

TEST(edit_parameters, add_new_ports) {
  // Ensure we can handle port labels (X1) and names (ethX2).
  auto input = R"({"port-configs": [
      {"port":"X1", "priority": 4, "path-cost":200, "bpdu-guard":false, "bpdu-filter":true, "edge-port":false, "root-guard":true},
      {"port": "ethX2", "priority": 3, "path-cost":201, "bpdu-guard":true, "bpdu-filter":false,"edge-port":true, "root-guard":false}
      ]})";

  ::std::vector<stp_port_config> expected_pc{{"ethX1", 4, 200, false, true, false, true},
                                             {"ethX2", 3, 201, true, false, true, false}};
  
  stp_config config{};
  edit_parameters(input, config);

  ASSERT_GT(config.port_configs.size(), 0);

  EXPECT_EQ(expected_pc[0].port, config.port_configs[0].port);
  EXPECT_EQ(expected_pc[0].priority, config.port_configs[0].priority);
  EXPECT_EQ(expected_pc[0].path_cost, config.port_configs[0].path_cost);
  EXPECT_EQ(expected_pc[0].bpdu_guard, config.port_configs[0].bpdu_guard);
  EXPECT_EQ(expected_pc[0].bpdu_filter, config.port_configs[0].bpdu_filter);
  EXPECT_EQ(expected_pc[0].edge_port, config.port_configs[0].edge_port);
  EXPECT_EQ(expected_pc[0].root_guard, config.port_configs[0].root_guard);

  ASSERT_EQ(config.port_configs.size(), 2);

  EXPECT_EQ(expected_pc[1].port, config.port_configs[1].port);
  EXPECT_EQ(expected_pc[1].priority, config.port_configs[1].priority);
  EXPECT_EQ(expected_pc[1].path_cost, config.port_configs[1].path_cost);
  EXPECT_EQ(expected_pc[1].bpdu_guard, config.port_configs[1].bpdu_guard);
  EXPECT_EQ(expected_pc[1].bpdu_filter, config.port_configs[1].bpdu_filter);
  EXPECT_EQ(expected_pc[1].edge_port, config.port_configs[1].edge_port);
  EXPECT_EQ(expected_pc[1].root_guard, config.port_configs[1].root_guard);
}

TEST(edit_parameters, edit_existing_port_parameters) {
  // Ensure we can handle port labels (X1) and names (ethX2).
  auto input = R"({"port-configs": [
      {"port":"X1", "priority": 4, "path-cost":200, "bpdu-guard":false, "bpdu-filter":true, "edge-port":false, "root-guard":true},
      {"port": "ethX2", "priority": 3, "path-cost":201, "bpdu-guard":true, "bpdu-filter":false,"edge-port":true, "root-guard":false}
      ]})";

  ::std::vector<stp_port_config> expected_pc{{"ethX1", 4, 200, false, true, false, true},
                                             {"ethX2", 3, 201, true, false, true, false}};

  stp_config config{};
  add_default_port_config_for("ethX1", config);
  add_default_port_config_for("ethX2", config);
  
  edit_parameters(input, config);

  ASSERT_GT(config.port_configs.size(), 0);

  EXPECT_EQ(expected_pc[0].port, config.port_configs[0].port);
  EXPECT_EQ(expected_pc[0].priority, config.port_configs[0].priority);
  EXPECT_EQ(expected_pc[0].path_cost, config.port_configs[0].path_cost);
  EXPECT_EQ(expected_pc[0].bpdu_guard, config.port_configs[0].bpdu_guard);
  EXPECT_EQ(expected_pc[0].bpdu_filter, config.port_configs[0].bpdu_filter);
  EXPECT_EQ(expected_pc[0].edge_port, config.port_configs[0].edge_port);
  EXPECT_EQ(expected_pc[0].root_guard, config.port_configs[0].root_guard);

  ASSERT_EQ(config.port_configs.size(), 2);

  EXPECT_EQ(expected_pc[1].port, config.port_configs[1].port);
  EXPECT_EQ(expected_pc[1].priority, config.port_configs[1].priority);
  EXPECT_EQ(expected_pc[1].path_cost, config.port_configs[1].path_cost);
  EXPECT_EQ(expected_pc[1].bpdu_guard, config.port_configs[1].bpdu_guard);
  EXPECT_EQ(expected_pc[1].bpdu_filter, config.port_configs[1].bpdu_filter);
  EXPECT_EQ(expected_pc[1].edge_port, config.port_configs[1].edge_port);
  EXPECT_EQ(expected_pc[1].root_guard, config.port_configs[1].root_guard);
}

TEST(edit_parameters, clear_port_configs_when_bridge_changes) {
  auto json_string =
    R"({"enabled": true, "version": 1, "protocol":"stp", "bridge": "br1"})";

  stp_config config{};
  config.bridge = "br0";
  add_default_port_config_for("ethX2", config);
  ASSERT_EQ(config.port_configs.size(), 1);

  edit_parameters(json_string, config);

  EXPECT_THAT(config.bridge, "br1");
  EXPECT_EQ(config.port_configs.size(), 0);
}

TEST(edit_parameters, throw_exception_for_newer_config_versions) {
  auto input = R"({"version": 2})";

  stp_config config{};
  EXPECT_THROW(edit_parameters(input, config), stp_config_exception);
}

TEST(edit_parameters, throw_exception_for_field_data_type_mismatch) {
  auto input = R"({"version": "true"})";

  stp_config config{};
  EXPECT_THROW(edit_parameters(input, config), stp_config_exception);
}

TEST(edit_parameters, throw_on_invalid_parameter) {
  auto input = R"({"asfdasf": 2)";

  stp_config config{};
  EXPECT_THROW(edit_parameters(input, config), stp_config_exception);
}

TEST(edit_parameters, throw_on_invalid_port_configs) {
  auto input = R"({"port-configs": {"asfdasf": 2}})";

  stp_config config{};
  EXPECT_THROW(edit_parameters(input, config), stp_config_exception);
}

TEST(edit_parameters, throw_on_invalid_port_configs_parameter) {
  auto input = R"({"port-configs": [{"port":"X2", {"asfdasf": 2}]})";

  stp_config config{};
  EXPECT_THROW(edit_parameters(input, config), stp_config_exception);
}

TEST(edit_parameters, throw_on_invalid_parameter_range) {
  auto input = R"({"port-configs": [{"port":"X2", "path-cost": -2}]})";

  stp_config config{};
  EXPECT_THROW(edit_parameters(input, config), stp_config_exception);
}


}  // namespace wago::stp::config_stp