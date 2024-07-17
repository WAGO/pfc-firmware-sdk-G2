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
///  \brief    Fake for unistd
///
///  \author   JMue : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef FAKE_UNISTD_H
#define FAKE_UNISTD_H

#include <unistd.h>
#include <fff.h>

DECLARE_FAKE_VALUE_FUNC1(int, close, int);
DECLARE_FAKE_VALUE_FUNC3(long int,lseek,int,long int,int);
DECLARE_FAKE_VALUE_FUNC3(ssize_t,read,int,void *,size_t);


#endif
