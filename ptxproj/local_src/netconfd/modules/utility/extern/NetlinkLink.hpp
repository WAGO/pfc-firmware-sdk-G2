// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include "INetlinkLink.hpp"


#include <string>
#include <vector>

namespace netconf {

class NetlinkLink : public INetlinkLink {
 public:
  NetlinkLink() = default;
  ~NetlinkLink() override = default;

  NetlinkLink(const NetlinkLink &other) = delete;
  NetlinkLink& operator=(const NetlinkLink &other) = delete;
  NetlinkLink(NetlinkLink &&other) = delete;
  NetlinkLink& operator=(NetlinkLink &&other) = delete;

  Status AddBridge(const ::std::string &name) override;
  Status AddDummy(const ::std::string &name) override;
  Status AddVLAN(const ::std::string &name, uint16_t id, int link_index) override;

  Status Delete(const ::std::string &name) override;

  void SetInterfaceUp(const ::std::string &name) override;
  void SetInterfaceDown(const ::std::string &name) override;
  void SetMac(const ::std::string &name, const ::std::string& mac) override;

  void DeleteParent(int child_index) override;
  void SetParent(int child_index, int parent_index) override;
  Links GetLinkInfo() override;
  ::std::optional<LinkInfo> GetLinkInfo(const ::std::string &name) override;
  ::std::optional<LinkInfo> GetLinkInfo(int index) override;
  LinkInfo GetLinkInfo(rtnl_link* link) override;

};

} /* namespace netconf */
