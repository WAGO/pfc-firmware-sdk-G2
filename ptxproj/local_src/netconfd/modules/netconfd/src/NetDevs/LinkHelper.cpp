// SPDX-License-Identifier: LGPL-3.0-or-later

#include "LinkHelper.hpp"

#include <boost/algorithm/string.hpp>
#include "Logger.hpp"

namespace netconf {

namespace {

DeviceType GetDeviceTypeOf(const LinkInfo &link_info) {
  if (link_info.type_ == "lo") {
    return DeviceType::Loopback;
  }
  if (link_info.type_ == "dummy") {
    return DeviceType::Dummy;
  }
  if (link_info.type_ == "vlan") {
    return DeviceType::Vlan;
  }
  if (link_info.type_ == "bridge") {
    return DeviceType::Bridge;
  }

  // Info: link type is undefined for ethX (show types list via: ip link help)
  if (boost::starts_with(link_info.name_, "ethX")) {
    return DeviceType::Port;
  }
  if (boost::starts_with(link_info.name_, "eth")) {
    return DeviceType::Ethernet;
  }

  if (boost::starts_with(link_info.name_, "wwan")) {
    return DeviceType::Wwan;
  }

  return DeviceType::Other;
}

}  // namespace

Interface LinkHelper::LinkToInterface(const LinkInfo &link_info) {
  Interface itf   = Interface::CreateOther(link_info.name_);
  DeviceType type = GetDeviceTypeOf(link_info);
  switch (type) {
    case DeviceType::Loopback:
      itf = Interface::CreateLoopback(link_info.name_);
      break;
    case DeviceType::Port:
      itf = Interface::CreatePort(link_info.name_);
      break;
    case DeviceType::Bridge:
      itf = Interface::CreateBridge(link_info.name_);
      break;
    case DeviceType::Vlan:
      if(link_info.vlanid_.has_value() && link_info.name_link_.has_value()){
        itf = Interface::CreateVLAN(link_info.name_, link_info.vlanid_.value(),
                                    link_info.name_link_.value());
      }else{
        LogError("LinkToInterface failed vlanid: " + ::std::to_string(link_info.vlanid_.value_or(0)) + " parent interface: "
                 + link_info.parent_name_.value_or("EMPTY"));
      }
      break;
    case DeviceType::Dummy:
      itf = Interface::CreateDummy(link_info.name_);
      break;
    case DeviceType::Ethernet:
      itf = Interface::CreateEthernet(link_info.name_);
      break;
    case DeviceType::Wwan:
      itf = Interface::CreateWwan(link_info.name_);
      break;
    default:
      break;
  }
  return itf;
}

} /* namespace netconf */
