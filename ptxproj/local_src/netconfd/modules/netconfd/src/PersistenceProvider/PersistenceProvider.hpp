// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "FileEditor.hpp"
#include "BackupRestore.hpp"
#include "DipSwitch.hpp"
#include "RestoreLegacy.hpp"
#include "PersistenceExecutor.hpp"
#include "IPersistence.hpp"

namespace netconf {

class PersistenceProvider : public IPersistenceProvider, public IPersistence<InterfaceConfigs> {
 public:
  PersistenceProvider(const ::std::string &persistence_path, DipSwitch &dip_switch, uint32_t device_port_count);
  ~PersistenceProvider() override = default;

  PersistenceProvider(const PersistenceProvider&) = delete;
  PersistenceProvider& operator=(const PersistenceProvider&) = delete;
  PersistenceProvider(const PersistenceProvider&&) = delete;
  PersistenceProvider& operator=(const PersistenceProvider&&) = delete;

  Status Write(const BridgeConfig& config) override;
  Status Read(BridgeConfig& config) override;

  Status Write(const IPConfigs& configs) override;
  Status Read(IPConfigs& configs) override;

  Status Write(const DipSwitchIpConfig& config) override;
  Status Read(DipSwitchIpConfig& config) override;

  Status Write(const Interfaces& config) override;
  void Read(Interfaces& config) override;

  Status Read(BridgeConfig& config, IPConfigs& configs, Interfaces& interfaces) override;

  Status Read(InterfaceConfigs& port_configs) override;
  Status Write(const InterfaceConfigs& port_configs) override;

  Status Backup(const std::string &file_path,
                const std::string &targetversion) override;
  Status Restore(const ::std::string &file_path,
                 BridgeConfig &bridge_config,
                 IPConfigs &ip_configs,
                 InterfaceConfigs &interface_configs,
                 DipSwitchIpConfig &dip_switch_config,
                 Interfaces &interfaces) override;
  uint32_t GetBackupParameterCount() const override;

 private:

  FileEditor file_editor_;
  BackupRestore backup_restore_;
  RestoreLegacy restore_legacy_;
  PersistenceExecutor persistence_executor_;

};

} /* namespace netconf */
