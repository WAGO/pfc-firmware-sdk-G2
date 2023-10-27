//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     wp_sniffer.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_CLASS_PCAP_HPP_
#define SRC_CLASS_PCAP_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <pcap/pcap.h>
#include <iostream>
#include <filesystem>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

/// The Singleton class defines the `Instance` method that serves as an
/// alternative to constructor and lets clients access the same instance of this
/// class over and over.
class PcapSniffer
{
    //--- attributes/variables ----
  private:
    char errbuf[PCAP_ERRBUF_SIZE] = "";
    pcap_t *pHandle = nullptr;
    pcap_dumper_t *pDumper = nullptr;
    u_int32_t net = 0;
    u_int32_t mask = 0;
    const int optimize = 1;

  public:
    int dumpCount = 200; // processes packets until packets will be dumped
    int bufferTimeout = 10000; // packet buffer timeout, as a non-negative value, in milliseconds
    std::filesystem::path savefile;
    // ----------------------------
    //--- methods/functions -------
  private:
    PcapSniffer() = default;
    void CloseDump();
    void CloseLive();
    std::string GetDateTime(std::time_t time);
    std::filesystem::path AppendDateTime(const std::filesystem::path& path);

  public:
    static PcapSniffer& Instance() {
      static PcapSniffer _instance;
      return _instance;
    }
    ~PcapSniffer();
    PcapSniffer(const PcapSniffer & other)  = delete;
    PcapSniffer & operator = (const PcapSniffer & other) = delete;
    PcapSniffer(const PcapSniffer &&) = delete;
    PcapSniffer & operator = (const PcapSniffer &&) = delete;

    bool CompileFilter(const std::string & filter, std::uint16_t snapLength);

    void OpenLive(const std::string & devName, std::uint16_t snapLength);
    void OpenDump(const std::filesystem::path & path);
    void SetFilter(const std::string & filter);
    bool Dispatch(int * pResult);
    bool Stats(pcap_stat * ps);
    bool BreakLoop();
    void Close();
    // ----------------------------
};

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* SRC_CLASS_PCAP_HPP_ */
//---- End of source file ------------------------------------------------------

