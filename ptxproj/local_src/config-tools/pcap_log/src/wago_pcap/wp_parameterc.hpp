//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     wp_parameter.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_WAGO_PCAP_JSON_WP_PARAMETER_HPP_
#define SRC_WAGO_PCAP_JSON_WP_PARAMETER_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <deque>
#include <filesystem>
#include <fstream>
#include <set>
#include <vector>

#include "wp_debug.hpp"
#include "nlohmann/json.hpp"
#include "netconf/BridgeConfig.hpp"

//------------------------------------------------------------------------------
namespace wp {
  //----------------------------------------------------------------------------
  // defines; structure, enumeration and type definitions
  //----------------------------------------------------------------------------
  #define FOLDER_SD "/media/sd/"
  #define FOLDER_FLASH "/var/tmp/"
  #define SUBFOLDER_PCAP "pcap/"
  #define PATH_CONFIG "/etc/pcap_log.conf"
  #define PATH_INFO "/var/run/pcap_log.info"
  #define SAVEFILE_NAME "log.pcap"

  #define STORAGE_MEMORY_CARD "Memory Card"
  #define STORAGE_INTERNAL_FLASH "RAM Disk"

  #define KB_TO_BYTE(value) ((value) * (1024u))
  #define BYTE_TO_KB(value) ((value) / (1024u))

    // default values
  #define DFLT_DEVICE "br0"
  #define DFLT_EMPTY_STR ""
  #define DFLT_SD false
  #define DFLT_MAX_FILE_SIZE (KB_TO_BYTE(51200))
  #define DFLT_MAX_FILE_COUNT 3
  #define DFLT_STORAGE STORAGE_INTERNAL_FLASH
  #define DFLT_ROTATE_FILES true
  #define DFLT_MAX_PART_SIZE_PERC 60
  #define DFLT_MAX_PACKET_LEN 2048

  // percentage of the available memory that should be used as the partition max size
  // (upper limit)
  #define MAX_PART_SIZE_PERC 80

  //----------------------------------------------------------------------------
  struct config_t{
      std::string device {DFLT_DEVICE};
      std::string filter {DFLT_EMPTY_STR};
      std::string storage {DFLT_STORAGE};
      std::uintmax_t maxFilesize {DFLT_MAX_FILE_SIZE};
      bool rotateFiles {DFLT_ROTATE_FILES};
      std::uint8_t maxPartitionSizePct {DFLT_MAX_PART_SIZE_PERC};
      std::uint16_t maxPacketLen {DFLT_MAX_PACKET_LEN};
  };

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(config_t, // NOLINT
                                                  device,
                                                  filter,
                                                  storage,
                                                  maxFilesize,
                                                  rotateFiles,
                                                  maxPartitionSizePct,
                                                  maxPacketLen)

  //----------------------------------------------------------------------------
  struct info_t{
      bool isRunning {false};
      pid_t lastPid {0};
      unsigned int lastRecv {0u};
      std::uintmax_t lastFSize {0u};
      std::string lastRfshTime = {DFLT_EMPTY_STR};
      std::vector<std::string> optDlPaths = {};
      std::vector<std::string> optDevices = {DFLT_DEVICE};
      bool optMemCard {DFLT_SD};
  };

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(info_t, // NOLINT
                                                  isRunning,
                                                  lastPid,
                                                  lastRecv,
                                                  lastFSize,
                                                  lastRfshTime,
                                                  optMemCard,
                                                  optDlPaths,
                                                  optDevices)

  //----------------------------------------------------------------------------
  // function prototypes
  //----------------------------------------------------------------------------
  bool jsonSaveToFile(const std::filesystem::path & path,
                      const nlohmann::json & json);

  nlohmann::json jsonLoadFromFile(const std::filesystem::path & path);

  bool jsonMerge(const std::string & str,
                 nlohmann::json * json);

  template <class T>
  bool jsonToStruct(const nlohmann::json & json,
                    T * data) {
    bool retVal = false;
    if(nullptr != data) {
      try {
        *data = json.get<T>();
        retVal = true;
      }
      catch(...) {
        Debug_Printf("Convert json to struct failed \n");
      }
    }
    return retVal;
  }

  template <class T>
  nlohmann::json structToJson(const T & data) {
    nlohmann::json json;
    try {
      json = data;
    }
    catch(...) {
      Debug_Printf("Convert struct to json failed \n");
    }
    return json;
  }

  std::filesystem::path getLogFolder(const std::filesystem::path & root);

  bool getOptMemoryCard();

  std::vector<std::string> getOptDevices();

  std::vector<std::string> getOptDlPaths(bool include_all_files = false);

  std::set<std::filesystem::path> getPcapFilePaths(
    const std::filesystem::path & dir,
    bool include_all_files = false);

  std::string getTimestamp(std::time_t time, const char * format);

  std::filesystem::path insertTimestampNowToFilename(const std::filesystem::path & path);
  //----------------------------------------------------------------------------
  // classes
  //----------------------------------------------------------------------------
  template <class T>
  class Parameter {
    protected:
      std::filesystem::path _path;

    public:
      T data;

      void setPath(const std::filesystem::path & path) {
        _path = path;
      }

      bool save() {
        bool retVal = false;
        nlohmann::json _json = structToJson(data);

        if(!_json.empty()) {
          retVal = jsonSaveToFile(_path, _json);
        }
        return retVal;
      }

      bool load() {
        bool retVal = false;
        nlohmann::json _json = jsonLoadFromFile(_path);
        if(!_json.empty()) {
          retVal = jsonToStruct(_json, &data);
        }
        return retVal;
      }

      bool merge(const std::string & str) {
        bool retVal = false;
        nlohmann::json _json = structToJson(data);

        retVal = jsonMerge(str, &_json);
        if(retVal) {
          retVal = jsonToStruct(_json, &data);
        }
        return retVal;
      }

      nlohmann::json json() {
        nlohmann::json _json = structToJson(data);
        return _json;
      }
  };

  //----------------------------------------------------------------------------
  class Info : public Parameter<info_t> {
    public:
      Info();
      void updateOptions(bool get_all_files = false);
      void updateLast(const unsigned int & recv,
                      const std::filesystem::path & path);
  };

  //----------------------------------------------------------------------------
  class Config : public Parameter<config_t> {
    private:
      // keeps track of the created / requested filenames
      std::deque<std::filesystem::path> _file_rotation;

      std::filesystem::path getFolder();
      std::filesystem::path getFilename();
      void registerSavefile(std::filesystem::path & path);

    public:
      Config();
      std::filesystem::path getNewSavefile();
      void cleanLogsFromNonActiveStorage();
      void cleanLogs(const std::string & root_path);
      std::uintmax_t calcMaxPartitionSize();
  };

  //----------------------------------------------------------------------------
} /* namespace wp */
//------------------------------------------------------------------------------
#endif /* SRC_WAGO_PCAP_JSON_WP_PARAMETER_HPP_ */
//---- End of source file ------------------------------------------------------

