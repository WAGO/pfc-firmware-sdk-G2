//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     wp_debug.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wp_debug.hpp"
#include "wp_util.hpp"

#include <chrono>
#include <cstdarg>  // va_xxx()
#include <fstream>
#include <filesystem>
#include <vector>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------
bool DebugPrintIsOn = false;

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
std::string format(const char * fmt, ...)
{
  va_list args;
  va_list args_copy;
  va_copy(args_copy, args);
  va_start(args, fmt);
  int formatted_length = std::vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  if (formatted_length <= 0)
  {
    return "";
  }

  std::vector<char> formatted(static_cast<size_t>(formatted_length) + 1);
  va_start(args_copy, fmt);
  std::vsnprintf(&formatted[0], static_cast<size_t>(formatted_length) + 1, fmt, args_copy);
  va_end(args_copy);

  return { formatted.data(), formatted.size() };
}

void LogToFile(const std::string & file_path, const std::string & msg)
{
  auto now = std::chrono::system_clock::now();
  auto datetime = std::chrono::system_clock::to_time_t(now);
  bool file_exists = std::filesystem::exists(file_path);

  std::ofstream ofs{file_path, std::ofstream::app};
  if (!file_exists)
  {
    std::filesystem::permissions(file_path, get_allowed_permissions());
  }

  ofs << std::put_time(std::localtime(&datetime), "%F %T %Z") << ": " << msg;
}

void Debug_SetEnable() {
  DebugPrintIsOn = true;
}

void Debug_SetDisable() {
  DebugPrintIsOn = false;
}

bool Debug_GetState() {
  return DebugPrintIsOn;
}
//---- End of source file ------------------------------------------------------

