//------------------------------------------------------------------------------
/// Copyright (c) 2020 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     defines.h
///
///  \brief    Various defines.
///
///  \author   MSc : WAGO GmbH & Co. KG
///  \author   MOe : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_CONFIG_DNSMASQ_DEFINES_HPP_
#define SRC_CONFIG_DNSMASQ_DEFINES_HPP_

#include <string>

#define SIZEOF_IPADDR   16
#define SIZEOF_MACADDR  18

// States of the domain name check routine.
using conv_state_t = enum
{
  ERR = -1,
  STOP,
  START,
  INLABEL,
  HYPHEN                         // Input character was a hyphen
};

// Main function command codes of config_dnsmasq.
using command_t = enum
{
  NONE = 0,
  GENERATE,
  GET_CONFIG,
  GET_JSON_CONFIG,
  SET_CONFIG,
  RESTORE_CONFIG,
  STORE_CONFIG,
  HELP
};

// Type of service of config_dnsmasq.
using service_t = enum
{
  UNDEF = 0,
  DHCPD,
  DNS
};

// Program configuration data.
using prgconf_t = struct prgconf
{
    command_t command{NONE};            // Function to perform.
    ::std::string dbg_root;             // Root directory for testing and debugging.
    ::std::string services_xml_path;    // Path to network-services.xml.
    ::std::string etchosts;             // Path to /etc/hosts.
    ::std::string dnsmasq_conf;         // Path to /etc/dnsmasq.conf.
    ::std::string dnsmasq_conf_tmp;     // Path to temporary /etc/dnsmasq.conf.
    ::std::string restart_command;      // Path to /etc/init.d/dnsmasq.
    ::std::string lease_file;           // Path to /var/lib/misc/dnsmasq.leases.
    ::std::string selected_port;        // Port selected for getting/setting DHCPD parameters.
    service_t service_type{UNDEF};      // Configuration type DNS or DHCPD.
    bool dnsmasq_restart{true};         // Do not call dnsmasq init script after config setting.
    ::std::string bridge_config;
    ::std::string ip_config;
    ::std::string interface_config;
    ::std::string interface_statuses;
    ::std::string backup_file_path;     //Path to backup file.

    prgconf() :
            dbg_root{""},
            services_xml_path{""},
            etchosts{""},
            dnsmasq_conf{""},
            dnsmasq_conf_tmp{""},
            restart_command{""},
            lease_file{""},
            selected_port{""},
            bridge_config{""},
            ip_config{""},
            interface_config{""},
            interface_statuses{""},
            backup_file_path{""}
    {
    }
};

#endif /* SRC_CONFIG_DNSMASQ_DEFINES_HPP_ */

