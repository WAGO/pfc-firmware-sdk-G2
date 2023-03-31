// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

struct nl_sock;
struct nl_cache_mngr;

namespace netconf {


class NetlinkCache {
 public:
  NetlinkCache()                    = default;
  virtual ~NetlinkCache()           = default;

  NetlinkCache(const NetlinkCache &other) = delete;
  NetlinkCache& operator=(const NetlinkCache &other) = delete;
  NetlinkCache(NetlinkCache &&other) = delete;
  NetlinkCache& operator=(NetlinkCache &&other) = delete;

  virtual void Resync(nl_sock*) = 0;

};

}  // namespace netconf
