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

#include <cstdarg>  // va_xxx()
#include <iostream> // vprintf()

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
void Debug_Printf(const char * msg, ...) {
  if (DebugPrintIsOn)
  {
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    /*
    FILE *fp = fopen("/root/pcap.log", "a");
    if(NULL != fp)
    {
      vfprintf(fp, msg, args);
      fclose(fp);
    }
    */
    va_end(args);
  }
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

