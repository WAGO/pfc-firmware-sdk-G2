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

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

void Debug_Printf(const char * msg, ...) __attribute__ ((format (printf, 1, 2)));
void Debug_SetEnable();
void Debug_SetDisable();
bool Debug_GetState();

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

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

