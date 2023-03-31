//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     pcap_log.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <iostream>
#include <getopt.h>
#include <cstdarg>
#include <csignal>
#include <unistd.h>

#include "wp_parameterc.hpp"
#include "wp_sniffer.hpp"
#include "wp_debug.hpp"
#include "wp_util.hpp"

#include "netconf/BridgeConfig.hpp"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define ARCHIVE_NAME "network_capture_logs.tar.gz"
#define CMD_TAR_CREATE "tar cfz "
#define CMD_TAR_FLATTEN_FLAG "--transform='s,.*/,,'"

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
static void DoStop();
static void DoStart();

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------
option longopts[] = {
   { "debug",       no_argument,        nullptr,   'd' },
   { "help",        no_argument,        nullptr,   'h' },
   { "info",        no_argument,        nullptr,   'i' },
   { "config",      optional_argument,  nullptr,   'c' },
   { "start",       no_argument,        nullptr,   '1' },
   { "stop",        no_argument,        nullptr,   '0' },
   { "test",        optional_argument,  nullptr,   't' },
   { "archive",     required_argument,  nullptr,   'a' },
   { "allfiles",    no_argument,        nullptr,   'f' },
   // last line
   { nullptr,       0,                  nullptr,    0  }
};

bool RunLoop = false;
//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
static void HandleSignal(int signal)
{
  (void) signal;
  PcapSniffer& sniffer = PcapSniffer::Instance();
  RunLoop = sniffer.BreakLoop();
}

//------------------------------------------------------------------------------
static void ShowStats(wp::info_t info, bool newLine)
{
  std::cout << info.lastRfshTime
            <<": packets=" << info.lastRecv
            << ", filesize=" << info.lastFSize
            << ", pid=" << info.lastPid;
  if(newLine) {
    std::cout << std::endl;
  }
  else {
    std::cout << "\r" << std::flush;
  }
}

//------------------------------------------------------------------------------
static void ShowHelp()
{
  Debug_Printf("ShowHelp() \n");

  std::cout << "\n";
  std::cout << "Packet capture logging command line tool\n";
  std::cout << "\n";
  std::cout << "options: \n";
  std::cout << "  -d [--debug]    -print trace msg, must be first option \n";
  std::cout << "  -h [--help]     -show help text \n";
  std::cout << "  -i [--info]     -configuration and state of service [JSON] \n";
  std::cout << "  -c [--config]   -configure service [JSON] \n";
  std::cout << "     [--start]    -start service to capture packets \n";
  std::cout << "     [--stop]     -stop service to capture packets \n";
  std::cout << "  -t [--test]     -test if a filter expression compiles\n";
  std::cout << "  -f [--allfiles] -include all files in the archive (see -a),\n";
  std::cout << "                   must come before -a\n";
  std::cout << "  -a [--archive]  -create log archive to destination directory\n";
  std::cout << "\n";
}

//------------------------------------------------------------------------------
static void ShowInfo()
{
  Debug_Printf("ShowInfo() \n");
  wp::Info info;
  info.load();
  info.updateOptions();
  std::cout << info.json() << std::endl;
}

//------------------------------------------------------------------------------
static void ShowConfig()
{
  Debug_Printf("ShowConfig() \n");
  wp::Config config;
  config.load();
  std::cout << config.json() << std::endl;
}

//------------------------------------------------------------------------------
static void ChangeConfig(char *arg)
{
  Debug_Printf("ChangeConfig(%s) \n", arg);
  //PcapJSON& json = PcapJSON::Instance();
  wp::Config config;
  config.load();
  auto prev_json = config.json();
  config.merge(UrlStringToString(arg));
  config.save();

  auto new_json = config.json();
  if (new_json != prev_json)
  {
    wp::Info info;
    info.load();
    if (info.data.isRunning)
    {
      // config was changed, restart pcap
      DoStop();
      usleep(100 * 1000); // wait 100ms to finish killing the pcap logger
      DoStart();
    }

    if (new_json["storage"] != prev_json["storage"])
    {
      config.cleanLogsFromNonActiveStorage();
    }
  }
}

//------------------------------------------------------------------------------
static void TestFilter(char *arg)
{
  Debug_Printf("TestFilter() \n");
  nlohmann::json j;
  j["filter"] = "";

  bool result = true; // true as default to allow empty filter / no argument
  if (arg != nullptr)
  {
    j["filter"] = UrlStringToString(arg);
    PcapSniffer& sniffer = PcapSniffer::Instance();
    result = sniffer.CompileFilter(UrlStringToString(arg));
  }

  j["isOk"] = result;
  std::cout << j.dump() << std::endl;
}

//------------------------------------------------------------------------------
static bool CreateArchive(char *arg, bool includeAllFiles)
{
  Debug_Printf("CreateArchive(,%s) \n", includeAllFiles ? "true" : "false");
  if (arg == nullptr ||
      !std::filesystem::is_directory(arg) ||
      !std::filesystem::exists(arg))
  {
    Debug_Printf("Invalid destination path \n");
    return false;
  }

  wp::Info info;
  info.load();
  info.updateOptions(includeAllFiles);
  if (info.data.optDlPaths.size() > 0)
  {
    std::string cmd = CMD_TAR_CREATE;
    cmd.append(std::filesystem::path(arg) / ARCHIVE_NAME);
    cmd.append(" ");
    cmd.append(CMD_TAR_FLATTEN_FLAG);
    for (const auto & file : info.data.optDlPaths)
    {
      cmd.append(" ");
      cmd.append(file);
    }

    if (std::system(cmd.c_str()) == 0)
    {
      Debug_Printf("Created archive at %s \n", arg);
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
static void DoStop()
{
  Debug_Printf("DoStop() \n");
  wp::Info info;
  info.load();
  if(info.data.lastPid > 0 && 0 == kill(info.data.lastPid, 0)) {
    kill(info.data.lastPid, SIGTERM);
  }
  if (info.data.isRunning)
  {
    info.data.isRunning = false;
    info.save();
  }
}

//------------------------------------------------------------------------------
static void DoStart()
{
  Debug_Printf("DoRun() \n");
  try {
    // init
    wp::Config config;
    wp::Info info;

    config.load();
    info.load();

    if(info.data.lastPid != 0) {
      if(0 == kill(info.data.lastPid, 0)) {
        throw std::invalid_argument("pcap_log is already running");
      }
    }

    if (!Debug_GetState())
    {
      if(0 != daemon(true, false))
      {
        throw std::runtime_error("Daemonising failed");
      }
    }

    // open sniffer
    PcapSniffer& sniffer = PcapSniffer::Instance();
    sniffer.OpenLive(config.data.device);
    sniffer.SetFilter(config.data.filter);
    sniffer.OpenDump(config.getNewSavefile());

    // loop sniffer
    RunLoop = true;
    int result = 0;
    unsigned int total = 0;

    while(RunLoop)
    {
      // statistics
      info.updateLast(total, sniffer.savefile);
      info.data.isRunning = true;
      info.save();
      ShowStats(info.data, false);

      // filesize
      if(info.data.lastFSize >= config.data.maxFilesize)
      {
        RunLoop = sniffer.BreakLoop();
      }

      RunLoop = sniffer.Dispatch(&result);
      total += static_cast<unsigned int>(result);

      // restart sniffer loop after switching file
      if (!RunLoop &&
          config.data.rotateFiles &&
          info.data.lastFSize >= config.data.maxFilesize)
      {
        Debug_Printf("Reached maximum filesize, continue with next file\n");
        info.data.isRunning = false;
        sniffer.Close();
        sniffer.OpenLive(config.data.device);
        sniffer.SetFilter(config.data.filter);
        sniffer.OpenDump(config.getNewSavefile());
        RunLoop = true;
        total = 0;
      }
    }
    // loop sniffer

    info.updateLast(total, sniffer.savefile);
    info.data.isRunning = false;
    sniffer.Close();
    ShowStats(info.data, true);
    info.save();
  }
  catch (std::exception& e) {
    Debug_Printf("%s \n", e.what());
  }
}

//------------------------------------------------------------------------------
// argc =    1,   2,   3,   4,  ...
// argv =   [0], [1], [2], [3], ...
//------------------------------------------------------------------------------
int main(int    argc,
         char** argv)
{
  int oc;       // option character
  int ocIndex;  //

  std::signal(SIGTERM, HandleSignal);
  std::signal(SIGINT, HandleSignal);
  std::signal(SIGQUIT, HandleSignal);

  bool archiveIncludeAllFiles = false;

  while((oc = getopt_long(argc, argv, "dfhic::t::a:", &longopts[0], &ocIndex)) != -1)
  {
    Debug_Printf("case: -%c \n", oc);
    switch(oc)
    {
      case 'd':
        Debug_SetEnable();
        Debug_Printf("debugPrintOn(), argc: %i \n", argc);
        break;
      case 'f':
        archiveIncludeAllFiles = true;
        break;
      case 'c':
        if(nullptr ==optarg)
        {
          ShowConfig();
        }
        else
        {
          ChangeConfig(optarg);
        }
        break;
      case 'i':
        ShowInfo();
        break;
      case '0':
        DoStop();
        break;
      case '1':
        DoStart();
        break;
      case 't':
        TestFilter(optarg);
        break;
      case 'a':
        if (!CreateArchive(optarg, archiveIncludeAllFiles))
        {
          Debug_Printf("Failed to create the archive.\n");
        }
        break;
      // invalid options, missing option argument or show help
      case '?':
      case 'h':
      case ':':
      default :
        ShowHelp();
        return 0;
    }
  }

  return 0;
}

//---- End of source file ------------------------------------------------------

