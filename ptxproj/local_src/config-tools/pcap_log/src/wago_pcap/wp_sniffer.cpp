//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     wp_sniffer.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <algorithm>

#include "wp_sniffer.hpp"
#include "wp_debug.hpp"
#include "wp_util.hpp"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
PcapSniffer::~PcapSniffer() {
  Close();
}

//------------------------------------------------------------------------------
void PcapSniffer::Close() {
  BreakLoop();
  CloseDump();
  CloseLive();
}

//------------------------------------------------------------------------------
void PcapSniffer::OpenDump(const std::filesystem::path & path) {
  savefile = path;
  if(nullptr != pHandle)
  {
    pDumper = pcap_dump_open(pHandle,
                             savefile.c_str());
    Debug_Printf("savefile=%s \n", savefile.c_str());
    if(nullptr == pDumper)
    {
      throw std::invalid_argument(pcap_geterr(pHandle));
    }

    // dump gets created with default permissions, fix them
    std::filesystem::permissions(path, get_allowed_permissions());
    set_owner_group_webserver(path);
  }
}

//------------------------------------------------------------------------------
bool PcapSniffer::CompileFilter(const std::string & filter, std::uint16_t snapLength)
{
  bool result = false;
  bpf_program prog = {0, nullptr};
  pcap_t * tmp_handle = pcap_open_dead(DLT_EN10MB, snapLength);

  if (tmp_handle != nullptr)
  {
    if (pcap_compile(tmp_handle, &prog, filter.c_str(), 0, PCAP_NETMASK_UNKNOWN) == 0)
    {
      result = true;
    }

    pcap_close(tmp_handle);
  }

  return result;
}

void PcapSniffer::SetFilter(const std::string & filter) {
  if((nullptr != pHandle) && (!filter.empty()))
  {
    bpf_program prog = {0, nullptr};

    if(0 == pcap_compile(pHandle, &prog, filter.c_str(), optimize, mask)) {
      if(0 != pcap_setfilter(pHandle, &prog)) {
        pcap_perror(pHandle, &errbuf[0]);
        throw std::invalid_argument(&errbuf[0]);
      }
    }
    else {
      throw std::invalid_argument(&errbuf[0]);
    }
  }
}

//------------------------------------------------------------------------------
void PcapSniffer::OpenLive(const std::string &devName, std::uint16_t snapLength) {
  pHandle = pcap_open_live(devName.c_str(),
                           snapLength,
                           PCAP_OPENFLAG_PROMISCUOUS,
                           bufferTimeout,
                           &errbuf[0]);
  if(nullptr == pHandle)
  {
    throw std::invalid_argument(&errbuf[0]);
  }

  if(0 != pcap_lookupnet(devName.c_str(), &net, &mask, &errbuf[0]))
  {
    throw std::invalid_argument(&errbuf[0]);
  }

  if(Debug_GetState()) {
    char networkAddr[INET_ADDRSTRLEN] = "";
    char subnetMask[INET_ADDRSTRLEN] = "";

    // ip address
    inet_ntop(AF_INET,
              static_cast<void*>(&net),
              &networkAddr[0],
              sizeof(networkAddr));
    // subnet mask
    inet_ntop(AF_INET,
              static_cast<void*>(&mask),
              &subnetMask[0],
              sizeof(subnetMask));

    Debug_Printf("interface=%s \n", devName.c_str());
    Debug_Printf("snaplen=%i \n", snapLength);
    Debug_Printf("interval=%i \n", bufferTimeout);
    Debug_Printf("networkAddr=%s \n", &networkAddr[0]);
    Debug_Printf("subnetMask=%s \n", &subnetMask[0]);
  }
}

//------------------------------------------------------------------------------
void PcapSniffer::CloseLive() {
  if(nullptr != pHandle)
  {
    pcap_close(pHandle);
    pHandle = nullptr;
  }
}

//------------------------------------------------------------------------------
bool PcapSniffer::Dispatch(int * pResult) {
  bool loop = false;

  if(nullptr != pDumper)
  {
    int result = pcap_dispatch(pHandle,
                               dumpCount,
                               &pcap_dump,
                               (unsigned char *) pDumper); // NOLINT

    if(-1 == result) {
      throw std::invalid_argument(pcap_geterr(pHandle));
    }

    if(-2 == result) {
      Debug_Printf("Breaking the loop is triggered \n");
      loop = false;
    }
    else {
      if(nullptr != pResult)
      {
        *pResult = result;
      }
      loop = true;
    }
  }
  else
  {
    throw std::invalid_argument("no valid savefile to dump data");
  }

  return loop;
}

//------------------------------------------------------------------------------
void PcapSniffer::CloseDump() {
  if(nullptr != pDumper)
  {
    pcap_dump_close(pDumper);
    pDumper = nullptr;
  }
}

//------------------------------------------------------------------------------
bool PcapSniffer::BreakLoop() {
  if(nullptr != pHandle) {
    pcap_breakloop(pHandle);
  }
  return false;
}
//------------------------------------------------------------------------------
bool PcapSniffer::Stats(pcap_stat * ps) {
  bool result = false;
  if((nullptr != pHandle) && (nullptr != ps))
  {
    if(0 == pcap_stats(pHandle, ps)) {
      result = true;
    }
  }
  return result;
}

//---- End of source file ------------------------------------------------------

