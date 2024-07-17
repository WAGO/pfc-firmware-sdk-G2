//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     fake_timw.h
///
///  \brief    Fake for time
///
///  \author   cwue : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef FAKE_TIME_H
#define FAKE_TIME_H

#include <time.h>
#include <fff.h>

DECLARE_FAKE_VALUE_FUNC2(int, clock_gettime, clockid_t, struct timespec*);

#endif
