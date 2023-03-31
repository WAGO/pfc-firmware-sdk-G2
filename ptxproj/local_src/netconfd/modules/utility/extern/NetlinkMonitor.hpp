// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "INetlinkMonitor.hpp"
#include <memory>

namespace netconf
{

  class NetlinkMonitor :public INetlinkMonitor
  {
   public:

    NetlinkMonitor();
    ~NetlinkMonitor() override;

    NetlinkMonitor(const NetlinkMonitor &other) = delete;
    NetlinkMonitor& operator=(const NetlinkMonitor &other) = delete;
    NetlinkMonitor(NetlinkMonitor &&other) = delete;
    NetlinkMonitor& operator=(NetlinkMonitor &&other) = delete;

  protected:
    nl_sock* GetNlSocket() override;
    nl_cache_mngr* GetNlCacheMngr() override;
    void AddCache(::std::shared_ptr<NetlinkCache> cache) override;

   private:
    class Impl;
    ::std::unique_ptr<Impl> pimpl_;

  };


}
