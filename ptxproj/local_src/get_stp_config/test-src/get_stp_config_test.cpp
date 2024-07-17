// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-actions.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stp.hpp>
#include <string>
#include <vector>

#include "network_config.hpp"
#include "stp_config_output.hpp"
#include "stp_data_provider.hpp"
#include "stp_data_provider_mock.hpp"

namespace {

constexpr auto json_config_one_port =
    R"({"bridge":"one_interface","enabled":false,"forward-delay":15,"hello-time":2,"max-age":20,"max-hops":20,"port-configs":[{"bpdu-filter":false,"bpdu-guard":false,"edge-port":false,"path-cost":0,"port":"X1","priority":8,"root-guard":false}],"priority":8,"protocol":"stp","version":1})";

constexpr auto json_config_two_ports =
    R"({"bridge":"two_interfaces","enabled":true,"forward-delay":30,"hello-time":4,"max-age":40,"max-hops":35,"port-configs":[{"bpdu-filter":false,"bpdu-guard":true,"edge-port":true,"path-cost":190,"port":"X1","priority":3,"root-guard":false},{"bpdu-filter":true,"bpdu-guard":false,"edge-port":false,"path-cost":200,"port":"X2","priority":4,"root-guard":true}],"priority":16,"protocol":"rstp","version":1})";

constexpr auto json_info_one_port =
    R"({"bridge":"two_interfaces","enabled":true,"forward-delay":15,"hello-time":2,"max-age":20,"max-hops":25,"path-cost":200000,"ports":[{"bpdu-filter":false,"bpdu-guard":false,"edge-port":false,"path-cost":1,"port":"X1","priority":"8.001","role":"Designated","root-guard":false,"status":"forwarding"}],"priority":"8.000.00:30:DE:40:82:16","protocol":"rstp"})";

::std::vector<::std::string> split_lines(const ::std::string& data) {
  ::std::string line;
  ::std::vector<::std::string> lines;

  ::std::stringstream ss(data);
  while (getline(ss, line, '\n')) {
    lines.push_back(line);
  }

  return lines;
}

MATCHER_P(StrLengthIsLessOrEqualTo, max_length, "") {
  return arg.length() <= max_length;
}

}  // namespace

namespace nlohmann {
void PrintTo(const ::nlohmann::json& j, std::ostream* os) {
  *os << j.dump();
}
}  // namespace nlohmann

namespace wago::stp::lib {

::std::string bool_to_string(bool value) {
  return (value) ? "true" : "false";
}

::std::string protocol_to_string(protocol_version value) {
  return (value == protocol_version::STP) ? "STP" : "RSTP";
}

void PrintTo(const stp_port_config& config, std::ostream* os) {
  *os << "(" << config.port << ", " << std::to_string(config.priority) << ", " << std::to_string(config.path_cost)
      << ", " << bool_to_string(config.bpdu_guard) << ", " << bool_to_string(config.bpdu_filter) << ", "
      << bool_to_string(config.edge_port) << ", " << bool_to_string(config.root_guard) << ")";
}

void PrintTo(const stp_config& config, std::ostream* os) {
  *os << "(" << std::to_string(config.version) << ", " << bool_to_string(config.enabled) << ", "
      << protocol_to_string(config.protocol) << ", " << config.bridge << ", " << std::to_string(config.priority) << ", "
      << std::to_string(config.max_age) << ", " << std::to_string(config.forward_delay) << ", "
      << std::to_string(config.hello_time);

  for (const auto& pc : config.port_configs) {
    PrintTo(pc, os);
  }

  *os << ")";
}

}  // namespace wago::stp::lib

namespace wago::stp::get_stp_config {

using namespace wago::stp::lib;  // NOLINT google-build-using-namespace
using namespace ::testing;       // NOLINT google-build-using-namespace

TEST(config_to_json, convert_stp_config) {
  ::std::vector<stp_port_config> port_configs{{"X1", 3, 190, true, false, true, false},
                                              {"X2", 4, 200, false, true, false, true}};
  stp_config config{1, true, protocol_version::RSTP, "two_interfaces", 16, 40, 35, 30, 4, port_configs};

  nlohmann::json j{};
  status s = config_to_json(j, config);

  ASSERT_TRUE(s.ok());
  EXPECT_THAT(j.dump(), json_config_two_ports);
}

TEST(config_to_json, reject_unsupported_config_version) {
  stp_config config{};
  config.version = 2;

  nlohmann::json j{};
  status s = config_to_json(j, config);

  status expected_status{status_code::JSON_PARSE_ERROR, "Unsupported config version"};
  EXPECT_THAT(s, expected_status);
}

TEST(config_to_json, print_json_config) {
  stp_data_provider_mock provider_mock;

  auto expected_json = ::nlohmann::json::parse(json_config_two_ports);

  // clang-format off
  stp_config config{1, true, protocol_version::RSTP, "two_interfaces", 16, 40, 35, 30, 4,
                  {{"X1", 3, 190, true, false, true, false},
                    {"X2", 4, 200, false, true, false, true}}};

  EXPECT_CALL(provider_mock, get_stp_config(_))
    .WillOnce(DoAll(
      SetArgReferee<0>(config), 
      Return(status{})
    ));
  // clang-format on

  testing::internal::CaptureStdout();
  print_json_config(option_map{}, provider_mock);
  std::string output = testing::internal::GetCapturedStdout();

  EXPECT_EQ(::nlohmann::json::parse(output), expected_json);
}

TEST(config_to_json, print_json_info) {
  stp_data_provider_mock provider_mock;

  auto expected_json = ::nlohmann::json::parse(json_info_one_port);

  stp_port_info port_info{};
  port_info.edge_port   = false;
  port_info.port        = "X1";
  port_info.bpdu_filter = false;
  port_info.bpdu_guard  = false;
  port_info.root_guard  = false;
  port_info.path_cost   = 1;
  port_info.priority    = "8.001";
  port_info.role        = "Designated";
  port_info.status      = "forwarding";

  stp_info info{};
  info.bridge        = "two_interfaces";
  info.enabled       = true;
  info.forward_delay = 15;
  info.hello_time    = 2;
  info.max_age       = 20;
  info.max_hops      = 25;
  info.path_cost     = 200000;
  info.ports         = {port_info};
  info.priority      = "8.000.00:30:DE:40:82:16";
  info.protocol      = protocol_version::RSTP;

  // clang-format off
  EXPECT_CALL(provider_mock, get_stp_info(_))
    .WillOnce(DoAll(
      SetArgReferee<0>(info), 
      Return(status{})
    ));
  // clang-format on

  testing::internal::CaptureStdout();
  print_json_info(option_map{}, provider_mock);
  std::string output = testing::internal::GetCapturedStdout();

  EXPECT_EQ(::nlohmann::json::parse(output), expected_json);
}

TEST(config_to_json, format_json_for_backup_v1) {
  stp_data_provider_mock provider_mock;

  auto expected_json = ::nlohmann::json::parse(json_config_two_ports);

  // clang-format off
  stp_config config{1, true, protocol_version::RSTP, "two_interfaces", 16, 40, 35, 30, 4,
                  {{"X1", 3, 190, true, false, true, false},
                    {"X2", 4, 200, false, true, false, true}}};

  EXPECT_CALL(provider_mock, get_stp_config(_))
    .WillOnce(DoAll(
      SetArgReferee<0>(config), 
      Return(status{})
    ));
  // clang-format on

  testing::internal::CaptureStdout();
  print_backup_content(option_map{}, provider_mock);
  std::string output = testing::internal::GetCapturedStdout();

  auto lines = split_lines(output);
  EXPECT_THAT(lines, Each(StartsWith("stp.data=")));
  EXPECT_THAT(lines, Each(StrLengthIsLessOrEqualTo(88)));

  ::boost::replace_all(output, "stp.data=", "");
  ::boost::replace_all(output, "\n", "");
  EXPECT_EQ(::nlohmann::json::parse(output), expected_json);
}

TEST(config_to_json, format_json_for_backup_with_targetversion) {
  stp_data_provider_mock provider_mock;

  auto expected_json = ::nlohmann::json::parse(json_config_two_ports);

  option_map options = {{action_option::targetversion, "V04.04.12"}};

  // clang-format off
  stp_config config{1, true, protocol_version::RSTP, "two_interfaces", 16, 40, 35, 30, 4,
                    {{"X1", 3, 190, true, false, true, false},
                     {"X2", 4, 200, false, true, false, true}}};

  EXPECT_CALL(provider_mock, get_stp_config(_))
    .WillOnce(DoAll(
      SetArgReferee<0>(config), 
      Return(status{})
    ));
  // clang-format on

  testing::internal::CaptureStdout();
  print_backup_content(options, provider_mock);
  std::string output = testing::internal::GetCapturedStdout();

  auto lines = split_lines(output);
  EXPECT_THAT(lines, Each(StartsWith("stp.data=")));
  EXPECT_THAT(lines, Each(StrLengthIsLessOrEqualTo(88)));

  ::boost::replace_all(output, "stp.data=", "");
  ::boost::replace_all(output, "\n", "");
  EXPECT_EQ(::nlohmann::json::parse(output), expected_json);
}

TEST(config_to_json, update_stp_config_initially_containing_no_ports) {
  stp_config config;
  ::std::vector<::std::string> ports = {"X1", "X2"};

  ::std::vector<stp_port_config> expected_port_configs = {{"X1", 8, 0, false, false, false, false},
                                                          {"X2", 8, 0, false, false, false, false}};
  stp_config expected_stp_config = stp_config{1, false, protocol_version::STP, "", 8, 20, 20, 15, 2, expected_port_configs};

  update_stp_config_regarding_bridge_ports(ports, config);

  EXPECT_THAT(config, Eq(expected_stp_config));
}

TEST(config_to_json, update_stp_config_initially_containing_one_port) {
  stp_config config;
  ::std::vector<::std::string> ports = {"X1", "X2"};
  config.port_configs                = {{"X1", 5, 1, true, true, true, true}};

  ::std::vector<stp_port_config> expected_port_configs = {{"X1", 5, 1, true, true, true, true},
                                                          {"X2", 8, 0, false, false, false, false}};
  stp_config expected_stp_config = stp_config{1, false, protocol_version::STP, "", 8, 20, 20, 15, 2, expected_port_configs};

  update_stp_config_regarding_bridge_ports(ports, config);

  EXPECT_THAT(config, Eq(expected_stp_config));
}

TEST(config_to_json, update_stp_config_initially_containing_all_ports) {
  stp_config config;
  ::std::vector<::std::string> ports = {"X1", "X2"};
  config.port_configs                = {{"X1", 5, 1, true, true, true, true}, {"X2", 8, 0, false, false, false, false}};

  ::std::vector<stp_port_config> expected_port_configs = {{"X1", 5, 1, true, true, true, true},
                                                          {"X2", 8, 0, false, false, false, false}};
  stp_config expected_stp_config = stp_config{1, false, protocol_version::STP, "", 8, 20, 20, 15, 2, expected_port_configs};

  update_stp_config_regarding_bridge_ports(ports, config);

  EXPECT_THAT(config, Eq(expected_stp_config));
}

TEST(config_to_json, update_stp_config_initially_containing_unavailable_port) {
  stp_config config;
  ::std::vector<::std::string> ports = {"X1", "X11"};
  config.port_configs                = {{"X1", 5, 1, true, true, true, true}, {"X2", 8, 0, false, false, false, false}};

  ::std::vector<stp_port_config> expected_port_configs = {{"X1", 5, 1, true, true, true, true},
                                                          {"X11", 8, 0, false, false, false, false}};
  stp_config expected_stp_config = stp_config{1, false, protocol_version::STP, "", 8, 20, 20, 15, 2, expected_port_configs};

  update_stp_config_regarding_bridge_ports(ports, config);

  EXPECT_THAT(config, Eq(expected_stp_config));
}

TEST(config_to_json, get_stp_config_as_json) {
  nlohmann::json j;
  stp_config config;
  config.bridge = "one_interface";  // Signal network-config-fake to return X1 port

  auto expected_json = nlohmann::json::parse(json_config_one_port);

  status s = get_stp_config_as_json(config, j);

  EXPECT_THAT(j, Eq(expected_json));
}

TEST(config_to_json, execute_action_for_help) {
  parameter_list parameters = {action::help};
  option_map options;

  testing::internal::CaptureStdout();
  status s           = execute_action(parameters, options, stp_data_provider{});
  std::string output = testing::internal::GetCapturedStdout();

  ASSERT_THAT(s, Eq(status{}));
  EXPECT_THAT(output, HasSubstr("-c|--config"));
  EXPECT_THAT(output, HasSubstr("-i|--info"));
  EXPECT_THAT(output, HasSubstr("-b|--backup"));
  EXPECT_THAT(output, HasSubstr("-h|--help"));

  EXPECT_THAT(output, HasSubstr("-t|--backup-targetversion"));
  EXPECT_THAT(output, HasSubstr("-p|--pretty"));
}

TEST(config_to_json, execute_action_with_unsupported_parameter_count) {
  parameter_list parameters = {action::help, action::json_config};

  status s = execute_action(parameters, option_map{}, stp_data_provider{});

  ASSERT_THAT(s.get_code(), Eq(status_code::WRONG_PARAMETER_PATTERN));
}

}  // namespace wago::stp::get_stp_config