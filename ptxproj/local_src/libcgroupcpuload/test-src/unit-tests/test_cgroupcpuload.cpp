//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_cgroupcpuload.cpp
///
///  \brief    Test cgroupcpuload functions.
///
///  \author   JMue: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <fff.h>
#include "fakes/fake_unistd.h"
#include "fakes/fake_stdlib.h"
#include "fakes/fake_time.h"
#include "fakes/fake_fcntl.h"


#include "cgroupcpuload.h"

#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

using namespace testing;

DEFINE_FFF_GLOBALS;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------
#define FFF_FAKES_LIST(FAKE) \
    FAKE(open)  \
    FAKE(close) \
    FAKE(lseek) \
    FAKE(read)  \
    FAKE(free)  \
    FAKE(clock_gettime)

static const char * cgrouprts = "rts";
static uint64_t timecounter_base = 1000;
static uint64_t timecounter_inc  = 0;


//------------------------------------------------------------------------------
// custom fakes
//------------------------------------------------------------------------------
static void reset_fakes()
{
    FFF_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
}

static int open_fake_return;
static int open_custom_fake(const char * path, int flags, va_list mode)
{
  open_fake_return = 42;
  return open_fake_return;
}

static ssize_t read_fake_return;
static ssize_t read_custom_fake(int file, void * buffer, size_t maxsize)
{
  sprintf((char *) buffer,"%lli %lli\n", timecounter_base * timecounter_inc,timecounter_base * timecounter_inc);

  read_fake_return =  (ssize_t) strlen((char *) buffer);
  timecounter_inc++;
  return read_fake_return;
}

static int clock_gettime_fake_return;
static int clock_gettime_custom_fake(clockid_t clockid, timespec * timerval)
{
  timerval->tv_sec  = 0;
  timerval->tv_nsec = timecounter_inc * 2 * timecounter_base;

  clock_gettime_fake_return = 0;
  return clock_gettime_fake_return;
}


//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------
class test_cgroupcpuload : public::testing::Test
{
  protected:
    void SetUp() override
    {
        reset_fakes();
    }

    void TearDown() override {}
};

TEST_F(test_cgroupcpuload, dummy_test)
{
    cgroupcpuload * cgroupcpuload;
    uint32_t load ;

    open_fake.custom_fake          = open_custom_fake;
    read_fake.custom_fake          = read_custom_fake;
    clock_gettime_fake.custom_fake = clock_gettime_custom_fake;

    cgroupcpuload = cgroupcpuload_init(cgrouprts);

    load = cgroupcpuload_get_load(cgroupcpuload);

    EXPECT_EQ(load, 50);

    load = cgroupcpuload_get_load(cgroupcpuload);

    EXPECT_EQ(load, 50);

    cgroupcpuload_destroy(cgroupcpuload);

    ASSERT_EQ(open_fake.call_count, 1);
    ASSERT_EQ(close_fake.call_count, 1);
    ASSERT_EQ(free_fake.call_count, 4);

    // calls in init
    ASSERT_EQ(fff.call_history[0], (void *)open);
    ASSERT_EQ(fff.call_history[1], (void *)lseek);
    ASSERT_EQ(fff.call_history[2], (void *)read);

    /*
    // calls in destroy
    ASSERT_EQ(fff.call_history[3], (void *)close);
    ASSERT_EQ(fff.call_history[4], (void *)free);
    ASSERT_EQ(fff.call_history[5], (void *)free);
    ASSERT_EQ(fff.call_history[6], (void *)free);
    */
}
