// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <ostream>

#include "Status.hpp"
#include "BaseTypes.hpp"

namespace netconf {

inline void PrintTo(const Status& status, std::ostream* os) {
  *os << static_cast<uint32_t>(status.GetStatusCode()) << ": " << status.ToString();  // whatever needed to print bar to os
}

inline void PrintTo(const StatusCode& status_code, std::ostream* os) {
  auto print = [&](auto s) {
    *os << s;
  };
  switch (status_code) {
    case ::netconf::StatusCode::OK:
      print("OK");
      break;
    case ::netconf::StatusCode::GENERIC_ERROR:
      print("GENERIC_ERROR");
      break;
    case ::netconf::StatusCode::SYSTEM_CALL:
      print("SYSTEM_CALL");
      break;
    case ::netconf::StatusCode::FILE_READ:
      print("FILE_READ");
      break;
    case ::netconf::StatusCode::FILE_WRITE:
      print("FILE_WRITE");
      break;
    case ::netconf::StatusCode::JSON_KEY_MISSING:
      print("JSON_KEY_MISSING");
      break;
    case ::netconf::StatusCode::JSON_CONVERT:
      print("JSON_CONVERT");
      break;
    case ::netconf::StatusCode::JSON_INCOMPLETE:
      print("JSON_INCOMPLETE");
      break;
    case ::netconf::StatusCode::IPV4_FORMAT:
      print("IPV4_FORMAT");
      break;
    case ::netconf::StatusCode::IP_CONFIG_READONLY:
      print("IP_CONFIG_READONLY");
      break;
    case ::netconf::StatusCode::IP_CONFIG_SOURCE:
      print("IP_CONFIG_SOURCE");
      break;
    case ::netconf::StatusCode::IP_INVALID:
      print("IP_INVALID");
      break;
    case ::netconf::StatusCode::IP_DISTRIBUTED_MULTIPLE_TIMES:
      print("IP_DISTRIBUTED_MULTIPLE_TIMES");
      break;
    case ::netconf::StatusCode::NETMASK_INVALID:
      print("NETMASK_INVALID");
      break;
    case ::netconf::StatusCode::NETWORK_CONFLICT:
      print("NETWORK_CONFLICT");
      break;
    case ::netconf::StatusCode::SYSTEM_EXECUTE:
      print("SYSTEM_EXECUTE");
      break;
    case ::netconf::StatusCode::ENTRY_DUPLICATE:
      print("ENTRY_DUPLICATE");
      break;
    case ::netconf::StatusCode::NAME_EMPTY:
      print("NAME_EMPTY");
      break;
    case ::netconf::StatusCode::INTERFACE_NOT_EXISTING:
      print("INTERFACE_NOT_EXISTING");
      break;
    case ::netconf::StatusCode::BRIDGE_NAME_INVALID:
      print("BRIDGE_NAME_INVALID");
      break;
    case ::netconf::StatusCode::DHCP_CLIENT_START:
      print("DHCP_CLIENT_START");
      break;
    case ::netconf::StatusCode::BOOTP_CLIENT_START:
      print("BOOTP_CLIENT_START");
      break;
    case ::netconf::StatusCode::PERSISTENCE_WRITE:
      print("PERSISTENCE_WRITE");
      break;
    case ::netconf::StatusCode::PERSISTENCE_READ:
      print("PERSISTENCE_READ");
      break;
    case ::netconf::StatusCode::RESTORE_CONTENT:
      print("RESTORE_CONTENT");
      break;
    case ::netconf::StatusCode::BACKUP_CONTENT_MISSING:
      print("BACKUP_CONTENT_MISSING");
      break;
    case ::netconf::StatusCode::BACKUP_CONTENT_INVALID:
      print("BACKUP_CONTENT_INVALID");
      break;
    case ::netconf::StatusCode::BACKUP_VERSION:
      print("BACKUP_VERSION");
      break;
    case ::netconf::StatusCode::DIP_NOT_AVAILABLE:
      print("DIP_NOT_AVAILABLE");
      break;
    case ::netconf::StatusCode::SET_IP:
      print("SET_IP");
      break;
    case ::netconf::StatusCode::SET_INTERFACE:
      print("SET_INTERFACE");
      break;
    case ::netconf::StatusCode::SET_INTERFACE_STATE:
      print("SET_INTERFACE_STATE");
      break;
    default:
      print("???" + Status(status_code).ToString());
  }
}

inline void PrintTo(const Interface& interface, std::ostream* os) {
  *os << "{" << ToString(interface.GetType()) << ':' << interface.GetName() << "}";
}

inline std::string IpSourceToString(::netconf::IPSource s) {
  switch (s) {
    case ::netconf::IPSource::BOOTP:
      return "BOOTP";
    case ::netconf::IPSource::DHCP:
      return "DHCP";
    case ::netconf::IPSource::STATIC:
      return "STATIC";
    case ::netconf::IPSource::NONE:
      return "NONE";
    case ::netconf::IPSource::UNKNOWN:
      return "UNKNOWN";
    default:
      return "???";
  }
}

inline void PrintTo(const IPConfig& config, std::ostream* os) {
  *os << "{Interface: " << config.interface_ << ", ";
  *os << "Source: " << IpSourceToString(config.source_) << ", ";
  *os << "IP: " << config.address_ << ", ";
  *os << "Mask: " << config.netmask_ << "}";
}

inline void PrintTo(const InterfaceConfig &interface_config, std::ostream* os) {
  *os << "{Device: " << interface_config.interface_.GetName() << ", ";
  *os << "State: " << static_cast<int>(interface_config.state_) << ", ";
  *os << "Autoneg: " << static_cast<int>(interface_config.autoneg_) << ", ";
  *os << "Speed: " << interface_config.speed_ << ", ";
  *os << "MacLearning" << static_cast<int>(interface_config.mac_learning_) << ", ";
  *os << "Duplex: " << static_cast<int>(interface_config.duplex_) << "}";
}

}  // namespace netconf

