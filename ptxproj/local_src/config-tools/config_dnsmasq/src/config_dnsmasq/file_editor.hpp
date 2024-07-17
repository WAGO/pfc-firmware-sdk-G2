/// Copyright (c) 2020-2024 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Write data to and read data from files.
///
///  \author   WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef SRC_CONFIG_DNSMASQ_FILE_EDITOR_HPP_
#define SRC_CONFIG_DNSMASQ_FILE_EDITOR_HPP_

#include "file_editor_i.hpp"

namespace configdnsmasq {

class file_editor : public file_editor_i {
 public:
  file_editor() = default;
  ~file_editor() override = default;

  file_editor(const file_editor&) = delete;
  file_editor& operator=(const file_editor&) = delete;
  file_editor(const file_editor&&) = delete;
  file_editor& operator=(const file_editor&&) = delete;

  eStatusCode Read(const ::std::string& file_path, ::std::string& data) const override;
  eStatusCode Write(const ::std::string& file_path, const ::std::string& data) const override;
  eStatusCode Append(const ::std::string& file_path, const ::std::string& data) const override;

};

} // namespace configdnsmasq

#endif // SRC_CONFIG_DNSMASQ_FILE_EDITOR_HPP_
