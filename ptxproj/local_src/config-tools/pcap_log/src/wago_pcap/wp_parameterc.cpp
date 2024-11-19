//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     wp_parameter.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wp_parameterc.hpp"
#include "wp_util.hpp"
#include <unistd.h>

//------------------------------------------------------------------------------
namespace wp {
  //----------------------------------------------------------------------------
  // defines; structure, enumeration and type definitions
  //----------------------------------------------------------------------------
  #define TIMESTAMP_FORMAT_FILE "%F_%T" // ISO 8601 without timezone information
  #define TIMESTAMP_FORMAT_JSON "%c"
  //----------------------------------------------------------------------------
  // function prototypes
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // macros
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // variables' and constants' definitions
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // function implementation
  //----------------------------------------------------------------------------
  bool jsonSaveToFile(const std::filesystem::path & path,
                      const nlohmann::json & json) {
    bool retVal = false;
    try {
      if(!json.empty() && !path.empty()) {
        std::ofstream outFile(path);
        outFile << std::setw(2) << json << std::endl;
        retVal = true;
      }
      else {
        throw std::invalid_argument("A Argument is empty");
      }
    }
    catch (...) {
      Debug_Printf("Save json to file(%s) failed \n", path.c_str());
    }
    return retVal;
  }

  //----------------------------------------------------------------------------
  nlohmann::json jsonLoadFromFile(const std::filesystem::path & path) {
    nlohmann::json json;
    try {
      // load from file
      if(std::filesystem::exists(path))
      {
        std::ifstream ifs(path);
        ifs >> json;
      }
      else {
        throw std::invalid_argument("File not exists");
      }
    }
    catch(...) {
      Debug_Printf("Load json form file(%s) failed \n", path.c_str());
    }
    return json;
  }

  //----------------------------------------------------------------------------
  bool jsonMerge(const std::string & str,
                 nlohmann::json * json) {
    bool retVal = false;
    if(nullptr != json) {
      try {
        if(nlohmann::json::accept(str, true)) {
          nlohmann::json patch = nlohmann::json::parse(str, nullptr, true, true);
          json->merge_patch(patch);
          retVal = true;
        }
        else {
          throw std::invalid_argument("Parse/Merge JSON-String failed");
        }
      }
      catch (...) {
        Debug_Printf("Parse/Merge json form string(%s) failed \n", str.c_str());
      }
    }
    return retVal;
  }

  //----------------------------------------------------------------------------
  std::filesystem::path getLogFolder(const std::filesystem::path & root)
  {
    auto full_path = root / SUBFOLDER_PCAP;
    if (!std::filesystem::exists(full_path))
    {
      std::filesystem::create_directory(full_path);
    }
    return full_path;
  }

  //----------------------------------------------------------------------------
  bool getOptMemoryCard() {
    return std::filesystem::exists(FOLDER_SD);
  }

  //----------------------------------------------------------------------------
  std::vector<std::string> getOptDevices() {
    std::vector<std::string> devices = {DFLT_DEVICE};
    netconf::api::BridgeConfig netBridgeConfig;

    netconf::Status netStatus = netconf::api::GetBridgeConfig(netBridgeConfig);
    if(netStatus.IsOk()) {
      auto netBridges = netBridgeConfig.GetBridges();
      if(!netBridges.empty()) {
        devices.clear();
        for (auto & netItem : netBridges) {
          devices.emplace_back(netItem);
        }
      }
    }
    return devices;
  }

  //----------------------------------------------------------------------------
  std::set<std::filesystem::path> getPcapFilePaths(
    const std::filesystem::path & dir,
    bool include_all_files)
  {
    std::set<std::filesystem::path> file_paths;

    if(std::filesystem::is_directory(dir)) {
      for(auto const & dir_entry : std::filesystem::directory_iterator(dir)) {
        if(std::filesystem::is_regular_file(dir_entry) &&
           (!std::filesystem::is_empty(dir_entry)) &&
           (dir_entry.path().extension() == ".pcapng" || dir_entry.path().extension() == ".pcap" || include_all_files)) {
          file_paths.insert(dir_entry.path());
        }
      }
    }

    return file_paths;
  }

  std::vector<std::string> getOptDlPaths(bool include_all_files) {
    std::vector<std::string> dlPaths;
    std::vector<std::filesystem::path> directorys = {getLogFolder(FOLDER_FLASH)};

    if(getOptMemoryCard()) {
      directorys.emplace_back(getLogFolder(FOLDER_SD));
    }

    for(const auto & dir : directorys) {
      for (const auto & file_path : getPcapFilePaths(dir, include_all_files))
      {
        dlPaths.emplace_back(file_path);
      }
    }

    return dlPaths;
  }

  //----------------------------------------------------------------------------
  std::string getTimestamp(std::time_t time, const char * format) {
    std::stringstream ss;

    ss << std::put_time(std::localtime(&time), format);
    std::string str = ss.str();

    return str;
  }

  //----------------------------------------------------------------------------
  std::filesystem::path insertTimestampNowToFilename(const std::filesystem::path & path)
  {
    std::filesystem::path newPath = path;
    std::string timestamp = getTimestamp(std::time(nullptr), TIMESTAMP_FORMAT_FILE);

    std::replace(timestamp.begin(), timestamp.end()-1, ':', '-');

    newPath.replace_filename(path.stem().string() + "_" +
                             timestamp +
                             path.extension().string());

    return newPath;
  }

  //----------------------------------------------------------------------------
  Info::Info() {
    this->setPath(PATH_INFO);
    if(!std::filesystem::exists(this->_path))
    {
      (void) save();
    }
  }

  void Info::updateOptions(bool get_all_files) {
    this->data.optMemCard = getOptMemoryCard();
    this->data.optDevices = getOptDevices();
    this->data.optDlPaths = getOptDlPaths(get_all_files);
  }

  void Info::updateLast(const unsigned int & recv,
                        const std::filesystem::path & path) {
    this->data.lastPid = getpid();
    this->data.lastRecv = recv;
    if(std::filesystem::exists(path)) {
      this->data.lastFSize = std::filesystem::file_size(path);
    }
    this->data.lastRfshTime = getTimestamp(std::time(nullptr), TIMESTAMP_FORMAT_JSON);
  }

  //----------------------------------------------------------------------------
  Config::Config() {
    this->setPath(PATH_CONFIG);
    if(!std::filesystem::exists(this->_path))
    {
      (void) save();
    }
  }

  std::filesystem::path Config::getFolder() {
    std::filesystem::path folder = FOLDER_FLASH;
    if(getOptMemoryCard() && (this->data.storage == STORAGE_MEMORY_CARD)) {
      folder = FOLDER_SD;
    }
    folder = getLogFolder(folder);
    return folder;
  }

  std::filesystem::path Config::getFilename() {
    std::filesystem::path filename = {SAVEFILE_NAME};
    filename = insertTimestampNowToFilename(filename);
    return filename;
  }

  void Config::registerSavefile(std::filesystem::path & path)
  {
    if (!data.rotateFiles)
    {
      return;
    }

    if (_file_rotation.size() == 0)
    {
      for (const auto & existing_path : getPcapFilePaths(path.parent_path()))
      {
        _file_rotation.emplace_back(existing_path);
      }
    }

    while (_file_rotation.size() >= DFLT_MAX_FILE_COUNT)
    {
      std::filesystem::path oldest_file = _file_rotation.front();
      std::filesystem::remove(oldest_file);
      _file_rotation.pop_front();
    }
    _file_rotation.emplace_back(path);
  }

  std::filesystem::path Config::getNewSavefile() {
    std::filesystem::path savefile = getFolder() / getFilename();
    registerSavefile(savefile);
    return savefile;
  }

  void Config::cleanLogsFromNonActiveStorage()
  {
    if (this->data.storage == STORAGE_MEMORY_CARD)
    {
      Debug_Printf("Purging old logs in %s...\n", FOLDER_FLASH);
      cleanLogs(FOLDER_FLASH);
    }
    else if (getOptMemoryCard() && this->data.storage == STORAGE_INTERNAL_FLASH)
    {
      Debug_Printf("Purging old logs in %s...\n", FOLDER_SD);
      cleanLogs(FOLDER_SD);
    }
  }

  void Config::cleanLogs(const std::string& storage)
  {
    const auto dir = getLogFolder(storage);
    if (!std::filesystem::exists(dir))
    {
      return;
    }

    for (const auto & entry : std::filesystem::directory_iterator(dir))
    {
      std::filesystem::remove_all(entry.path());
    }
  }

  std::uintmax_t Config::calcMaxPartitionSize()
  {
    auto available_mem = get_avail_mem();

    std::uint8_t percentage = data.maxPartitionSizePct > MAX_PART_SIZE_PERC ? MAX_PART_SIZE_PERC : data.maxPartitionSizePct;
    auto allowed_size = available_mem * percentage / 100;

    std::uintmax_t target_mem_size = data.maxFilesize;
    if (data.rotateFiles)
    {
      target_mem_size *= DFLT_MAX_FILE_COUNT;
    }
    target_mem_size += 1 * 1024 * 1024; // 1 MB buffer in case the files are slightly larger than maxFilesize

    if (allowed_size >= target_mem_size)
    {
      // return target_mem_size; // use this instead to create partitions that allow the currently needed space only
      return allowed_size;
    }

    return 0;
  }

  //----------------------------------------------------------------------------
} /* namespace wp */
//---- End of source file ------------------------------------------------------

