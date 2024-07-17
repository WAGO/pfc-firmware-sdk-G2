// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <exception>
#include <map>
#include <utility>
#include <vector>
#include <string>

namespace netconf {

enum class StatusCode : ::std::uint32_t {
  OK = 0,
  GENERIC_ERROR,
  SYSTEM_CALL,
  FILE_READ,
  FILE_WRITE,
  JSON_KEY_MISSING,
  JSON_CONVERT,
  JSON_INCOMPLETE,
  IPV4_FORMAT,
  IP_CONFIG_READONLY,
  IP_CONFIG_SOURCE,
  IP_INVALID,
  IP_DISTRIBUTED_MULTIPLE_TIMES,
  NETMASK_INVALID,
  NETWORK_CONFLICT,
  SYSTEM_EXECUTE,
  ENTRY_DUPLICATE,
  NAME_EMPTY,
  INTERFACE_NOT_EXISTING,
  BRIDGE_NAME_INVALID,
  DHCP_CLIENT_START,
  BOOTP_CLIENT_START,
  PERSISTENCE_WRITE,
  PERSISTENCE_READ,
  RESTORE_CONTENT,
  BACKUP_CONTENT_MISSING,
  BACKUP_CONTENT_INVALID,
  BACKUP_VERSION,
  DIP_NOT_AVAILABLE,
  SET_IP,
  SET_INTERFACE,
  SET_INTERFACE_STATE,
  DBUS,
  LINK_MODE_NOT_SUPPORTED,
  AUTONEGOTIATION_NOT_SUPPORTED,
  DEVICE_TYPE_NOT_SOPPORTED,
  INTERFACE_VALIDATION,
  REDUNDANCY,
  LINK_MODE_NOT_CONFIGURABLE,
};

class Status {
 public:
  using Parameters = ::std::vector<::std::string>;
  Status()
      : code_ { StatusCode::OK } {
  }

#if __cpp_fold_expressions
  template <typename... Args>
  explicit Status(StatusCode code, Args&&... args) : code_{code} {
    // @formatter:off
    (parameter_.emplace_back(::std::forward<decltype(args)>(args)), ...);
    // @formatter:on
  }
#else
  explicit Status(StatusCode code)
      : code_ { code } {
  }
  Status(StatusCode code, ::std::string &&arg1)
      : code_ { code } {
    parameter_.emplace_back(::std::forward<::std::string>(arg1));
  }
#endif

#if __cpp_fold_expressions
  template <typename... Args>
  void Set(StatusCode code, Args&&... args) {
    code_ = code;
    parameter_.clear();
    // @formatter:off
    (parameter_.emplace_back(::std::forward<decltype(args)>(args)), ...);
    // @formatter:on
  }
#endif

  void Set(StatusCode code, Parameters &&p);
  void Set(StatusCode code);
  Status& Append(const std::string &parameter);
  Status& Append(const std::string &&parameter);
  bool IsOk() const noexcept;
  bool IsNotOk() const noexcept;
  bool operator==(Status const &other) const noexcept;
  ::std::string ToString() const;
  StatusCode GetStatusCode() const;
  const Parameters& GetParameter() const;

  static Status MakeSystemCallError(const ::std::string& syscall, const ::std::string& function, int line);

 private:
  StatusCode code_;
  Parameters parameter_;

  bool ReplaceFirstOccurence(std::string &str, const std::string &from, const std::string &to) const;
};

#define MAKE_SYSTEMCALL_ERROR(syscall) Status::MakeSystemCallError(syscall, &__func__[0], __LINE__)

}  // namespace netconf
