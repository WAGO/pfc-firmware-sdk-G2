// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

namespace wago::stp::lib {

enum class protocol_version { STP, RSTP, MSTP };

inline ::std::string to_string(protocol_version protocol) {
  switch (protocol) {
    case protocol_version::STP:
      return "stp";
    case protocol_version::RSTP:
      return "rstp";
    case protocol_version::MSTP:
      return "mstp";
    default:
      return "to string not supported";
  };
}

class stp_port_config {
 public:
  ::std::string port;
  ::std::uint16_t priority  = 8;
  ::std::uint16_t path_cost = 0;
  bool bpdu_guard           = false;
  bool bpdu_filter          = false;
  bool edge_port            = false;
  bool root_guard           = false;
};

inline bool operator==(const stp_port_config &lhs, const stp_port_config &rhs) {
  return ::std::tie(lhs.port, lhs.priority, lhs.path_cost, lhs.bpdu_guard, lhs.bpdu_filter, lhs.edge_port,
                    lhs.root_guard) == ::std::tie(rhs.port, rhs.priority, rhs.path_cost, rhs.bpdu_guard,
                                                  rhs.bpdu_filter, rhs.edge_port, rhs.root_guard);
}

inline bool operator!=(const stp_port_config &lhs, const stp_port_config &rhs) {
  return !(lhs == rhs);
}

struct stp_port_info {
  ::std::string port;
  ::std::string role;
  ::std::string status;
  ::std::string priority;
  ::std::uint32_t path_cost =
      0;  // TODO(Team): is uint32_t correct??? what do we want to see here? (see info_parser -> external-port-cost)
  bool bpdu_guard  = false;
  bool bpdu_filter = false;
  bool edge_port   = false;
  bool root_guard  = false;
};

struct stp_config {
  ::std::uint8_t version    = 1;
  bool enabled              = false;
  protocol_version protocol = protocol_version::STP;
  ::std::string bridge;
  ::std::uint16_t priority     = 8;
  ::std::uint8_t max_age       = 20;
  ::std::uint8_t max_hops      = 20;
  ::std::uint8_t forward_delay = 15;
  ::std::uint8_t hello_time    = 2;
  ::std::vector<stp_port_config> port_configs;
};

inline bool operator==(const stp_config &lhs, const stp_config &rhs) {
  return ::std::tie(lhs.version, lhs.enabled, lhs.protocol, lhs.bridge, lhs.priority, lhs.max_age, lhs.max_hops,
                    lhs.forward_delay, lhs.hello_time, lhs.port_configs) ==
         ::std::tie(rhs.version, rhs.enabled, rhs.protocol, rhs.bridge, rhs.priority, rhs.max_age, rhs.max_hops,
                    rhs.forward_delay, rhs.hello_time, rhs.port_configs);
}
inline bool operator!=(const stp_config &lhs, const stp_config &rhs) {
  return !(lhs == rhs);
}

struct stp_info {
  bool enabled = false;
  ::std::string bridge;
  protocol_version protocol = protocol_version::STP;
  ::std::string priority;
  ::std::uint8_t max_age       = 0;
  ::std::uint8_t max_hops      = 0;
  ::std::uint8_t forward_delay = 0;
  ::std::uint8_t hello_time    = 0;
  ::std::uint32_t path_cost    = 0;
  ::std::vector<stp_port_info> ports;
};

enum class status_code : ::std::uint32_t {
  OK                      = 0,
  WRONG_PARAMETER_PATTERN = 1,
  SYSTEM_CALL_ERROR       = 2,
  JSON_PARSE_ERROR        = 3
};

class status {
 public:
  status() = default;
  status(status_code code, ::std::string &&msg) : s_code{code}, s_msg{msg} {
  }
  virtual ~status()                      = default;
  status(const status &other)            = default;
  status(status &&other)                 = default;
  status &operator=(const status &other) = default;
  status &operator=(status &&other)      = default;

  ::std::string to_string() const {
    return s_msg;
  }

  bool ok() const {
    return s_code == status_code::OK;
  }
  bool operator==(const status &other) const {
    return s_code == other.s_code && s_msg == other.s_msg;
  }

  status_code get_code() const {
    return s_code;
  }

 private:
  status_code s_code = status_code::OK;
  ::std::string s_msg;
};

status configure(const stp_config &config);

status get_persisted(stp_config &config);

// Info: In contrast to stp_config, stp_info has additional information such as port status and port role
status get_info(stp_info &info);

}  // namespace wago::stp::lib