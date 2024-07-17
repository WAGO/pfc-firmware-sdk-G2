// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "configure.hpp"

#include <string>

#include "config_parser.hpp"
#include "file.hpp"
#include "program.hpp"
#include "stp.hpp"

namespace wago::stp::lib {

constexpr auto STP_CONFIG_PERSISTENCE_PATH  = "/etc/specific/stp.conf";
constexpr auto MSTPD_BATCH_FILE_CONFIG_PATH = "/tmp/mstpd_batch_file.conf";

namespace {

status configure_mstp_deamon(::std::string &&path) {
  auto p = program::execute("/usr/sbin/mstpctl --batch " + path);
  status s{};
  if (!p.get_stdout().empty()) {
    s = status{status_code::WRONG_PARAMETER_PATTERN, p.get_stdout()};
  } else if (p.get_result() != 0) {
    s = status{status_code::SYSTEM_CALL_ERROR, p.get_stderr()};
  }
  return s;
}

}  // namespace

status configure_stp(const stp_config &config) {
  ::std::stringstream ss = to_stream(config, true);
  status s               = write(ss, MSTPD_BATCH_FILE_CONFIG_PATH);
  if (s.ok()) {
    s = configure_mstp_deamon(MSTPD_BATCH_FILE_CONFIG_PATH);
  }
  return s;
}

status read_persistence(stp_config &config) {
  ::std::stringstream ss;
  status s = read(ss, STP_CONFIG_PERSISTENCE_PATH);
  if (s.ok()) {
    config = from_stream(ss);
  }
  return s;
}

status write_persistence(const stp_config &config) {
  ::std::stringstream ss = to_stream(config);
  return write(ss, STP_CONFIG_PERSISTENCE_PATH);
}

}  // namespace wago::stp::lib