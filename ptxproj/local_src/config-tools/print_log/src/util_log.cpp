//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     util_print_file.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "util_log.hpp"

#include <iostream>
#include <boost/range.hpp>
#include <string>
#include "boost/filesystem.hpp"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define JSON_ARRAY_BEGIN "["
#define JSON_ARRAY_END "]"
#define JSON_STR_BEGIN "\""
#define JSON_STR_END "\""
#define JSON_ARRAY_NEXT ","

#define CMD_TAR_CREATE "tar cfzP "
#define CMD_OPKG_LIST "opkg list > "
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

std::string StringList2JsonArray(const std::vector<std::string> & list)
{
  std::string retStr;
  if(!list.empty())
  {
    retStr = JSON_ARRAY_BEGIN;
    for(auto it = list.cbegin(); it != list.cend();)
    {
      retStr.append(JSON_STR_BEGIN);
      retStr.append(*it);
      retStr.append(JSON_STR_END);
      it++;
      if(it != list.cend())
      {
        retStr.append(JSON_ARRAY_NEXT);
      }
    }
    retStr.append(JSON_ARRAY_END);
  }
  return retStr;
}

std::vector<boost::filesystem::path> GetFilePaths(const boost::filesystem::path & folderPath)
{
  std::vector<boost::filesystem::path> filePaths;

  if(boost::filesystem::is_directory(folderPath)) {

    // scan folder
    auto files = boost::make_iterator_range(boost::filesystem::directory_iterator(folderPath));

    // add regular and no empty file path to vector
    for(const auto& file  : files) {
      if(boost::filesystem::is_regular(file)) {
        if(!boost::filesystem::is_empty(file)) {
          filePaths.push_back(file.path());
        }
      }
    }
  }
  std::sort(filePaths.begin(), filePaths.end());

  return filePaths;
}

std::vector<std::string> GetFilenames(const boost::filesystem::path & folderPath)
{
  std::vector<boost::filesystem::path> filePaths = GetFilePaths(folderPath);
  std::vector<std::string> fileNames;

  
  // add filename to vector
  fileNames.reserve(filePaths.size());
  for(const auto& filePath : filePaths) {
    fileNames.emplace_back(filePath.filename().c_str());
  }

  return fileNames;
}

unsigned int GetNumberOfLines(const boost::filesystem::path & filePath)
{
  unsigned int number = 0;
  if(boost::filesystem::is_regular_file(filePath)) {
    std::ifstream is(filePath.c_str());
    std::string line;

    // Count number of lines
    while(std::getline(is, line)) {
      number++;
    }
  }

  return number;
}

unsigned int ReadFileLineByLine(const boost::filesystem::path & filePath,
                                unsigned int limit,
                                std::ostream & out)
{
  unsigned int outCounter = 0;
  unsigned int lines = GetNumberOfLines(filePath);

  if(lines > 0) {

    // Calculate start line
    unsigned int startLine = 0;
    if((limit > 0) && (limit <= lines)) {
      startLine = lines - limit;
    }

    // Read file line by line
    unsigned int lineCounter = 0;
    std::ifstream is(filePath.c_str());
    std::string line;
    while(std::getline(is, line)) {

      // Print out if start line reached
      if(lineCounter >= startLine) {
        out << line << std::endl;
        outCounter++;
      }
      lineCounter++;
    }
  }

  return outCounter;
}

eStatusCode PrintHelpText(std::ostream & out)
{
  out << "\n";
  out << "Print logging files command line tool\n";
  out << "\n";
  out << "options:\n";
  out << "  -h [--help]                   - show help text\n";
  out << "  -s [--scan]                   - scan all available file\n";
  out << "  -a [--archive]  <path>        - create log archive to destination path\n";
  out << "  -j [--json]                   - json output format\n";
  out << "  -r [--read]     <filename>    - read content of file\n";
  out << "  -l [--limit]    <value>       - limit read output\n";
  out << "\n";

  return SUCCESS;
}

eStatusCode PrintExistingFiles(const boost::filesystem::path & folderpath,
                               bool json,
                               std::ostream & out)
{
  eStatusCode status = ERROR;

  if(boost::filesystem::is_directory(folderpath)) {
    std::vector<std::string> files = GetFilenames(folderpath);
    // json format output
    if(json) {
      out << StringList2JsonArray(files) << std::endl;
    }
    // text format output
    else {
      for(auto const& it : files) {
        out << it << std::endl;
      }
    }
    status = SUCCESS;
  }

  return status;
}

eStatusCode PrintFileContent(const std::string & filename,
                             const boost::filesystem::path & folderpath,
                             unsigned int limit,
                             std::ostream & out)
{
  eStatusCode status = ERROR;
  boost::filesystem::path filePath = folderpath / filename;

  if(boost::filesystem::is_regular_file(filePath))
  {
    ReadFileLineByLine(filePath, limit, out);
    status = SUCCESS;
  }

  return status;
}

eStatusCode SystemCall(const std::string & cmd)
{
  eStatusCode status = ERROR;
  if(! cmd.empty())
  {
    if(std::system(cmd.c_str()) == 0)
    {
      status = SUCCESS;
    }
  }
  return status;
}

eStatusCode SavePackageList(const std::string & dst)
{
  eStatusCode status = ERROR;
  std::string cmd = CMD_OPKG_LIST;

  cmd.append(dst);
  status = SystemCall(cmd);

  return status;
}

eStatusCode CreateNewArchive(const std::vector<std::string> & srcList,
                             const std::string & dst)
{
  eStatusCode status = ERROR;

  if(!srcList.empty() && !dst.empty())
  {
    std::string cmd = CMD_TAR_CREATE;

    cmd.append(dst);
    for(auto &item : srcList)
    {
      if(! boost::filesystem::exists(item))
      {
        continue;
      }
      cmd.append(" ");
      cmd.append(item);
    }
    status = SystemCall(cmd);
  }

  return status;
}

bool FolderToPathExist(const std::string & dst)
{
  boost::filesystem::path p{dst};
  if(! boost::filesystem::is_directory(p)) // not a directory?
  {
    if(boost::filesystem::exists(p.parent_path())) // parent directory exists?
    {
      return true;
    }
  }
  return false;
}

//---- End of source file ------------------------------------------------------

