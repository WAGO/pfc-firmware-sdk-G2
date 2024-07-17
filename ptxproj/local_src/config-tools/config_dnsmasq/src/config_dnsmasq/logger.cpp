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
///
///  \brief    Write logging data.
///
///  \author   MSc, WAGO GmbH & Co. KG.
///  \author   MOe, WAGO GmbH & Co. KG.
//------------------------------------------------------------------------------

#include "logger.hpp"

#include <syslog.h>

namespace configdnsmasq
{

  logger::logger(::std::string const& identifier)
  {
    identifier_ = identifier;
    openlog(identifier_.c_str(), LOG_CONS | LOG_NDELAY, LOG_SYSLOG);
  }

  logger::~logger()
  {
    closelog();
  }

} /* namespace configdnsmasq */
