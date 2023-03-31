// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>
#include <ostream>
#include <vector>

namespace netconf {

/**
 * Device types used in the netconf environment to classify different types of interfaces.
 */
enum class DeviceType {
  Other,        //!< Undefined interface type
  Ethernet,     //!< Ethernet basic parent interfaces e.g. eth0
  Bridge,       //!< Bridge bridge interfaces e.g. br0,br1,...
  Wwan,         //!< Wwan modem device e.g. wwan0
  Loopback,     //!< Loopback loopback device e.g. lo
  Port,         //!< Port ethernet port interfaces e.g. ethX1, ethX2, ethX11, ethX12
  Dummy,        //!< virtual dummy interface
  Vlan,         //!< vlan interface
};

using DeviceTypes = ::std::vector<DeviceType>;

enum class InterfaceProperty {
  VlanID,
  Link,
};

::std::string ToString(DeviceType type);
DeviceTypes DeviceTypeFromString(const ::std::string& str);
bool DeviceTypeIsValid(const ::std::string& str);

template<typename ... Args>
static constexpr bool DeviceTypeIsAnyOf(DeviceType type, Args ... types) {
  bool match = false;
  (void) std::initializer_list<int> { (match |= (type == (types)),0)... };
  return match;
}

inline ::std::vector<DeviceType> SupportedDeviceTypes() {
  return {DeviceType::Bridge , DeviceType::Dummy , DeviceType::Vlan , DeviceType::Wwan , DeviceType::Port};
}

inline ::std::vector<DeviceType> PersistedDeviceTypes() {
  return {DeviceType::Bridge , DeviceType::Dummy , DeviceType::Vlan};
}

inline ::std::vector<DeviceType> VirtualInterfaceDeviceTypes() {
  return {DeviceType::Dummy , DeviceType::Vlan};
}

constexpr bool IsIpAddressable(DeviceType type){
  return DeviceTypeIsAnyOf(type, DeviceType::Bridge, DeviceType::Wwan, DeviceType::Dummy, DeviceType::Vlan);
}

constexpr bool IsDeletable(DeviceType type){
  return DeviceTypeIsAnyOf(type, DeviceType::Bridge, DeviceType::Dummy, DeviceType::Vlan);
}


}  // namespace netconf

//---- End of header file ------------------------------------------------------
