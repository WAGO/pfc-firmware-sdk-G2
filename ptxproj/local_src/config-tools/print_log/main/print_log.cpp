//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     main.cpp
///
///  \brief    file print function for WBM diagnostic.
///
///  \author   N. Baade : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <getopt.h>
#include <iostream>
#include <string>       // std::string
#include <sstream>      // std::stringbuf
#include <algorithm>    // std::find
#include "boost/filesystem.hpp"
#include "config_tool_lib.h"

#include "util_log.hpp"
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
struct option g_longopts[] = {
   { "help",        no_argument,        nullptr,   'h' },
   { "json",        no_argument,        nullptr,   'j' },
   { "read",        required_argument,  nullptr,   'r' },
   { "scan",        no_argument,        nullptr,   's' },
   { "limit",       required_argument,  nullptr,   'l' },
   { "archive",     required_argument,  nullptr,   'a' },
   // last line
   { nullptr,       no_argument,        nullptr,    0  }
};

// path
const char * g_syslogPath =   "/var/log/";
const char * g_svnRevPath =   "/etc/SVNREVISION";
const char * g_RevPath =      "/etc/REVISIONS";
const char * g_packagesPath = "/tmp/packages.txt";

// archive list
std::vector<std::string> g_archiveList;

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
static std::vector<std::string> & GetArchiveList()
{
  if(g_archiveList.empty())
  {
    g_archiveList.emplace_back(g_syslogPath);
    g_archiveList.emplace_back(g_svnRevPath);
    g_archiveList.emplace_back(g_RevPath);
    g_archiveList.emplace_back(g_packagesPath);
  }
  return g_archiveList;
}

static void archiveProlog()
{
  // generate Package List
  (void) SavePackageList(g_packagesPath);
}

static void archiveEpilog()
{
  // remove Package List
  if(boost::filesystem::exists(g_packagesPath))
  {
    boost::filesystem::remove(g_packagesPath);
  }
}

//------------------------------------------------------------------------------
// argc =    1,   2,   3,   4,  ...
// argv =   [0], [1], [2], [3], ...
//------------------------------------------------------------------------------
int main(int    argc,
         char** argv)
{
  int status = SUCCESS;

  // operation code
  int oc;

  // buffer
  int buffer_oc = 0;
  unsigned int buffer_limit = 0;
  bool buffer_json = false;
  std::string buffer_arg;

  // check arg
  if(argc <= 1) {
    status = MISSING_PARAMETER;
  }
  else {

    // first check all options
    while ((oc = getopt_long(argc,
                             argv,
                             "hjr:sl:a:",
                             &g_longopts[0],
                             nullptr)) != -1) {
      switch (oc) {
        // valid options
        case 'h':
          status = PrintHelpText(std::cout);
          break;
        case 'j':
          buffer_json = true;
          break;
        case 'l':
          buffer_limit = strtoul (optarg, nullptr, 0);
          break;
        case 'a':
          archiveProlog();
          if(FolderToPathExist(optarg))
          {
            auto archivList = GetArchiveList();
            status = CreateNewArchive(archivList,
                                      optarg);
            if(SUCCESS == status)
            {
              std::cout << "Create archive to: " << optarg << " done" << std::endl;
            }
          }
          else
          {
            std::cout << "Create archive to: " << optarg << " failed" << std::endl;
            status = INVALID_PARAMETER;
          }
          archiveEpilog();
          break;

        // valid option for later doing
        case 'r':
          buffer_oc = oc;
          buffer_arg = optarg;
          break;
        case 's':
          buffer_oc = oc;
          break;

        // invalid options or missing option argument
        case '?':
          status = INVALID_PARAMETER;
          break;
        case ':':
          status = MISSING_PARAMETER;
          break;
        default:
          status = ERROR;
          break;
      }
    }

    // second work with all options
    if(0 != buffer_oc) {
      switch(buffer_oc) {
        // read file
        case 'r':
          status = INVALID_PARAMETER;
          // check filename characters
          if(buffer_arg.find('/') == std::string::npos)
          {
            // check filename exist
            std::vector<std::string> files = GetFilenames(g_syslogPath);
            if(std::find(files.begin(), files.end(), buffer_arg) != files.end())
            {
              status = PrintFileContent(buffer_arg,
                                        g_syslogPath,
                                        buffer_limit,
                                        std::cout);
            }
          }
          break;

        // scan folder
        case 's':
          status = PrintExistingFiles(g_syslogPath,
                                      buffer_json,
                                      std::cout);
          break;
        default:
          status = ERROR;
          break;
      }
    }
  }

  return status;
}

//---- End of source file ------------------------------------------------------


