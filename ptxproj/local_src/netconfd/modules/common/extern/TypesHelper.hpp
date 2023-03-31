// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>
#include <optional>
#include "Types.hpp"

namespace netconf {

bool IsEmptyOrZeroIP(const ::std::string& address);

::std::string IPSourceToString(IPSource value);
IPSource StringToIPSource(const ::std::string& value);

bool IsEqual(const BridgeConfig& a, const BridgeConfig& b);
bool IsEqual(IPConfigs& lhs, IPConfigs& rhs);

void AddIPConfig(const IPConfig& ip_config, IPConfigs& ip_configs);

Interfaces GetBridgesOfBridgeConfig(const BridgeConfig& bridge_config);

bool ConfigContainsBridgeWithSpecificInterfaces(const BridgeConfig& bridge_config, const Interface& bridge,
                                                const Interfaces& interfaces);

void IpConfigsIntersection(IPConfigs& ip_configs, const Interfaces& selection);

IPConfig& ComplementNetmask(IPConfig& ip_config);
IPConfigs& ComplementNetmasks(IPConfigs& ip_configs);

::std::string IPConfigToString(const IPConfig& config);

void RemoveUnnecessaryIPParameter(IPConfigs &ip_configs);

/**
 * Increment the number representation of the given IP address string by the given increment value.
 * @param address The IP address to be incremented.
 * @param increment The increment value
 * @return The incremented address
 */
Address IpAddressV4Increment(const Address& address, uint32_t increment = 1);

/**
 * Extract the interface index from an interface name.
 * The name must start with characters followed by any number, e.g. eth0, br2, wwan5, myif42
 *
 * @return The index part of the interface name as optional integer.
 */
std::optional<uint32_t> ExtractInterfaceIndex(const ::std::string& interface);

/**
 * Get an iterator to an IPConfig in the IPConfigs matching the desired interface name.
 * @return an interator to the desired IPConfig, or end iterator.
 */
IPConfigs::iterator GetIpConfigByInterface(IPConfigs& configs, const Interface& interface);

/**
 * Get the set difference of A \ B by their interface names.
 * @param a IP config A
 * @param b IP config A
 * @return new IP configs with A \ B
 */
IPConfigs GetIpConfigsDifferenceByInterface(const IPConfigs& a, const IPConfigs& b);

/**
 * Sort the IP config based on the interface name
 * @param a The IP config to sort
 */
void SortByInterface(IPConfigs& a);

}
