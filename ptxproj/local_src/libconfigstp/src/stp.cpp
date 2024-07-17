// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "stp.hpp"

#include "configure.hpp"
#include "setup.hpp"
#include "info.hpp"

namespace wago::stp::lib {

status configure(const stp_config& config) {
  status s = setup_stp(config);
  if (s.ok() && config.enabled) {
    s = configure_stp(config);
  }
  if (s.ok()) {
    s = write_persistence(config);
  }
  return s;
}

status get_persisted(stp_config& config) {
  return read_persistence(config);
}

status get_info(stp_info& info) {
  return get_stp_info(info);
}

}  // namespace wago::stp::lib
