// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <functional>
#include <list>

#include "Types.hpp"
#include "NetworkInterfaceConstants.hpp"

namespace netconf {

class IPLink {
 public:

  IPLink(int index, Interface interface, Address address = ZeroIP,
         Netmask netmask = ZeroNetmask, eth::InterfaceLinkState state = eth::InterfaceLinkState::Down);
  ~IPLink() = default;

  IPLink(const IPLink &other) = delete;
  IPLink& operator=(const IPLink &other) = delete;
  IPLink(IPLink &&other) = delete;
  IPLink& operator=(IPLink &&other) = delete;

  void SetIPConfig(const IPConfig& ip_config);
  void SetLinkState(eth::InterfaceLinkState state);
  void SetAddress(const Address &address, const Netmask &netmask);

  ::std::optional<IPConfig> GetIPConfig() const;
  ::std::string GetClientID() const;
  IPConfig GetCurrentIPConfig() const;
  int GetIfIndex() const;
  IPSource GetSource() const;
  eth::InterfaceLinkState GetLinkState() const;
  Interface GetInterface() const;
  Address GetAddress() const;


 private:
  int index_;
  Interface interface_;
  Address address_;
  Netmask netmask_;
  eth::InterfaceLinkState link_state_;
  ::std::optional<IPConfig> ip_config_;
};

using IPLinks = ::std::list<::std::shared_ptr<IPLink>>;
using IPLinkPtr = ::std::shared_ptr<IPLink>;

}  // namespace netconf
