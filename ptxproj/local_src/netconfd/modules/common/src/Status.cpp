// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Status.hpp"

namespace netconf {

namespace {

std::string GetTextForCode(StatusCode c) {
  // @formatter:off
  // clang-format off

static const ::std::map<StatusCode, ::std::string> status_text = {
    { StatusCode::OK, ::std::string{} },
    { StatusCode::FILE_READ,                     "Failed to read file %s" },
    { StatusCode::FILE_WRITE,                    "Failed to write file %s" },
    { StatusCode::SYSTEM_CALL,                   "System call error: %s" },
    { StatusCode::JSON_KEY_MISSING,              "Missing JSON key %s" },
    { StatusCode::JSON_CONVERT,                  "JSON convert error: %s" },
    { StatusCode::JSON_INCOMPLETE,               "JSON content is incomplete" },
    { StatusCode::IPV4_FORMAT,                   "IPv4 format incorrect: %s" },
    { StatusCode::IP_CONFIG_READONLY,            "IP config of %s is readonly"},
    { StatusCode::IP_CONFIG_SOURCE,              "IP source of %s is invalid"},
    { StatusCode::IP_DISTRIBUTED_MULTIPLE_TIMES, "IP address %s is distributed multiple times"},
    { StatusCode::IP_INVALID,                    "Interface %s ip %s is invalid. %s"},
    { StatusCode::NETMASK_INVALID,               "Interface %s netmask %s is invalid. %s"},
    { StatusCode::NETWORK_CONFLICT,              "Network for interface %s and %s is in the same range"},
    { StatusCode::SYSTEM_EXECUTE,                "System execution failed: %s" },
    { StatusCode::ENTRY_DUPLICATE,               "Entry is duplicated: %s"},
    { StatusCode::INTERFACE_NOT_EXISTING,        "Interface does not exist: %s"},
    { StatusCode::NAME_EMPTY,                    "Name is empty"},
    { StatusCode::DHCP_CLIENT_START,             "Starting dhcp client for %s failed"},
    { StatusCode::BOOTP_CLIENT_START,            "Starting bootp client for %s failed"},
    { StatusCode::SET_IP,                        "Set IP address %s netmask %s for interface %s failed"},
    { StatusCode::SET_INTERFACE,                 "Set interface configuration for %s failed"},
    { StatusCode::SET_INTERFACE_STATE,           "Set interface state for %s failed"},
    { StatusCode::PERSISTENCE_WRITE,             "Failed to write data to persistence file"},
    { StatusCode::PERSISTENCE_READ,              "Failed to read data from persistence file"},
    { StatusCode::DIP_NOT_AVAILABLE,             "DIP switch based IP configuration not available"},
    { StatusCode::BACKUP_CONTENT_MISSING,        "Backup file is missing content for %s"},
    { StatusCode::BACKUP_CONTENT_INVALID,        "Backup file having invalid content for %s"},
    { StatusCode::BACKUP_VERSION,                "Unknown backup data version, need %s have %s"},
    { StatusCode::RESTORE_CONTENT,               "Failed to restore content from %s"},
    { StatusCode::DBUS,                          "DBUS error occurred: %s: %s"},
    { StatusCode::GENERIC_ERROR,                 "Error: %s" },
    { StatusCode::LINK_MODE_NOT_SUPPORTED,       "Interface %s does not support (speed %s/duplex %s)" },
    { StatusCode::AUTONEGOTIATION_NOT_SUPPORTED, "Interface %s does not support Autonegotiation" },
    { StatusCode::DEVICE_TYPE_NOT_SOPPORTED,     "Device type %s not supported" },
    { StatusCode::INTERFACE_VALIDATION,          "Interface validation error: %s" },
    { StatusCode::REDUNDANCY,                    "Redundancy error: %s" },
    { StatusCode::LINK_MODE_NOT_CONFIGURABLE,    "Interface %s does not allow to configure (speed %s/duplex %s)" },

};
  // clang-format on
  // @formatter:on
  return status_text.at(c);
}

}  // namespace


bool Status::operator==(Status const& other) const noexcept {
  return code_ == other.code_;
}

void Status::Set(StatusCode code, Parameters &&p) {
  code_ = code;
  parameter_ = ::std::forward<Parameters>(p);
}

void Status::Set(StatusCode code){
  code_ = code;
}

Status& Status::Append(const std::string &parameter) {
  parameter_.push_back(parameter);
  return *this;
}

Status& Status::Append(const std::string &&parameter) {
  parameter_.push_back(parameter);
  return *this;
}

bool Status::IsOk() const noexcept {
  return code_ == StatusCode::OK;
}

bool Status::IsNotOk() const noexcept {
  return !IsOk();
}

::std::string Status::ToString() const {
  ::std::string error = GetTextForCode(code_);
  Parameters append_at_end;

  if (not parameter_.empty()) {
    for (auto &p : parameter_) {
      if (not ReplaceFirstOccurence(error, "%s", p)) {
        append_at_end.push_back(p);
      }
    }
  }

  if (not append_at_end.empty()) {
    std::for_each(append_at_end.begin(), append_at_end.end(), [&error](auto &s) {
      error.append(": ").append(s);
    });
  }

  while (ReplaceFirstOccurence(error, "%s", "")){
    ;
  }

  return error;
}

StatusCode Status::GetStatusCode() const {
  return code_;
}

const Status::Parameters& Status::GetParameter() const {
  return parameter_;
}

bool Status::ReplaceFirstOccurence(std::string& str, const std::string& from, const std::string& to) const {
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos) {
    return false;
  }
  str.replace(start_pos, from.length(), to);
  return true;
}

Status Status::MakeSystemCallError(const ::std::string& syscall, const ::std::string& function, int line) {
  return Status {StatusCode::SYSTEM_CALL,
                function + ":" + ::std::to_string(line) + " " + syscall + ": " + ::std::strerror(errno)};
}

}
