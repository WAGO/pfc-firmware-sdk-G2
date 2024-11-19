//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     wp_util.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wp_util.hpp"
#include "wp_debug.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <curl/curl.h>
#include <grp.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>
#include <unordered_set>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define ALLOWED_FILE_PARENT_DIR "/var/tmp/"
#define ALLOWED_FILE_EXTENSION_LOG ".log"
#define ALLOWED_FILE_EXTENSION_PCAP ".pcap"
#define ALLOWED_FILE_EXTENSION_PCAPNG ".pcapng"

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------
std::unordered_set<std::string> allowed_extensions = {
  ALLOWED_FILE_EXTENSION_LOG,
  ALLOWED_FILE_EXTENSION_PCAP,
  ALLOWED_FILE_EXTENSION_PCAPNG
};

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
std::string UrlStringToString(std::string const& urlStr)
{
  std::string result;

  CURL* curlCtx = curl_easy_init();
  if(nullptr != curlCtx)
  {
    char* pDecoded = curl_easy_unescape(curlCtx, urlStr.c_str(), 0, nullptr);
    if(nullptr != pDecoded)
    {
      result = pDecoded;
      curl_free(pDecoded);
    }
    curl_easy_cleanup(curlCtx);
  }

  return result;
}

bool ends_with(const std::string & str, const std::string & suffix)
{
  return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool canonicalise_file_path(std::string & path)
{
  try
  {
    std::filesystem::path file_path = std::filesystem::canonical(path);
    path = file_path.string();
    return true;
  }
  catch (const std::filesystem::filesystem_error &)
  {
    Debug_Printf("Path \"%s\" not found.\n", path.c_str());
  }

  return false;
}

bool check_allowed_extra_file_path(const std::string & path)
{
  if (path.compare(0, std::string(ALLOWED_FILE_PARENT_DIR).length(), ALLOWED_FILE_PARENT_DIR) == 0)
  {
    // starts with parent dir: ok
    if (allowed_extensions.find(std::filesystem::path(path).extension()) != allowed_extensions.end())
    {
      // ends with extension: ok
      return true;
    }
  }

  return false;
}

bool create_tmpfs_mount(const std::filesystem::path & mount_path, std::uintmax_t size_bytes)
{
  if (!std::filesystem::exists(mount_path) && !std::filesystem::create_directories(mount_path))
  {
    return false;
  }

  if (is_mounted_dir(mount_path))
  {
    // is already mounted, unmount first
    if (umount(mount_path.c_str()) != 0)
    {
      Debug_Printf(("Failed to unmount " + mount_path.string() + "\n").c_str());
      return false;
    }
  }

  std::ostringstream mount_opts;
  mount_opts << "mode=0755,uid=0,gid=0,size=" << size_bytes;

  Debug_Printf(("Attempting to mount tmpfs at " + mount_path.string() + " with opts: " + mount_opts.str() + "\n").c_str());
  int result = mount("tmpfs", mount_path.c_str(), "tmpfs", 0, mount_opts.str().c_str());
  Debug_Printf("Mount result: %d, errno: %d\n", result, errno);

  return result == 0;
}

bool is_mounted_dir(const std::filesystem::path & dir)
{
  if (!std::filesystem::exists(dir))
  {
    return false;
  }

  const auto stripped_dir = std::filesystem::canonical(dir);
  if (!std::filesystem::exists(stripped_dir) ||
      !std::filesystem::is_directory(stripped_dir) ||
      !stripped_dir.has_parent_path())
  {
    return false;
  }

  struct stat dir_stat;
  struct stat dir_parent_stat;
  stat(stripped_dir.c_str(), &dir_stat);
  stat(stripped_dir.parent_path().c_str(), &dir_parent_stat);

  Debug_Printf("dir [%s] dev num: %llu; parent [%s] dev num: %llu\n",
               stripped_dir.c_str(),
               dir_stat.st_dev,
               stripped_dir.parent_path().c_str(),
               dir_parent_stat.st_dev);

  // compare device numbers of dir and its parent, if they differ it is a mountpoint
  return dir_stat.st_dev != dir_parent_stat.st_dev;
}

std::uintmax_t get_avail_mem()
{
  std::string key;
  std::ifstream file("/proc/meminfo");

  while (file.is_open() && file >> key)
  {
    if (key == "MemAvailable:")
    {
      std::uintmax_t result;
      if (file >> result)
      {
        return result * 1024; // value is in kB
      }
      break;
    }

    // ignore everything else on this line
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }

  return 0;
}

std::uintmax_t get_free_space_left(const std::string & path)
{
  try
  {
    auto space_info = std::filesystem::space(path);
    return space_info.available;
  }
  catch(const std::filesystem::filesystem_error &)
  {
  }

  return 0;
}

std::uintmax_t get_file_size(const std::string & path)
{
  try
  {
    return std::filesystem::file_size(path);
  }
  catch(const std::filesystem::filesystem_error &)
  {
  }

  return 0;
}

std::filesystem::perms get_allowed_permissions()
{
  return std::filesystem::perms::owner_read | std::filesystem::perms::owner_write | std::filesystem::perms::group_read;
}

bool set_owner_group_webserver(const std::string & path)
{
  group * grp = getgrnam("www");
  if (grp != nullptr)
  {
    return chown(path.c_str(), (uid_t)-1, grp->gr_gid) == 0;
  }

  return false;
}

//---- End of source file ------------------------------------------------------

