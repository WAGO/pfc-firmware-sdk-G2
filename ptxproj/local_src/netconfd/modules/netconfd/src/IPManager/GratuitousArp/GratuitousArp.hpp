// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IGratuitousArp.hpp"
#include "MacAddress.hpp"

namespace netconf {

class GratuitousArp : public IGratuitousArp{

 public:
  GratuitousArp() = default;
  ~GratuitousArp() override = default;

  GratuitousArp(const GratuitousArp &other) = delete;
  GratuitousArp(GratuitousArp &&other) = delete;
  GratuitousArp& operator=(const GratuitousArp &other) = delete;
  GratuitousArp& operator=(GratuitousArp &&other) = delete;

  void SendGratuitousArpOnPort(NetDevPtr port_netdev, NetDevPtr bridge_netdev, Address address) const override;
  void SendGratuitousArpOnBridge(NetDevPtr bridge_netdev, Address address) const override;
  void EnableGratuitousArp(IPLinkPtr ip_link) const override;

 private:
  Status Send(const Address& ip_address, const NetDevPtr& bridge_netdev, const NetDevPtr& port_netdev) const;

};

}
