//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     fake_unistd.h
///
///  \brief    Fake for fcntl
///
///  \author   cwue : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef FAKE_FCNTL_H
#define FAKE_FCNTL_H

#include <fcntl.h>
#include <fff.h>

DECLARE_FAKE_VALUE_FUNC_VARARG(int, open, const char *, int, ...);

#endif
