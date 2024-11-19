//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     wp_util.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_COMMON_UTIL_HPP_
#define SRC_COMMON_UTIL_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <ctime>
#include <string>
#include <filesystem>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
std::string UrlStringToString(std::string const& urlStr);
bool ends_with(const std::string & str, const std::string & suffix);
bool canonicalise_file_path(std::string & path);
bool check_allowed_extra_file_path(const std::string & path);

bool create_tmpfs_mount(const std::filesystem::path & mount_path, std::uintmax_t size_bytes);
bool is_mounted_dir(const std::filesystem::path & dir);
std::uintmax_t get_avail_mem();
std::uintmax_t get_free_space_left(const std::string & path);
std::uintmax_t get_file_size(const std::string & path);
std::filesystem::perms get_allowed_permissions();
bool set_owner_group_webserver(const std::string & path);

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* SRC_COMMON_UTIL_HPP_ */
//---- End of source file ------------------------------------------------------

