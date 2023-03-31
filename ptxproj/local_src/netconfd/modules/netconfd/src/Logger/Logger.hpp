// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define DBOOST_LOG_DYN_LINK
//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <memory>
#include <sstream>

#include "Status.hpp"

namespace netconf {

enum class LogSink { TERMINAL, SYSLOG };

/*
 * What is logged?
 * debug  --> logging: all
 * info   --> logging: info, warning, error
 * warning--> logging: warning, error
 * error  --> logging: error
 */
enum class LogLevel { debug, info, warning, error };

void SetLogSink(LogSink sink);
void SetLogLevel(LogLevel level);

void LogWarning(::std::string const& message);
void LogInfo(::std::string const& message);
void LogError(::std::string const& message);
void LogDebug(::std::string const& message);
void LogDebug(::std::string const& func, ::std::string const& message);

#define LOG_STATUS(status)                                               \
  do {                                                                   \
    if ((status).IsNotOk()) LOG_DEBUG("Status: " + (status).ToString()); \
  } while (0)

//TODO(Team): find a nicer solution to fix warning. -> macro argument should be enclosed in parentheses [bugprone-macro-parentheses]
#define LOG_DEBUG(message){                                        \
    ::std::stringstream ss;                                       \
    ::std::string func{&__func__[0]};                             \
    ::std::string file{&__FILE__[0]};                             \
    int line{__LINE__};                                           \
    ss << file << ":" << line << "-" << func << " message:" << message; LogDebug(ss.str()); } //NOLINT

LogLevel LogLevelFromString(::std::string const& level);

}  // namespace netconf
