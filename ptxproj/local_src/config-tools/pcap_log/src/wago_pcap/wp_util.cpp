//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     wp_util.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wp_util.hpp"

#include <algorithm>
#include <sstream>
#include <filesystem>
#include <curl/curl.h>

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

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
std::string UrlStringToString(std::string const& urlStr)
{
  std::string result;

  CURL* curlCtx = curl_easy_init();
  if(nullptr != curlCtx)
  {
    char* pDecoded = curl_easy_unescape(curlCtx, urlStr.c_str(), 0, nullptr);
    if(nullptr != pDecoded)
    {
      result = pDecoded;
      curl_free(pDecoded);
    }
    curl_easy_cleanup(curlCtx);
  }

  return result;
}

//---- End of source file ------------------------------------------------------

