// SPDX-License-Identifier: GPL-2.0-or-later

#include "NetDev.hpp"

#include "LinkHelper.hpp"
#include "LinkInfo.hpp"
#include "Logger.hpp"

namespace netconf {

namespace {

eth::InterfaceLinkState InterfaceFlagsToState(::std::uint32_t flags) {
  return (flags & IFF_LOWER_UP) == IFF_LOWER_UP ? eth::InterfaceLinkState::Up : eth::InterfaceLinkState::Down;
}

}  // namespace

NetDev::NetDev(LinkInfo attributes) : attributes_{::std::move(attributes)} {
}

Interface NetDev::GetInterface() const {
  return LinkHelper::LinkToInterface(attributes_);
}

::std::string NetDev::GetName() const {
  return attributes_.name_;
}

int NetDev::GetIndex() const {
  return attributes_.index_;
}

int NetDev::GetParentIndex() const {
  return attributes_.parent_index_.value_or(-1);
}

DeviceType NetDev::GetDeviceType() const {
  Interface itf = LinkHelper::LinkToInterface(attributes_);
  return itf.GetType();
}

MacAddress NetDev::GetMac() const {
  return MacAddress::FromString(attributes_.mac_);
}

eth::InterfaceLinkState NetDev::GetLinkState() const {
  return InterfaceFlagsToState(attributes_.flags_);
}

bool NetDev::IsLinkStateUp() const {
  return InterfaceFlagsToState(attributes_.flags_) == eth::InterfaceLinkState::Up;
}

void NetDev::SetLinkInfo(LinkInfo attributes) {
  attributes_ = ::std::move(attributes);
}

LinkInfo NetDev::GetLinkInfo(){
  return attributes_;
}

}  // namespace netconf
