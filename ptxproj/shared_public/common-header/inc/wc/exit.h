//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project common-header (PTXdist package libcommonheader).
//
// Copyright (c) 2022 WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     exit.h
///
///  \brief    Helper constants for usable program exit codes. Also refer to
///            https://tldp.org/LDP/abs/html/exitcodes.html for details.
///
///  \author   MaHe: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_WC_EXIT_H_
#define INC_WC_EXIT_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/assertion.h"
#include "stdlib.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

/// \def WC_EXIT_SUCCESS
/// Indicates success in \def wc_exit(exit_code) resp. \ref exit(exit_code).
/// \see https://tldp.org/LDP/abs/html/exitcodes.html
#define WC_EXIT_SUCCESS           0

/// \def WC_EXIT_ERROR_GENERAL
/// Indicates a general error (impermissible operation)
/// in \def wc_exit(exit_code) resp. \ref exit(exit_code).
/// \see https://tldp.org/LDP/abs/html/exitcodes.html
#define WC_EXIT_ERROR_GENERAL     1

/// \def WC_EXIT_ERROR_FIRST
/// Lowest allowed value which indicates an application specific error
/// in \def wc_exit(exit_code) resp. \ref exit(exit_code).
/// \see https://tldp.org/LDP/abs/html/exitcodes.html
#define WC_EXIT_ERROR_FIRST      79

/// \def WC_EXIT_ERROR_LAST
/// Highest allowed value which indicates an application specific error
/// in \def wc_exit(exit_code) resp. \ref exit(exit_code).
/// \see https://tldp.org/LDP/abs/html/exitcodes.html
#define WC_EXIT_ERROR_LAST      113

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// Calls \ref exit(status), after applying a debug assertion to the exit
/// code to be either \ref WC_EXIT_SUCCESS or in the valid range of error codes
/// as defined by \ref WC_EXIT_ERROR_FIRST and \ref WC_EXIT_ERROR_LAST.
///
/// \see https://tldp.org/LDP/abs/html/exitcodes.html
///
/// \param status
///   The status to be used for exit.
  //------------------------------------------------------------------------------
static inline void wc_exit(int status);

#ifdef __cplusplus
}
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
static inline void wc_exit(int status)
{
  WC_ASSERT(   ( status == WC_EXIT_SUCCESS)
            || ( status == WC_EXIT_ERROR_GENERAL)
            || (   (status >= WC_EXIT_ERROR_FIRST)
                && (status <= WC_EXIT_ERROR_LAST)));
  exit(status);
}


#endif /* INC_WC_EXIT_H_ */
//---- End of source file ------------------------------------------------------

