//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     fake_stdlib.h
///
///  \brief    Fake for stdlib
///
///  \author   JMue : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef FAKE_STDLIB_H
#define FAKE_STDLIB_H

#include <stdlib.h>
#include <fff.h>

DECLARE_FAKE_VOID_FUNC1(free, void*);

#endif