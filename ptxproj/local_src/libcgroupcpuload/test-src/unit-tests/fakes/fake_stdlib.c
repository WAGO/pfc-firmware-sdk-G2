//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     fake_stdlib.c
///
///  \brief    Fake for stdlib
///
///  \author   WDt : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include "fake_stdlib.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"

DEFINE_FAKE_VOID_FUNC1(free, void*);

#pragma GCC diagnostic pop