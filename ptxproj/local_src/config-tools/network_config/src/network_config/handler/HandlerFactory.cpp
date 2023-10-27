// SPDX-License-Identifier: GPL-2.0-or-later

#include "HandlerFactory.hpp"

#include <ReloadHostConfHandler.hpp>
#include <memory>

#include "BackupRestoreHandler.hpp"
#include "BridgeConfigHandler.hpp"
#include "DSAModeHandler.hpp"
#include "DeviceInfoHandler.hpp"
#include "DipSwitchHandler.hpp"
#include "DynamicIPEventHandler.hpp"
#include "FixIpHandler.hpp"
#include "IPConfigHandler.hpp"
#include "InterfaceConfigHandler.hpp"
#include "InterfaceStatusHandler.hpp"
#include "MacAddressHandler.hpp"
#include "OptionStrings.hpp"
#include "InterfaceHandler.hpp"

namespace network_config {

::std::unique_ptr<IHandler> HandlerFactory::CreateHandler(const OptionParser &parser) {
  const auto &opts = GetOptions();
  const auto &map  = parser.GetVariableMap();

  if (parser.IsSet(opts.bridge_config.name)) {
    return ::std::make_unique<BridgeConfigHandler>(map);
  }
  if (parser.IsSet(opts.ip_config.name)) {
    return ::std::make_unique<IPConfigHandler>(parser);
  }
  if (parser.IsSet(opts.mac_address.name)) {
    return ::std::make_unique<MacAddressHandler>(map);
  }
  if (parser.IsSet(opts.interface_config.name)) {
    return ::std::make_unique<InterfaceConfigHandler>(map);
  }
  if (parser.IsSet(opts.device_info.name)) {
    return ::std::make_unique<DeviceInfoHandler>(map);
  }
  if (parser.IsSet(opts.interface_status.name)) {
    return ::std::make_unique<InterfaceStatusHandler>(map);
  }
  if (parser.IsSet(opts.interface.name)) {
    return ::std::make_unique<InterfaceHandler>(map);
  }
  if (parser.IsSet(opts.backup.name) || parser.IsSet(opts.restore.name) ||
             parser.IsSet(opts.get_backup_parameter_count.name)) {
    return ::std::make_unique<BackupRestoreHandler>(map);
  }
  if (parser.IsSet(opts.dsa_mode.name)) {
    return ::std::make_unique<DSAModeHandler>(map);
  }
  if (parser.IsSet(opts.fix_ip.name)) {
    return ::std::make_unique<FixIpHandler>();
  }
  if (parser.IsSet(opts.dip_switch_config.name)) {
    return ::std::make_unique<DipSwitchHandler>(map);
  }
  if (parser.IsSet(opts.dynamic_ip_event.name)) {
    return ::std::make_unique<DynamicIPEventHandler>(map);
  }
  if (parser.IsSet(opts.reload_host_conf.name)) {
    return ::std::make_unique<ReloadHostConfHandler>();
  }

  return nullptr;
}

} /* namespace network_config */
