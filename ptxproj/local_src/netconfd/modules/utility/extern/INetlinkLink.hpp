// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>
#include <optional>
#include "BaseTypes.hpp"
#include "LinkInfo.hpp"
#include "Status.hpp"

struct rtnl_link;
namespace netconf {


class INetlinkLink {
 public:
  INetlinkLink() = default;
  virtual ~INetlinkLink() = default;

  INetlinkLink(const INetlinkLink &other) = delete;
  INetlinkLink& operator=(const INetlinkLink &other) = delete;
  INetlinkLink(INetlinkLink &&other) = delete;
  INetlinkLink& operator=(INetlinkLink &&other) = delete;

  virtual Status AddBridge(const ::std::string &name) = 0;
  virtual Status AddDummy(const ::std::string &name) = 0;
  virtual Status AddVLAN(const ::std::string &name, uint16_t id, int link_index) = 0;

  virtual Status Delete(const ::std::string &name) = 0;

  virtual void SetInterfaceUp(const ::std::string &name) = 0;
  virtual void SetInterfaceDown(const ::std::string &name) = 0;
  virtual void SetMac(const ::std::string &name, const ::std::string& mac) = 0;

  virtual void DeleteParent(int child_index) = 0;
  virtual void SetParent(int child_index, int parent_index) = 0;
  virtual Links GetLinkInfo() = 0;
  virtual ::std::optional<LinkInfo> GetLinkInfo(const ::std::string &name) = 0;
  virtual ::std::optional<LinkInfo> GetLinkInfo(int index) = 0;
  virtual LinkInfo GetLinkInfo(rtnl_link* link) = 0;

};

}
