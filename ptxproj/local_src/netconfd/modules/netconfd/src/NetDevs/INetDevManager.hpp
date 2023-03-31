// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <vector>
#include <string>

#include "BaseTypes.hpp"
#include "Types.hpp"
#include "NetDev.hpp"


namespace netconf {

class INetDevEvents;

class INetDevManager {
 public:
  INetDevManager() = default;
  virtual ~INetDevManager() = default;

  INetDevManager(const INetDevManager&) = delete;
  INetDevManager& operator=(const INetDevManager&) = delete;
  INetDevManager(const INetDevManager&&) = delete;
  INetDevManager& operator=(const INetDevManager&&) = delete;

  virtual void RegisterForNetDevConstructionEvents(INetDevEvents& netdev_construction) = 0;
  virtual NetDevPtr GetByInterface(Interface interface) = 0;
  virtual NetDevPtr GetByIfIndex(::std::int32_t if_index) = 0;
  virtual NetDevPtr GetByName(const ::std::string& name) = 0;
  virtual NetDevs GetNetDevs()= 0;
  virtual NetDevs GetNetDevs(::std::vector<DeviceType> types) = 0;
  virtual Interfaces GetInterfaces() = 0;
  virtual Interfaces GetInterfacesByDeviceType(::std::vector<DeviceType> types) = 0;
  virtual Interfaces GetBridgesWithAssignetPort() = 0;
  virtual NetDevPtr GetParent(NetDevPtr child) = 0;
  virtual NetDevs GetChildren(NetDevPtr parent) = 0;
  virtual bool IsAnyChildUp(NetDevPtr parent) = 0;
  virtual void SetMac(NetDevPtr netdev, const ::std::string &mac) = 0;

  virtual Status AddInterface(const Interface& interface) = 0;
  virtual Status AddInterfaces(const Interfaces& interfaces) = 0;
  virtual Status Delete(const Interface& interface) = 0;
  virtual Status Delete(const Interfaces& interfaces) = 0;
  virtual void SetUp(const Interface& interface) = 0;
  virtual void SetDown(const Interface& interface) = 0;
  virtual Interfaces GetPorts(const Interface& bridge) = 0;
  virtual void BridgePortLeave(const Interface& port) = 0;
  virtual Status BridgePortJoin(const Interface& port, const Interface& bridge) = 0;

};

} /* namespace netconf */
