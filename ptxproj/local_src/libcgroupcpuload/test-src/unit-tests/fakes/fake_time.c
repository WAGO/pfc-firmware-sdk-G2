//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     fake_time.c
///
///  \brief    Fake for time
///
///  \author   WDt : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include "fake_time.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"

DEFINE_FAKE_VALUE_FUNC2(int, clock_gettime, clockid_t, struct timespec*);

#pragma GCC diagnostic pop
