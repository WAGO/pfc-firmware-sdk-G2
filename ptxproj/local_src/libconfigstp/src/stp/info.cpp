// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "info.hpp"

#include <string>

#include "configure.hpp"
#include "info_parser.hpp"
#include "program.hpp"
#include "setup.hpp"

namespace wago::stp::lib {

constexpr auto STP_BRIDGE_INFO_CMD     = "mstpctl showbridge -f json ";
constexpr auto STP_PORTDETAIL_INFO_CMD = "mstpctl showportdetail -f json ";

namespace {

status call_program(::std::string&& cmd, ::std::string& std_out) {
  auto p = program::execute(cmd);

  if (p.get_result() != 0) {
    return status{status_code::SYSTEM_CALL_ERROR, p.get_stderr()};
  }
  std_out = p.get_stdout();
  return status{};
}

int get_pid_of(const ::std::string &daemon_path) {
  ::std::string cmd_line = "/usr/bin/pidof " + daemon_path;
  auto program           = program::execute(cmd_line);
  if (program.get_result() == 0) {
    return static_cast<int>(::std::strtol(program.get_stdout().c_str(), nullptr, 0));
  }
  return 0;
}

bool mstp_deamon_running() {
  return get_pid_of("/usr/sbin/mstpd") > 0;
}

}  // namespace

status get_stp_info(stp_info& info) {
  info = stp_info{};
  status s{};
  if (mstp_deamon_running()) {
    info.enabled = true;

    stp_config config{};
    read_persistence(config);

    ::std::string json_bridge_info;
    s = call_program(STP_BRIDGE_INFO_CMD + config.bridge, json_bridge_info);
    if (s.ok()) {
      s = parse_stp_info(json_bridge_info, info);
    }

    ::std::string json_port_info;
    if (s.ok()) {
      s = call_program(STP_PORTDETAIL_INFO_CMD + config.bridge, json_port_info);
    }
    if (s.ok()) {
      s = parse_stp_info(json_port_info, info);
    }
  }

  return s;
}
}  // namespace wago::stp::lib