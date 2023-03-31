// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

#include "IDeviceTypeLabel.hpp"
#include "IEventManager.hpp"
#include "IInterfaceMonitor.hpp"
#include "IMacDistributor.hpp"
#include "INetDevEvents.hpp"
#include "INetDevManager.hpp"
#include "INetlinkLink.hpp"
#include "NetDev.hpp"

namespace netconf {

class NetDevManager : public INetDevManager, public IInterfaceEvent {
 public:
  NetDevManager(::std::shared_ptr<IInterfaceMonitor> interface_monitor, IEventManager &event_manager,
                INetlinkLink &netlink);
  NetDevPtr GetByInterface(Interface interface) override;
  NetDevPtr GetByIfIndex(::std::int32_t if_index) override;
  NetDevPtr GetByName(const ::std::string &name) override;
  NetDevs GetNetDevs() override;
  NetDevs GetNetDevs(::std::vector<DeviceType> types) override;
  Interfaces GetInterfaces() override;
  Interfaces GetInterfacesByDeviceType(::std::vector<DeviceType> types) override;
  Interfaces GetBridgesWithAssignetPort() override;
  NetDevPtr GetParent(NetDevPtr child) override;
  NetDevs GetChildren(NetDevPtr parent) override;
  bool IsAnyChildUp(NetDevPtr parent) override;

  Status AddInterface(const Interface &interface) override;
  Status AddInterfaces(const Interfaces &interfaces) override;
  Status Delete(const Interface &interface) override;
  Status Delete(const Interfaces &interfaces) override;
  void SetUp(const Interface &interface) override;
  void SetDown(const Interface &interface) override;
  Interfaces GetPorts(const Interface &interface) override;
  void BridgePortLeave(const Interface &port) override;
  Status BridgePortJoin(const Interface &port, const Interface &bridge) override;
  void SetMac(NetDevPtr netdev, const ::std::string &mac) override;

  void LinkChange(LinkInfo new_link, InterfaceEventAction action) override;
  void RegisterForNetDevConstructionEvents(INetDevEvents &netdev_construction) override;

  static bool ExistsByInterface(Interface interface, const NetDevs &netdevs);
  static bool DoesNotExistByInterface(Interface interface, const NetDevs &netdevs);

 private:
  NetDevPtr UpdateOrCreateNetdev(LinkInfo &link_info);
  void Delete(const NetDevPtr& netdev);

  void OnNetDevCreated(NetDevPtr netdev) const;
  void OnNetDevRemoved(NetDevPtr netdev) const;
  void OnBridgePortsChange(NetDevPtr netdev) const;

  Status NetlinkAddVLAN(const Interface &interface);

  ::std::shared_ptr<IInterfaceMonitor> interface_monitor_;
  ::std::vector<::std::shared_ptr<NetDev>> net_devs_;
  INetDevEvents *netdev_construction_;
  INetlinkLink &netlink_;
  IEventManager &event_manager_;
};

}  // namespace netconf
