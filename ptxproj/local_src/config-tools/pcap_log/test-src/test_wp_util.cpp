//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_wp_util.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>

#include "wp_debug.hpp"
#include "wp_util.hpp"

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
using namespace testing;

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
class UTIL_func_UrlStringToString : public ::testing::Test {
  protected:
    std::string url = "";
    std::string result = "";

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(UTIL_func_UrlStringToString, Space)
{
  url = "%20";
  result = UrlStringToString(url);
  ASSERT_STREQ(" ", result.c_str());
}

TEST_F(UTIL_func_UrlStringToString, Quote)
{
  url = "%22";
  result = UrlStringToString(url);
  ASSERT_STREQ("\"", result.c_str());
}

//---- End of source file ------------------------------------------------------

