// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

namespace network_config {

struct Option {
  const char* name = nullptr;
  const char* description = nullptr;
  const char* parameter = nullptr;
};

/**
 * Main program option names and description
 */
struct Options {
  // @formatter:off
  const Option bridge_config
    { "bridge-config", "Set or get bridge configuration." };
  const Option interface_config
    { "interface-config", "Set or get interface configuration." };
  const Option ip_config
    { "ip-config", "Set or get ip configuration." };
  const Option interface
    { "interface", "Add or delete an interface of type dummy or vlan." };
  const Option mac_address
    { "mac-address", "Get mac address of an device. The device name must be given using the --device option." };
  const Option device_info
    { "device-info", "Get information about ports and/or bridges." };
  const Option interface_status
    { "interface-status", "Get information about ports states." };
  const Option backup_targetversion
    { "backup-targetversion", "Backup made for the specific firmware version [targetversion], e.g. V03.03.02" };
  const Option backup
    { "backup", "Backup network settings to given <file path>." };
  const Option restore
    { "restore", "Restore network settings from given <file path>." };
  const Option get_backup_parameter_count
    { "get-backup-parameter-count", "Get number of backup parameters." };
  const Option dsa_mode
    { "dsa-mode", "Set or Get dsa-mode configuration." };
  const Option fix_ip
    { "fix-ip", "Set fixed ip (192.168.1.17)" };
  const Option dip_switch_config
    { "dip-switch-config", "Set or get DIP switch configuration. Caution: mode and last address byte cannot be changed." };
  const Option dynamic_ip_event
    { "dynamic-ip-event", "Propagate a dynamic ip event. e.g. (UDHCP event: bound, renew, ...)" };
  const Option reload_host_conf
    { "reload-host-conf", "Trigger reload of /etc/host.conf" };

  // Operations
  const Option get
    { "get", "Get configuration; Optionally specify a value to get only part of the configuration. The \"value\" can only be used together with the --mac-address option." };
  const Option get_current
    { "get-current", "Get a current configuration" };
  const Option set
    { "set", "Set a configuration with the given value" };
  const Option add
    { "add", R"(Add an interfaces of type dummy or vlan, e.g. {"name": "dummy0", "type": "dummy"} or {"name": "vlan1", "type": "vlan", "link": "br1", "vlanid": 100})" };
  const Option del
    { "del", R"(Delete an interfaces of type dummy or vlan, e.g. {"name": "dummy0"})" };
  const Option filter
    {"filter", "Displays data of configuration only from given device type. Usable in 'get' operations. <brigde|wwan|port|dummy|vlan>" };

  // Format options
  const Option format
    { "format,f", "Choose how to format the output. <text|json|pretty-json>" };

  // Miscellaneous options
  const Option help
    { "help", "Print usage information about this tool" };
  const Option quiet
    { "quiet", "Suppress any outout to the console" };
    // note:
    // letting the caller (i.e. wbm) write to arbitrary locations as root opens the door for all kinds of security issues
    // as a workaround: keep this option but ignore its parameter and only write to /tmp/last_error.txt
  const Option error_msg_dst
    { "error-msg-dst", "deprecated: is replaced with write-last-error option", "<ignored>" };
  const Option write_last_error
    { "write-last-error", "write error messages to /tmp/last_error.txt"};
  const Option dryrun
    { "dryrun", "do not actually write anything just validate the request" };
  // @formatter:on
};

/**
 * Option field names and description
 */
struct Fields {
  // @formatter:off
  const Option field
      { "field","Selects a specific field of the data set, e.g. ipaddr", "<name>" };
  const Option device
    { "device", "Device name <br0|wwan0|..>", "<value>" };  
  const Option backup_version
    { "backup-targetversion", "Target version, e.g. V03.03.02" ,"<value>"};
  const Option action
      { "action", "action <bound|renew|release|nak>" ,"<value>"};
  // @formatter:on
};

const Options& GetOptions();
const Fields& GetFields();
}  // namespace netconf
