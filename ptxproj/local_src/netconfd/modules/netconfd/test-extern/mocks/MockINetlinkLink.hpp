// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "INetlinkLink.hpp"

namespace netconf {

class MockINetlinkLink : public INetlinkLink {
 public:
  MOCK_METHOD1(AddBridge, Status(const ::std::string& name));
  MOCK_METHOD1(AddDummy, Status(const ::std::string& name));
  MOCK_METHOD3(AddVLAN, Status(const ::std::string& name, uint16_t id, int link_index));
  MOCK_METHOD1(Delete, Status(const ::std::string& name));
  MOCK_METHOD1(SetInterfaceUp, void(const ::std::string& name));
  MOCK_METHOD1(SetInterfaceDown, void(const ::std::string& name));
  MOCK_METHOD2(SetMac, void(const ::std::string& name, const ::std::string& mac));
  MOCK_METHOD1(DeleteParent, void(int child_index));
  MOCK_METHOD0(GetLinkInfo, Links());
  MOCK_METHOD2(SetParent, void(int child_index, int parent_index));
  MOCK_METHOD1(GetLinkInfo, ::std::optional<LinkInfo>(const ::std::string& name));
  MOCK_METHOD1(GetLinkInfo, ::std::optional<LinkInfo>(int index));
  MOCK_METHOD1(GetLinkInfo, LinkInfo(rtnl_link* link));
};

}  // namespace netconf
