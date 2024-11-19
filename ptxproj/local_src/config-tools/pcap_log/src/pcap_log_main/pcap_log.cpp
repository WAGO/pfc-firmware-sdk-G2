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

#include "wp_edit.hpp"
#include "wp_parameterc.hpp"
#include "wp_sniffer.hpp"
#include "wp_debug.hpp"
#include "wp_util.hpp"

#include "netconf/BridgeConfig.hpp"

extern "C"
{
#include <ct_liblog.h>
}

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define ARCHIVE_NAME "network_capture_logs.tar.gz"
#define CMD_TAR_CREATE "tar cfz "
#define CMD_TAR_FLATTEN_FLAG "--transform='s,.*/,,'"

enum class opt_actions
{
  none = 1 << 0,
  stop_pcap = 1 << 1,
  restart_pcap = 1 << 2,
  get_config = 1 << 3,
  set_config = 1 << 4,
  get_info = 1 << 5,
  archive_all = 1 << 6,
  archive = 1 << 7,
  test_filter = 1 << 8,
  rm_logs = 1 << 9,
  inject_secrets = 1 << 10
};

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
opt_actions operator|(opt_actions lhs, opt_actions rhs);
opt_actions operator&(opt_actions lhs, opt_actions rhs);
bool test(opt_actions action);

static bool DoStop();
static int DoStart();
void tmp_pcpp();

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
   { "incfiles",    optional_argument,  nullptr,   'f' },
   { "delete",      no_argument,        nullptr,   'r' },
   { "inject",      required_argument,  nullptr,   'j' },
   // last line
   { nullptr,       0,                  nullptr,    0  }
};

bool RunLoop = false;
//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
opt_actions operator|(opt_actions lhs, opt_actions rhs)
{
  return static_cast<opt_actions>(
    static_cast<std::underlying_type<opt_actions>::type>(lhs) |
    static_cast<std::underlying_type<opt_actions>::type>(rhs));
}

inline opt_actions & operator|=(opt_actions & lhs, opt_actions rhs)
{
  return lhs = lhs | rhs;
}

opt_actions operator&(opt_actions lhs, opt_actions rhs)
{
  return static_cast<opt_actions>(
    static_cast<std::underlying_type<opt_actions>::type>(lhs) &
    static_cast<std::underlying_type<opt_actions>::type>(rhs));
}

bool test(opt_actions action)
{
  return static_cast<std::underlying_type<opt_actions>::type>(action);
}

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
  std::cout << "  -f [--incfiles] -if used with -a: include these files in the archive,\n";
  std::cout << "                   if used with -j: inject secrets into these files;\n";
  std::cout << "                   can be used multiple times,\n";
  std::cout << "                   only allows .log/.pcap/.pcapng files below /var/tmp;\n";
  std::cout << "  -a [--archive]  -create log archive to destination directory\n";
  std::cout << "  -r [--delete]   -remove all log files\n";
  std::cout << "  -j [--inject]   -inject given secrets file into files given with -f,\n";
  std::cout << "                   format: -j <type>,<file path>\n";
  std::cout << "                   where <type> can be one of:\n";
  std::cout << "                     - tls\n";
  std::cout << "                   if no files are given with -f, all current logs will be used\n";
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
static void ChangeConfig(const std::string & arg)
{
  Debug_Printf("ChangeConfig(%s) \n", arg.c_str());
  wp::Config config;
  config.load();
  auto prev_json = config.json();
  config.merge(UrlStringToString(arg));
  config.save();

  auto new_json = config.json();
  if (new_json["storage"] != prev_json["storage"])
  {
    config.cleanLogsFromNonActiveStorage();
  }
}

//------------------------------------------------------------------------------
static void RemoveLogs()
{
  Debug_Printf("RemoveLogs() \n");
  wp::Config config;
  config.load();
  config.cleanLogs(FOLDER_FLASH);
  if (wp::getOptMemoryCard())
  {
    config.cleanLogs(FOLDER_SD);
  }
}

//------------------------------------------------------------------------------
static bool InjectSecrets(const std::string & arg, std::vector<std::string> & pcapngFiles)
{
  Debug_Printf("InjectSecrets(%s) \n", arg.c_str());

  std::string arg_type;
  std::string arg_path;
  std::stringstream arg_stream{arg};

  // expect format like "tls,/a/path.log"
  if (!std::getline(arg_stream, arg_type, ',') || arg_type.empty())
  {
    Debug_Printf("Invalid argument formatting\n");
    return false;
  }
  if (!std::getline(arg_stream, arg_path, ',') || arg_path.empty())
  {
    Debug_Printf("Invalid argument formatting\n");
    return false;
  }
  if (!canonicalise_file_path(arg_path) || !check_allowed_extra_file_path(arg_path))
  {
    Debug_PrintAndLogToFile(INJECT_SECRETS_LOG_PATH, "No valid secrets log found at \"%s\".", arg_path.c_str());
    return false;
  }

  pcpp::DecryptionSecretType secret_type;
  if (!wp::SecretsHandler::parse_secrets_type(arg_type, secret_type))
  {
    Debug_Printf("Invalid secret type \"%s\"\n", arg_type.c_str());
    return false;
  }

  std::vector<pcpp::DecryptionSecret> secrets;
  pcpp::DecryptionSecret secret(secret_type, 0, nullptr);
  if (!wp::SecretsHandler::parse_secrets_file(arg_path, secret_type, secret))
  {
    Debug_PrintAndLogToFile(INJECT_SECRETS_LOG_PATH, "Failed to parse secrets log file \"%s\".", arg_path.c_str());
    return false;
  }
  secrets.emplace_back(secret);

  // if no pcaps given, use any existing in capture dir
  if (pcapngFiles.empty())
  {
    wp::Info info;
    info.load();
    info.updateOptions(true);
    for (const auto & file : info.data.optDlPaths)
    {
      std::filesystem::path path{file};
      if (path.extension() == ".pcap" || path.extension() == ".pcapng")
      {
        pcapngFiles.emplace_back(file);
      }
    }
  }

  bool result = true;
  for (const auto & pcapngFile : pcapngFiles)
  {
    if (!wp::SecretsHandler::inject_secrets(secrets, pcapngFile))
    {
      Debug_Printf("Failed to inject secrets into \"%s\"\n", pcapngFile.c_str());
      result = false;
    }
  }

  return result;
}

//------------------------------------------------------------------------------
static void TestFilter(const std::string & arg)
{
  Debug_Printf("TestFilter() \n");
  nlohmann::json j;
  j["filter"] = "";

  bool result = true; // true as default to allow empty filter / no argument
  if (!arg.empty())
  {
    j["filter"] = UrlStringToString(arg);
    PcapSniffer& sniffer = PcapSniffer::Instance();
    result = sniffer.CompileFilter(UrlStringToString(arg), DFLT_MAX_PACKET_LEN);
  }

  j["isOk"] = result;
  std::cout << j.dump() << std::endl;
}

//------------------------------------------------------------------------------
static bool CreateArchive(const std::string & arg, bool includeAllFiles, const std::vector<std::string> & extraFiles)
{
  Debug_Printf("CreateArchive(,%s) \n", includeAllFiles ? "true" : "false");
  if (arg.empty() ||
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
    for (const auto & file : extraFiles)
    {
      cmd.append(" ");
      cmd.append(file);
    }

    if (std::system(cmd.c_str()) == 0)
    {
      Debug_Printf("Created archive at %s \n", arg.c_str());
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
static bool DoStop()
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

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
static int DoStart()
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

    // create tmp partition if saving in RAM
    if (config.data.storage == STORAGE_INTERNAL_FLASH)
    {
      auto full_path = std::filesystem::canonical(wp::getLogFolder(FOLDER_FLASH));
      std::uintmax_t max_size = config.calcMaxPartitionSize();
      bool result = max_size > 0;
      // only create mountpoint if it is not one yet or the mounted path is empty
      // to prevent deleting previous logs
      if (result && (!is_mounted_dir(full_path) || std::filesystem::is_empty(full_path)))
      {
        result &= create_tmpfs_mount(full_path, max_size);
      }

      if (!result)
      {
        // failed to prepare mount, abort
        ct_liblog_setLastError("Failed to create capture log location: Not enough memory. Lower the maximum log size or disable log rotation!");
        return 10;
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
    sniffer.OpenLive(config.data.device, config.data.maxPacketLen);
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

      // filesize exceeded or file no longer exists
      if(info.data.lastFSize >= config.data.maxFilesize ||
         !std::filesystem::exists(sniffer.savefile))
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
        sniffer.OpenLive(config.data.device, config.data.maxPacketLen);
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

    return 0;
  }
  catch (std::exception& e) {
    Debug_Printf("%s \n", e.what());
  }

  return 1;
}

//------------------------------------------------------------------------------
// argc =    1,   2,   3,   4,  ...
// argv =   [0], [1], [2], [3], ...
//------------------------------------------------------------------------------
int main(int    argc,
         char** argv)
{
  int result = 0;
  int oc;       // option character
  int ocIndex;  //

  std::signal(SIGTERM, HandleSignal);
  std::signal(SIGINT, HandleSignal);
  std::signal(SIGQUIT, HandleSignal);

  opt_actions option_actions = opt_actions::none;
  std::string config_optarg = "";
  std::string filter_optarg = "";
  std::string archive_optarg = "";
  std::vector<std::string> archive_extra_files;
  std::string inject_optarg = "";

  while((oc = getopt_long(argc, argv, "drhic::t::f:a:j:", &longopts[0], &ocIndex)) != -1)
  {
    Debug_Printf("case: -%c \n", oc);
    switch(oc)
    {
      case 'd':
        Debug_SetEnable();
        Debug_Printf("debugPrintOn(), argc: %i \n", argc);
        break;
      case 'f':
        {
          std::string extra_path = optarg;
          if (optarg != nullptr && canonicalise_file_path(extra_path) && check_allowed_extra_file_path(extra_path))
          {
            archive_extra_files.emplace_back(extra_path);
          }
          break;
        }
      case 'c':
        if(nullptr == optarg)
        {
          option_actions |= opt_actions::get_config;
        }
        else
        {
          option_actions |= opt_actions::set_config;
          config_optarg = optarg;
        }
        break;
      case 'i':
        option_actions |= opt_actions::get_info;
        break;
      case '0':
        option_actions |= opt_actions::stop_pcap;
        break;
      case '1':
        option_actions |= opt_actions::restart_pcap;
        break;
      case 't':
        option_actions |= opt_actions::test_filter;
        if (optarg != nullptr)
        {
          filter_optarg = optarg;
        }
        break;
      case 'a':
        option_actions |= opt_actions::archive;
        if (optarg != nullptr)
        {
          archive_optarg = optarg;
        }
        break;
      case 'r':
        option_actions |= opt_actions::rm_logs;
        break;
      case 'j':
        option_actions |= opt_actions::inject_secrets;
        inject_optarg = optarg;
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

  if (test(option_actions & opt_actions::get_info))
  {
    ShowInfo();
  }

  if (test(option_actions & opt_actions::get_config))
  {
    ShowConfig();
  }

  if (test(option_actions & opt_actions::test_filter))
  {
    TestFilter(filter_optarg);
  }

  if (test(option_actions & opt_actions::rm_logs))
  {
    RemoveLogs();
  }

  if (test(option_actions & opt_actions::archive))
  {
    if (!CreateArchive(archive_optarg, test(option_actions & opt_actions::archive_all), archive_extra_files))
    {
      Debug_Printf("Failed to create the archive.\n");
    }
  }

  bool wasRunningBefore = false;
  if (test(option_actions & opt_actions::stop_pcap) ||
      test(option_actions & opt_actions::set_config) ||
      test(option_actions & opt_actions::restart_pcap))
  {
    wasRunningBefore = DoStop();
  }

  if (test(option_actions & opt_actions::set_config) && !config_optarg.empty())
  {
    ChangeConfig(config_optarg);

    if (wasRunningBefore && !test(option_actions & opt_actions::stop_pcap))
    {
      // restart pcap if we stopped it before changing the config
      option_actions |= opt_actions::restart_pcap;
    }
  }

  if (test(option_actions & opt_actions::restart_pcap))
  {
    usleep(100 * 1000); // wait 100ms to finish killing the pcap logger in case it was stopped before
    int start_result = DoStart();
    if (start_result != 0)
    {
      Debug_Printf("Failed to start with code %d\n", start_result);
      result = start_result;
    }
  }

  if (test(option_actions & opt_actions::inject_secrets))
  {
    if (wasRunningBefore)
    {
      usleep(100 * 1000); // wait 100ms to finish killing the pcap logger before accessing the files
    }

    if (!InjectSecrets(inject_optarg, archive_extra_files))
    {
      Debug_Printf("Could not inject secrets into at least one file.\n");
    }
  }

  return result;
}

//---- End of source file ------------------------------------------------------
