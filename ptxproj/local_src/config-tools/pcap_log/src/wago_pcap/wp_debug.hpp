//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     wp_debug.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_WAGO_PCAP_WP_DEBUG_HPP_
#define SRC_WAGO_PCAP_WP_DEBUG_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
std::string format(const char * fmt, ...) __attribute__ ((format (printf, 1, 2)));
void LogToFile(const std::string & file_path, const std::string & msg);

void Debug_SetEnable();
void Debug_SetDisable();
bool Debug_GetState();

template<typename ...Args> void Debug_Printf(const std::string & msg, Args... args)
{
  if (Debug_GetState())
  {
    std::cout << format(msg.c_str(), args...);
  }
}

template<typename ...Args> void Debug_PrintAndLogToFile(
  const std::string & file_path,
  const std::string & msg,
  Args... args)
{
  Debug_Printf(msg, args...);
  LogToFile(file_path, format(msg.c_str(), args...));
}

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* SRC_WAGO_PCAP_WP_DEBUG_HPP_ */
//---- End of source file ------------------------------------------------------

