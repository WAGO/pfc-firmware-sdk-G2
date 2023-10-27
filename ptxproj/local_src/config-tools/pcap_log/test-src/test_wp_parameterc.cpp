//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_wp_parameter.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>

#include "../src/wago_pcap/wp_parameterc.hpp"
//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define TEST_JSON_CONFIG_STR R"({"device":"br0","filter":"","maxFilesize":52428800,"maxPacketLen":2048,"maxPartitionSizePct":60,"rotateFiles":true,"storage":"RAM Disk"})"
#define TEST_JSON_INFO_STR R"({"isRunning":false,"lastFSize":0,"lastPid":0,"lastRecv":0,"lastRfshTime":"","optDevices":["br0"],"optDlPaths":[],"optMemCard":false})"

using namespace testing;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------
nlohmann::json g_vaild_json = TEST_JSON_CONFIG_STR;

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
class wp_parameter : public ::testing::Test {
  protected:
    wp::Config config;
    wp::Info info;
    std::string str;
    std::filesystem::path path;
    nlohmann::json json;
    bool retVal = true;
    void SetUp() override {
      path.clear();
      json.clear();
      str.clear();
    }
    void TearDown() override {
      path.clear();
      json.clear();
      str.clear();
    }
};

TEST_F(wp_parameter, jsonSaveToFile_invalid_argument_path_and_json)
{
  retVal = wp::jsonSaveToFile(path, json);
  ASSERT_FALSE(retVal);
}

TEST_F(wp_parameter, jsonSaveToFile_invalid_argument_json)
{
  path = PATH_CONFIG;
  retVal = wp::jsonSaveToFile(path, json);
  ASSERT_FALSE(retVal);
}

TEST_F(wp_parameter, jsonSaveToFile_invalid_argument_path)
{
  json = g_vaild_json;
  retVal = wp::jsonSaveToFile(path, json);
  ASSERT_FALSE(retVal);
}

//------------------------------------------------------------------------------

TEST_F(wp_parameter, jsonLoadFromFile_invalid_argument_path)
{
  json = wp::jsonLoadFromFile(path);
  ASSERT_TRUE(json.empty());
}

//------------------------------------------------------------------------------

TEST_F(wp_parameter, jsonMerge_invalid_argument_str_and_json)
{
  retVal = wp::jsonMerge(str, nullptr);
  ASSERT_FALSE(retVal);
}

TEST_F(wp_parameter, jsonMerge_invalid_argument_json)
{
  str = TEST_JSON_CONFIG_STR;
  retVal = wp::jsonMerge(str, nullptr);
  ASSERT_FALSE(retVal);
}

TEST_F(wp_parameter, jsonMerge_invalid_argument_str)
{
  str = "test";
  retVal = wp::jsonMerge(str, &json);
  ASSERT_FALSE(retVal);
}

//------------------------------------------------------------------------------

TEST_F(wp_parameter, config_parameter_data_default)
{
  ASSERT_STREQ(DFLT_DEVICE, config.data.device.c_str());
  ASSERT_STREQ(DFLT_EMPTY_STR, config.data.filter.c_str());
  ASSERT_EQ(DFLT_STORAGE, config.data.storage);
  ASSERT_EQ(DFLT_MAX_FILE_SIZE, config.data.maxFilesize);
  ASSERT_EQ(DFLT_ROTATE_FILES, config.data.rotateFiles);
}

TEST_F(wp_parameter, info_parameter_data_default)
{
  ASSERT_EQ(false, info.data.isRunning);
  ASSERT_EQ(0, info.data.lastPid);
  ASSERT_EQ(0, info.data.lastRecv);
  ASSERT_EQ(0, info.data.lastFSize);
  ASSERT_TRUE(info.data.optDlPaths.empty());
  ASSERT_STREQ(DFLT_EMPTY_STR, info.data.lastRfshTime.c_str());
  ASSERT_EQ(DFLT_SD, info.data.optMemCard);
  ASSERT_FALSE(info.data.optDevices.empty());
  ASSERT_STREQ(DFLT_DEVICE, info.data.optDevices.front().c_str());
}

TEST_F(wp_parameter, config_parameter_json_default)
{
  ASSERT_STREQ(TEST_JSON_CONFIG_STR, config.json().dump().c_str());
}

TEST_F(wp_parameter, info_parameter_json_default)
{
  ASSERT_STREQ(TEST_JSON_INFO_STR, info.json().dump().c_str());
}

//---- End of source file ------------------------------------------------------

