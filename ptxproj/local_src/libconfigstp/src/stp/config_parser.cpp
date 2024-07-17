// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "config_parser.hpp"

#include <functional>
#include <unordered_map>
#include "stp.hpp"

namespace wago::stp::lib {

constexpr int STP_CONFIG_FILE_VERSION = 1;
::std::uint32_t MSTID = 0;

namespace {

protocol_version to_protocol_version(const ::std::string& version) {
  static const ::std::unordered_map<::std::string, protocol_version> versions = {
    {"stp", protocol_version::STP},
    {"rstp", protocol_version::RSTP},
    {"mstp", protocol_version::MSTP}
  };

  if (versions.count(version) > 0){
    return versions.at(version);
  }

  return protocol_version::STP;
}

bool to_bool(const ::std::string& value) {
  return value == "yes";
}

uint16_t to_uint16(::std::string value) {
  if (!value.empty() && std::all_of(value.begin(), value.end(), ::isdigit)) {
    return static_cast<uint16_t>(std::stoi(value));
  }
  return 0;
}

uint8_t to_uint8(::std::string value) {
  if (!value.empty() && std::all_of(value.begin(), value.end(), ::isdigit)) {
    return static_cast<uint8_t>(std::stoi(value));
  }
  return 0;
}

struct config_line {
  ::std::vector<::std::string> v;

  ::std::string parameter() const {
    return !v.empty() ? v[0] : "";
  }
  ::std::string bridge() const {
    return v.size() >= 2 ? v[1] : "";
  }
  ::std::string port() const {
    return v.size() >= 3 ? v[2] : "";
  }
  ::std::string value() const {
    return !v.empty() ? v[v.size() - 1] : "";
  }
};

template<typename Functor>
void set_port_config_parameter(const config_line &l, stp_config &c, Functor&& func) {
  ::std::string port = l.port();
  auto it =
      std::find_if(c.port_configs.begin(), c.port_configs.end(), [&](stp_port_config &p) { return p.port == port; });
  if (it != c.port_configs.end()) {
    func(*it);
  }else{
    stp_port_config pc{port};
    func(pc);
    c.port_configs.emplace_back(pc);
  }
}

void to_config(config_line& line, stp_config &config) {
  // clang-format off
  static const ::std::unordered_map<std::string, ::std::function<void(const config_line &, stp_config &)>> extractors{
      {"stp",          [](const config_line &l, stp_config &c) { c.enabled = to_bool(l.value()); }},
      {"setforcevers", [](const config_line &l, stp_config &c) { c.protocol = to_protocol_version(l.value());
                                                                 c.bridge = l.bridge();}},
      {"settreeprio",  [](const config_line &l, stp_config &c) { c.priority = to_uint16(l.value()); }},
      {"setfdelay",    [](const config_line &l, stp_config &c) { c.forward_delay = to_uint8(l.value()); }},
      {"setmaxage",    [](const config_line &l, stp_config &c) { c.max_age = to_uint8(l.value()); }},
      {"setmaxhops",   [](const config_line &l, stp_config &c) { c.max_hops = to_uint8(l.value()); }},
      {"sethello",     [](const config_line &l, stp_config &c) { c.hello_time = to_uint8(l.value()); }},

      {"settreeportprio",  [](const config_line &l, stp_config &c) {
        set_port_config_parameter(l, c, [&](stp_port_config& pc){pc.priority = to_uint8(l.value());}); }},
      {"setportpathcost",  [](const config_line &l, stp_config &c) {
        set_port_config_parameter(l, c, [&](stp_port_config& pc){pc.path_cost = to_uint16(l.value());}); }},
      {"setportadminedge", [](const config_line &l, stp_config &c) {
        set_port_config_parameter(l, c, [&](stp_port_config& pc){pc.edge_port = to_bool(l.value());}); }},
      {"setportbpdufilter",[](const config_line &l, stp_config &c) {
        set_port_config_parameter(l, c, [&](stp_port_config& pc){pc.bpdu_filter = to_bool(l.value());}); }},
      {"setbpduguard",     [](const config_line &l, stp_config &c) {
        set_port_config_parameter(l, c, [&](stp_port_config& pc){pc.bpdu_guard = to_bool(l.value());}); }},
      {"setportrestrrole", [](const config_line &l, stp_config &c) {
        set_port_config_parameter(l, c, [&](stp_port_config& pc){pc.root_guard = to_bool(l.value());}); }}};
  // clang-format on

  if (extractors.count(line.parameter()) > 0) {
    extractors.at(line.parameter())(line, config);
  }
}

}  // namespace

stp_config from_stream(::std::stringstream &ss) {
  stp_config config;
  ::std::string line;
  while (std::getline(ss, line)) {
    ::std::stringstream line_ss(line);
    ::std::string token;
    config_line c_line;
    while (getline(line_ss, token, ' ')) {
      c_line.v.push_back(token);
    }
    to_config(c_line, config);
  }
  return config;
}

::std::stringstream to_stream(stp_config config, bool only_mstp_config) {
  ::std::stringstream ss;
  // clang-format off
  if(not only_mstp_config){
    ss << "#Generale stp/rstp parameters" << ::std::endl;
    ss << "version" << " " << (STP_CONFIG_FILE_VERSION) << ::std::endl;
    ss << "stp"          << " " << config.bridge << " " << (config.enabled ? "yes" : "no") << ::std::endl;
    ss << ::std::endl;
  }

  ss << "#Bridge specific stp/rstp parameters" << ::std::endl;
  ss << "setforcevers" << " " << config.bridge << " " << to_string(config.protocol) << ::std::endl;
  ss << "settreeprio"  << " " << config.bridge << " " << MSTID << " " << static_cast<uint32_t>(config.priority) << ::std::endl;

  if (only_mstp_config) {
    ss << "setmaxage"    << " " << config.bridge << " " << static_cast<uint32_t>(6) << ::std::endl;
  }
  ss << "setfdelay"    << " " << config.bridge << " " << static_cast<uint32_t>(config.forward_delay) << ::std::endl;
  ss << "setmaxage"    << " " << config.bridge << " " << static_cast<uint32_t>(config.max_age) << ::std::endl;
  ss << "setmaxhops"    << " " << config.bridge << " " << static_cast<uint32_t>(config.max_hops) << ::std::endl;
  ss << "sethello"     << " " << config.bridge << " " << static_cast<uint32_t>(config.hello_time) << ::std::endl;
  ss << ::std::endl;
  // clang-format on

  ss << "#Port specific stp/rstp parameters" << ::std::endl;
  for (auto &port : config.port_configs) {
    // clang-format off
    ss << "settreeportprio"   << " " << config.bridge << " " << port.port << " " << MSTID << " " << port.priority << ::std::endl;
    ss << "setportpathcost"   << " " << config.bridge << " " << port.port << " " << (port.path_cost) << ::std::endl;
    ss << "setbpduguard"      << " " << config.bridge << " " << port.port << " " << (port.bpdu_guard ? "yes" : "no") << ::std::endl;
    ss << "setportbpdufilter" << " " << config.bridge << " " << port.port << " " << (port.bpdu_filter ? "yes" : "no") << ::std::endl;
    ss << "setportadminedge"  << " " << config.bridge << " " << port.port << " " << (port.edge_port ? "yes" : "no") << ::std::endl;
    ss << "setportrestrrole"  << " " << config.bridge << " " << port.port << " " << (port.root_guard ? "yes" : "no") << ::std::endl;
    ss << ::std::endl;
    // clang-format on
  }

  return ss;
}

}  // namespace wago::stp::lib