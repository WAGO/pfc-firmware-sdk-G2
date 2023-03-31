// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IBackupRestore.hpp"
#include "IFileEditor.hpp"

namespace netconf {

class RestoreLegacy : public IBackupRestore {
 public:
  explicit RestoreLegacy(IFileEditor& file_editor, uint32_t device_port_count);
  ~RestoreLegacy() override = default;

  RestoreLegacy(const RestoreLegacy &other) = delete;
  RestoreLegacy& operator=(const RestoreLegacy &other) = delete;
  RestoreLegacy(RestoreLegacy &&other) = delete;
  RestoreLegacy& operator=(RestoreLegacy &&other) = delete;

  ::std::uint32_t GetBackupParameterCount() const override {
    return 0;
  }
  Status Backup(const ::std::string& file_path, const ::std::string& data, const ::std::string& data_key, uint32_t version) const override;
  Status Restore(const ::std::string& file_path, ::std::string& backup_network_data, ::std::string& backup_dipswitch_data, uint32_t& version) const override;

 private:

  IFileEditor& file_editor_;
  uint32_t device_port_count_;

  Status GetInterfaceConfigFromAPreviousFirmware(const ::std::string& backup_content, ::std::string& interface_config) const;
  Status GetConfigsFromAPreviousFirmwareBackup(const ::std::string& backup_content, ::std::string& backup_network_data) const;

};

} /* namespace netconf */
