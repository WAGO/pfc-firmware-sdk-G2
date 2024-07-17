//------------------------------------------------------------------------------
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
///  \brief    Collection of utility functions.
///
///  \author   MSc : WAGO GmbH & Co. KG
///  \author   MOe : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef SRC_CONFIG_DNSMASQ_UTILITIES_HPP_
#define SRC_CONFIG_DNSMASQ_UTILITIES_HPP_

#include <ct_error_handling.h>
#include <string>
#include <vector>

namespace configdnsmasq {

using dhcpd_host_t = struct dhcpd_host {
  ::std::string mac{};
  uint32_t ip_net_part = 0;
  uint32_t ip_host_part = 0;
};

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/**
 * Convert IP-Address from dotted ASCII string to uint32_t.
 * Did not use inet_aton() in favour of more strict format checks.
 * Set result to NULL for checking only.
 *
 * @param address
 * @return
 */
eStatusCode conv_ip_ascii2bin(uint32_t &bin_address, const ::std::string &address);
bool is_valid_ip_string (const ::std::string &address);
void conv_ip_bin2ascii(::std::string& address, uint32_t bin_address);

/**
 * Converts a string of the form mac_addr-ip_addr,... into an array of structures of type dhcpd_host_t
 *
 * @param hostsstr
 * @param netmask
 * @param hosts
 * @return status
 */
eStatusCode conv_hosts_ascii2bin(const ::std::string &hostsstr, uint32_t netmask, ::std::vector<dhcpd_host_t> &hosts);
void conv_hosts_bin2ascii(::std::string& hosts, const ::std::vector<dhcpd_host_t> &host_list);
bool conv_is_valid_domain_name(const ::std::string &name);
bool conv_is_valid_mac(const ::std::string &mac_in);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

} // namespace configdnsmasq
#endif // SRC_CONFIG_DNSMASQ_UTILITIES_HPP_
