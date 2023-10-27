// SPDX-License-Identifier: GPL-2.0-or-later

#include "GratuitousArp.hpp"
#include "IPLink.hpp"
#include "IIPConfigure.hpp"
#include "NetworkInterfaceConstants.hpp"
#include "boost/algorithm/string.hpp"
#include "Logger.hpp"

namespace netconf {

IPLink::IPLink(int index, Interface interface, Address address,
               Netmask netmask, eth::InterfaceLinkState state)
    : index_ { index },
      interface_ { ::std::move(interface) },
      address_ { ::std::move(address) },
      netmask_ { ::std::move(netmask) },
      link_state_ { state }{
}


void IPLink::SetIPConfig(const IPConfig& ip_config) {
  ip_config_ = ip_config;
}

[[gnu::pure]]
int IPLink::GetIfIndex() const {
  return index_;
}

::std::optional<IPConfig> IPLink::GetIPConfig() const {
  return ip_config_;
}

Interface IPLink::GetInterface() const {
  return interface_;
}

IPConfig IPLink::GetCurrentIPConfig() const {
  IPConfig config{interface_.GetName(), IPSource::NONE};
  if(ip_config_.has_value()){
    config = ip_config_.value();
  }
  config.address_ = address_;
  config.netmask_ = netmask_;
  return config;
}

Address IPLink::GetAddress() const {
  return address_;
}

IPSource IPLink::GetSource() const {
  return ip_config_.has_value() ? ip_config_.value().source_ : IPSource::NONE;
}

eth::InterfaceLinkState IPLink::GetLinkState() const {
  return link_state_;
}

::std::string IPLink::GetClientID() const {
  if(ip_config_.has_value()){
    return ip_config_.value().dhcp_client_id_;
  }
  return "";
}

void IPLink::SetAddress(const Address &address, const Netmask &netmask) {
  address_ = address;
  netmask_ = netmask;
}

void IPLink::SetLinkState(eth::InterfaceLinkState state) {
  link_state_ = state;
}

}
