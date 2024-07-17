//------------------------------------------------------------------------------
/// Copyright (c) 2024 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     error_handling.c
///
///  \brief    Error handling.
///
///  \author   MSc : WAGO GmbH & Co. KG
///  \author   MOe : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include "error_handling.hpp"

#include <syslog.h>

#include <cassert>
#include <map>

namespace configdnsmasq {
namespace {

::std::unique_ptr<configdnsmasq::logger> log;

const ::std::map<eStatusCode, ::std::string> status_codes = { // NOLINT(cert-err58-cpp)
    {MISSING_PARAMETER, "MISSING_PARAMETER"},
    {INVALID_PARAMETER, "INVALID_PARAMETER"},
    {FILE_OPEN_ERROR, "FILE_OPEN_ERROR"},
    {FILE_READ_ERROR, "FILE_READ_ERROR"},
    {FILE_CLOSE_ERROR, "FILE_CLOSE_ERROR"},
    {NOT_FOUND, "NOT_FOUND"},
    {SYSTEM_CALL_ERROR, "SYSTEM_CALL_ERROR"},
    {CONFIG_FILE_INCONSISTENT, "CONFIG_FILE_INCONSISTENT"},
    {TIMEOUT, "TIMEOUT"},
    {FILE_WRITE_ERROR, "FILE_WRITE_ERROR"},
    {NARROW_SPACE_WARNING, "NARROW_SPACE_WARNING"},
    {NOT_ENOUGH_SPACE_ERROR, "NOT_ENOUGH_SPACE_ERROR"},
};
}

void erh_init(const ::std::string &prgname) {
  log = ::std::make_unique<configdnsmasq::logger>(prgname);
}

void erh_set_error(eStatusCode code, const std::string &message, bool exit_on_error) {
  erh_log(::std::string{"exit_on_error: "} + (exit_on_error ? "true" : "false"));

  auto m = (boost::format("%s: " + message) % log->identifier_).str();

  ct_liblog_setLastError(m.c_str());

  auto status_str = status_codes.count(code) == 1 ? status_codes.at(code) : "";

  fprintf(stderr, "%s: %s; code: %d\n", status_str.c_str(), m.c_str(), code);
  syslog(LOG_DEBUG, "%s: %s; code: %d\n", status_str.c_str(), message.c_str(), code);

  if (exit_on_error) {
    exit(code);
  }
}

void erh_assert(bool condition, eStatusCode code, const std::string &message, bool exit_on_error) {
  if (!condition) {
    erh_set_error(code, message, exit_on_error);
  }
}

void erh_assert(bool condition, eStatusCode code, const boost::format &message, bool exit_on_error) {
  erh_assert(condition, code, message.str(), exit_on_error);
}

void erh_log(const std::string &message) {
  syslog(LOG_DEBUG, "%s\n", message.c_str());
}

}  // namespace configdnsmasq
