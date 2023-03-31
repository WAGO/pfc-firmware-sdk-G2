// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include "NetlinkCache.hpp"

#include <memory>

#include "IIPMonitor.hpp"

namespace netconf {

class NetlinkAddressCache : public NetlinkCache, public IIPMonitor {
 public:
  NetlinkAddressCache() = delete;
  NetlinkAddressCache(nl_sock* nl_sock, nl_cache_mngr* nl_cache_mgr);
  ~NetlinkAddressCache() override;

  NetlinkAddressCache(const NetlinkAddressCache &other) = delete;
  NetlinkAddressCache& operator=(const NetlinkAddressCache &other) = delete;
  NetlinkAddressCache(NetlinkAddressCache &&other) = delete;
  NetlinkAddressCache& operator=(NetlinkAddressCache &&other) = delete;

  void Resync(nl_sock* nl_sock) override;

  Address GetIPAddress(int if_index) override;
  Netmask GetNetmask(int if_index) override;

  void RegisterEventHandler(IIPEvent& event_handler) override;
  void UnregisterEventHandler(IIPEvent& event_handler) override;

 private:
  class Impl;
  ::std::unique_ptr<Impl> impl_;
};

}  // namespace netconf
