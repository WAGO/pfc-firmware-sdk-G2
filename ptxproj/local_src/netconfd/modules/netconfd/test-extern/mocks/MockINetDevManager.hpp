// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "INetDevEvents.hpp"
#include "INetDevManager.hpp"

namespace netconf {

class MockINetDevManager : public INetDevManager {
 public:

  MOCK_METHOD1(RegisterForNetDevConstructionEvents,void(INetDevEvents& netdev_construction));
  MOCK_METHOD1(GetByInterface, ::std::shared_ptr<NetDev>(Interface interface) );
  MOCK_METHOD1(GetByIfIndex, ::std::shared_ptr<NetDev>(::std::int32_t if_index) );
  MOCK_METHOD1(GetByName, ::std::shared_ptr<NetDev>(const ::std::string& name) );
  MOCK_METHOD0(GetNetDevs, NetDevs () );
  MOCK_METHOD1(GetNetDevs, NetDevs (::std::vector<DeviceType>) );
  MOCK_METHOD0(GetBridgesWithAssignetPort, Interfaces () );
  MOCK_METHOD0(GetInterfaces, Interfaces());
  MOCK_METHOD1(GetInterfacesByDeviceType, Interfaces(::std::vector<DeviceType>));
  MOCK_METHOD1(GetParent, ::std::shared_ptr<NetDev>(::std::shared_ptr<NetDev> child) );
  MOCK_METHOD1(GetChildren, NetDevs(::std::shared_ptr<NetDev> parent) );
  MOCK_METHOD1(IsAnyChildUp, bool(::std::shared_ptr<NetDev> parent) );
  MOCK_METHOD1(AddInterface, Status(const Interface& interface) );
  MOCK_METHOD1(AddInterfaces, Status(const Interfaces& interfaces) );
  MOCK_METHOD1(Delete, Status(const Interface& interface) );
  MOCK_METHOD1(Delete, Status(const Interfaces& interfaces) );
  MOCK_METHOD1(SetUp, void(const Interface& interface) );
  MOCK_METHOD1(SetDown, void(const Interface& interface) );
  MOCK_METHOD1(GetPorts, Interfaces(const Interface& bridge) );
  MOCK_METHOD1(BridgePortLeave, void(const Interface& port) );
  MOCK_METHOD2(BridgePortJoin, Status(const Interface& port, const Interface& bridge) );
  MOCK_METHOD2(SetMac, void(::std::shared_ptr<NetDev> netdev, const ::std::string &mac) );
};


}  // namespace netconf
