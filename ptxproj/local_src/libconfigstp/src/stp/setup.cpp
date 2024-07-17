// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "setup.hpp"

#include "configure.hpp"
#include "info.hpp"
#include "program.hpp"
#include "stp.hpp"

namespace wago::stp::lib {

namespace {

status call_programm(::std::string &&cmd_line) {
  auto p = program::execute(cmd_line);
  if (p.get_result() != 0) {
    return status{status_code::SYSTEM_CALL_ERROR, p.get_stderr()};
  }
  return status{};
}

status configure_multicast_mac_for_stp(bool add, const ::std::string &bridge) {
  ::std::string add_or_delete = add ? ::std::string("add") : ::std::string("del");
  status s = call_programm("/usr/sbin/ip maddr " + add_or_delete + " 01:80:c2:00:00:00 dev " + bridge);
  if(s.ok()){
    //Info: When you call it again with add or del,
    //the bridge tool reports an error indicating that the setting has already been made.
    //Unfortunately the tool doesn't return OK the second time, so we ignore the status.
    call_programm("/usr/sbin/bridge fdb " + add_or_delete + " dev " + bridge + " 01:80:c2:00:00:00");
  }
  return s;
}

status bridge_stp(bool enable, const ::std::string &bridge) {
  ::std::string enable_or_disable = enable ? ::std::string("1") : ::std::string("0");
  return call_programm("/usr/sbin/ip link set " + bridge + " type bridge stp_state " + enable_or_disable);
}

status mstp_deamon(bool start, const ::std::string &bridge) {
  ::std::string start_or_stop = start ? ::std::string("start") : ::std::string("stop");
  return call_programm("/usr/sbin/bridge-stp " + bridge + " " + start_or_stop);
}

status enable_stp(const ::std::string &bridge) {
  status s = configure_multicast_mac_for_stp(true, bridge);
  if (s.ok()) {
    s = bridge_stp(true, bridge);
    if (s.ok()) {
      s = mstp_deamon(true, bridge);
    }
  }
  return s;
}

bool should_stp_be_enabled(const stp_config &config) {
  return config.enabled && !config.bridge.empty();
}

void disable_stp_when_not_needed(const stp_config &config) {
  stp_config persisted_config{};
  read_persistence(persisted_config);

  // Case 1: STP is switched off
  // Case 2: Another bridge is used as STP-Bridge. Then the old one has to be removed.
  // Case 3: If the old STP-Bridge is also the new one, then nothing happens
  ::std::string stp_bridge_to_be_removed = persisted_config.bridge;
  if (config.enabled && config.bridge == persisted_config.bridge) {
    stp_bridge_to_be_removed.clear();
  }

  if (!stp_bridge_to_be_removed.empty()) {
    mstp_deamon(false, stp_bridge_to_be_removed);
    bridge_stp(false, stp_bridge_to_be_removed);
    configure_multicast_mac_for_stp(false, stp_bridge_to_be_removed);
  }
}

}  // namespace

status setup_stp(const stp_config &config) {
  disable_stp_when_not_needed(config);

  if (should_stp_be_enabled(config)) {
    return enable_stp(config.bridge);
  }
  return {};
}

}  // namespace wago::stp::lib