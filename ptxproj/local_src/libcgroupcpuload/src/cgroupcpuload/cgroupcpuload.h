//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     cgroupcpuload.h
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef _CGROUPCPULOAD_H_
#define _CGROUPCPULOAD_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <inttypes.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

struct cgroupcpuload
{
  uint64_t   timestamp;
  uint64_t * runtime_core;
  uint64_t * runtime_core_old;
  uint32_t * load_core;
  uint16_t   cpu_cores;
  int        cpuacct_usage_fd;
};

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


  struct cgroupcpuload * cgroupcpuload_init(char const * cgroup_path);

  void cgroupcpuload_destroy(struct cgroupcpuload * cgroupcpuload);

  //
  //  uint32_t cgroupcpuload_get_load(struct cgroupcpuload * cgroupcpuload);
  //
  //  The result value is in percent. On Multicore systems the return value is modified
  //  to represent a significant system load.
  //  To represent this the highest load of all cores is evaluated
  //
  //  Three levels of system load are distinguished:
  //
  //  1: all core loads are below 50%    > the average load is resulted
  //  2: highest core load is above 50 % > then medium value between the highest and the average value is the result
  //  3: highest core load is above 75%  > the highest core load is resulted
  //

  uint32_t cgroupcpuload_get_load(struct cgroupcpuload * cgroupcpuload);


  #ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------



#endif /* _CGROUPCPULOAD_H_ */



//---- End of source file ------------------------------------------------------
