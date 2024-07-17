//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     fake_unistd.c
///
///  \brief    Fake for unistd
///
///  \author   cwue : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include "fake_unistd.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"

DEFINE_FAKE_VALUE_FUNC1(int, close, int);
DEFINE_FAKE_VALUE_FUNC3(long int, lseek, int, long int, int);
DEFINE_FAKE_VALUE_FUNC3(ssize_t, read, int, void *,size_t);


#pragma GCC diagnostic pop
