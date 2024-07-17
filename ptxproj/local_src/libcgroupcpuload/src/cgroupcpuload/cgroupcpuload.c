//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     cgroupcpuload.c
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "cgroupcpuload.h"
#include "log.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>


//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define NSEC_PER_SEC        ((uint64_t)(1000 * 1000 * 1000))
#define TIMESPEC_TO_U64(ts) ((uint64_t)(ts).tv_nsec+(uint64_t)(ts).tv_sec*NSEC_PER_SEC)
#define DIV_ROUND_UP(n,d)   ((((n) + (d)) - 1U)/ (d))

#define SINGLECORE_THRESHOLD2 75U  // show value of most loaded core instead of average of all cores
#define SINGLECORE_THRESHOLD1 50U  // show mix of most loaded core and average of all cores

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
static void cgroupcpuload_read_runtime(struct cgroupcpuload * cgroupcpuload);

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------
//#define LOG_LEVEL LogLevel_Debug

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
DLL_DECL struct cgroupcpuload * cgroupcpuload_init(char const * const cgroup_path)
{
  char   path[256];
  char   buffer[256];
  struct timespec timespec;
  struct cgroupcpuload *cgroupcpuload = calloc(1, sizeof(*cgroupcpuload));
  int    fd;

  snprintf(path, sizeof(path), "/sys/fs/cgroup/cpuacct/%s/cpuacct.usage_percpu", cgroup_path);
  fd = open(path, O_RDONLY | O_NOATIME);

  if (fd == -1)
  {
    ERROR("open (%s, ..) failed: %s", path, strerror(errno));
    return NULL;
  }

  cgroupcpuload->cpuacct_usage_fd = fd;
  cgroupcpuload->cpu_cores        = 1;
  cgroupcpuload->runtime_core     = NULL;
  cgroupcpuload->runtime_core_old = NULL;
  cgroupcpuload->load_core        = NULL;

  lseek(cgroupcpuload->cpuacct_usage_fd, 0, SEEK_SET);
  ssize_t const length = read(cgroupcpuload->cpuacct_usage_fd, buffer, sizeof(buffer) - 1U);

  if (length > 0)
  {
	  buffer[length-1] = '\0';

	  for (int i = 0; i < length; i++)
	  {
	    if ((buffer[i] == ' ') && (buffer[i+1] > ' '))
	    {
	      cgroupcpuload->cpu_cores++;
	    }
    }

    cgroupcpuload->runtime_core     = calloc(1, cgroupcpuload->cpu_cores * sizeof(uint64_t));
    cgroupcpuload->runtime_core_old = calloc(1, cgroupcpuload->cpu_cores * sizeof(uint64_t));
    cgroupcpuload->load_core        = calloc(1, cgroupcpuload->cpu_cores * sizeof(uint32_t));

    if ((cgroupcpuload->runtime_core == NULL) || (cgroupcpuload->load_core == NULL) || (cgroupcpuload->runtime_core_old == NULL))
    {
      ERROR("calloc failed: %s", strerror(errno));
      return NULL;
    }
  }

  cgroupcpuload_get_load(cgroupcpuload);

  if (clock_gettime(CLOCK_MONOTONIC, &timespec) != 0)
  {
    return NULL;
  }

  cgroupcpuload->timestamp = TIMESPEC_TO_U64(timespec);

  return cgroupcpuload;
}

//------------------------------------------------------------------------------
DLL_DECL void cgroupcpuload_destroy(struct cgroupcpuload * cgroupcpuload)
{

  if (cgroupcpuload != NULL)
  {
     close(cgroupcpuload->cpuacct_usage_fd);
    if (cgroupcpuload->runtime_core != NULL)
    {
      free(cgroupcpuload->runtime_core);
    }
    if (cgroupcpuload->runtime_core_old != NULL)
    {
      free(cgroupcpuload->runtime_core_old);
    }
    if (cgroupcpuload->load_core != NULL)
    {
      free(cgroupcpuload->load_core);
    }
    free(cgroupcpuload);
  }
}


//------------------------------------------------------------------------------
DLL_DECL uint32_t cgroupcpuload_get_load(struct cgroupcpuload * cgroupcpuload)
{
  struct timespec timespec;
  uint32_t load       = 0U;
  uint32_t maxload    = 0U;
  uint64_t timestamp;
  uint64_t delta_time;
  uint64_t delta_runtime;

  if ((cgroupcpuload == NULL) || (cgroupcpuload->runtime_core == NULL) || (cgroupcpuload->runtime_core_old == NULL))
  {
    return 0U;
  }

  cgroupcpuload_read_runtime(cgroupcpuload);

  if (clock_gettime(CLOCK_MONOTONIC, &timespec) != 0)
  {
    return 0U;
  }

  timestamp   = TIMESPEC_TO_U64(timespec);
  delta_time  = (timestamp - cgroupcpuload->timestamp);

  DBG("timestamp: %" PRIu64 " old_timestamp: %" PRIu64 " delta_time: %" PRIu64,
      timestamp, cgroupcpuload->timestamp, delta_time);

  if(delta_time != 0U)
  {
    cgroupcpuload->timestamp = timestamp;

    for (int core = 0; core < cgroupcpuload->cpu_cores; core++)
    {
      delta_runtime = (cgroupcpuload->runtime_core[core] - cgroupcpuload->runtime_core_old[core]);
      cgroupcpuload->load_core[core] = (uint32_t) DIV_ROUND_UP(100U * delta_runtime, delta_time);
      load += cgroupcpuload->load_core[core];
      DBG("core %i delta_runtime %" PRIu64 " load: %" PRIu32 "%%", core + 1, delta_runtime, cgroupcpuload->load_core[core]);
      if (cgroupcpuload->load_core[core] > maxload)
      {
        maxload = cgroupcpuload->load_core[core];
      }
    }
  }

  load = load / cgroupcpuload->cpu_cores;

  if (maxload > SINGLECORE_THRESHOLD2)
  {
    load = maxload;
  }
  else
  {
    if (maxload > SINGLECORE_THRESHOLD1)
    {
      load = (load + maxload) / 2;
    }
  }

  if (load > 100U)
  {
    load = 100U;
  }

  INFO("cpu load: %" PRIu32 "%%", load);

  return load;
}


//------------------------------------------------------------------------------
static void cgroupcpuload_read_runtime(struct cgroupcpuload * cgroupcpuload)
{
  char buffer[256];
  int  core   = 0;
  int  oldpos = 0;
  uint64_t result = 0;
  const char * const uint64_format = "%" PRIu64 "\n";

  lseek(cgroupcpuload->cpuacct_usage_fd, 0, SEEK_SET);
  ssize_t const length = read(cgroupcpuload->cpuacct_usage_fd, buffer, sizeof(buffer) - 1U);
  if (length > 0)
  {
    buffer[length] = '\0';
    DBG("buffer %s", buffer);

    for (int pos = 0; pos < length; pos++)
    {
      if (((buffer[pos] == ' ') && (buffer[pos+1] > ' ')) || (pos == (length -1)))
      {
        DBG("core %i runtime buffer %s", core + 1, &buffer[oldpos]);
        buffer[pos] = '\n';

        if (sscanf(&buffer[oldpos], uint64_format, &result) > 0)
        {
          if (core < cgroupcpuload->cpu_cores)
          {
            cgroupcpuload->runtime_core_old[core] = cgroupcpuload->runtime_core[core];
            cgroupcpuload->runtime_core[core]     = result;
            DBG("core %i runtime %" PRIu64, core + 1, cgroupcpuload->runtime_core[core]);
            core++;
          }
        }
        oldpos = pos + 1;
      }
    }
  }
}



//---- End of source file ------------------------------------------------------
