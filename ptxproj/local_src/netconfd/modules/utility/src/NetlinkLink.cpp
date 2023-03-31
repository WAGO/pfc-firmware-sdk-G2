// SPDX-License-Identifier: LGPL-3.0-or-later

#include "NetlinkLink.hpp"

#include <asm-generic/errno-base.h>
#include <linux/if.h>
#include <netlink/errno.h>
#include <netlink/route/link.h>
#include <netlink/route/link/bridge.h>
#include <netlink/route/link/vlan.h>
#include <netlink/socket.h>
#include <netlink/addr.h>
#include <sys/socket.h>

#include <cstdint>
#include <exception>
#include <functional>
#include <memory>
#include <optional>
#include <system_error>
#include <gsl/gsl>

#include "Logger.hpp"

namespace netconf {

using nl_sock_ptr = ::std::unique_ptr<nl_sock, ::std::function<void(nl_sock *)>>;
using rtnl_link_ptr = ::std::unique_ptr<rtnl_link, ::std::function<void(rtnl_link *)>>;
using nl_cache_ptr = ::std::unique_ptr<nl_cache, ::std::function<void(nl_cache *)>>;

using rtnl_link_ptrs = ::std::vector<rtnl_link_ptr>;

namespace {

void LogNetlinkError(int res, const ::std::string& msg) {
  if (res != 0) {
    LogError(msg + " " + ::std::to_string(res) + " " + nl_geterror(res));
  }
}

::std::string ToString(const char *str){
  if (str == nullptr) {
    return "";
  }
  return str;
}

LinkInfo NetlinkToLinkInfo(rtnl_link *link) {

  auto to_optional = [](int index) -> ::std::optional<int> {
    if (index < 1) {
      return std::nullopt;
    }
    return index;
  };

  LinkInfo link_info;
  link_info.index_ = rtnl_link_get_ifindex(link);
  link_info.name_ = ToString(rtnl_link_get_name(link));
  link_info.type_ = ToString(rtnl_link_get_type(link));

  link_info.parent_index_ = to_optional(rtnl_link_get_master(link));
  link_info.index_link_ = to_optional(rtnl_link_get_link(link));

  link_info.flags_ = rtnl_link_get_flags(link);
  link_info.child_type_ = ToString(rtnl_link_get_slave_type(link));

  nl_addr *nladdr = rtnl_link_get_addr(link);
  char buf[40] = { 0 };
  ::gsl::span<char> addr_buffer_(buf);
  char *addr = nl_addr2str(nladdr, addr_buffer_.data(), addr_buffer_.size());
  link_info.mac_ = ToString(addr);

  if (rtnl_link_is_vlan(link) > 0) {
    link_info.vlanid_ = rtnl_link_vlan_get_id(link);
  }

  if (link_info.type_.empty()) {
    if ((link_info.flags_ & IFF_LOOPBACK) != 0) {
      link_info.type_ = "lo";
    }
  }

  return link_info;
}

}

class Netlink {
 public:
  Netlink() {
    auto nl_socket_deleter = [](nl_sock *s) {
      if (s != nullptr) {
        nl_close(s);
        nl_socket_free(s);
      }
    };

    sk_ = { nl_socket_alloc(), nl_socket_deleter };
    nl_connect(sk_.get(), NETLINK_ROUTE);
  }
  ~Netlink() = default;

  Netlink(const Netlink &)             = delete;
  Netlink &operator=(const Netlink &)  = delete;
  Netlink(const Netlink &&)            = delete;
  Netlink &operator=(const Netlink &&) = delete;

  Status AddBridge(const ::std::string& name) {
    Status status;
    auto link = wrap_link(rtnl_link_bridge_alloc());
    rtnl_link_set_name(link.get(), name.c_str());

    if (rtnl_link_add(sk_.get(), link.get(), NLM_F_CREATE) < 0) {
      status = Status { StatusCode::SYSTEM_CALL, "Netlink error add link " + name };
    }
    return status;
  }

  Status AddDummy(const ::std::string& name) {
    Status status;
    auto link = wrap_link(rtnl_link_alloc());
    rtnl_link_set_name(link.get(), name.c_str());

    if (rtnl_link_set_type(link.get(), "dummy") < 0) {
      status = Status { StatusCode::SYSTEM_CALL, "Netlink error set link type dummy" };
    }

    if (rtnl_link_add(sk_.get(), link.get(), NLM_F_CREATE) < 0) {
      status = Status { StatusCode::SYSTEM_CALL, "Netlink error add link " + name };
    }
    return status;
  }

  Status AddVlan(const ::std::string &name, uint16_t id, int link_index) {
    Status status;
    auto link = wrap_link(rtnl_link_vlan_alloc());

    if (not link) {
      status = Status { StatusCode::SYSTEM_CALL, "Netlink error alloc vlan link " + name };
    }

    rtnl_link_set_name(link.get(), name.c_str());
    rtnl_link_set_link(link.get(), link_index);

    if (rtnl_link_vlan_set_id(link.get(), id) < 0) {
      status = Status { StatusCode::SYSTEM_CALL, "Netlink error alloc vlan link " + name };
    }
    if (rtnl_link_add(sk_.get(), link.get(), NLM_F_CREATE) < 0) {
      status = Status { StatusCode::SYSTEM_CALL, "Netlink error add link " + name };
    }
    return status;
  }

  Status Delete(const ::std::string &name) {
    Status status;
    auto link = wrap_link(rtnl_link_alloc());
    rtnl_link_set_name(link.get(), name.c_str());

    if (rtnl_link_delete(sk_.get(), link.get()) < 0) {
      status = Status { StatusCode::SYSTEM_CALL, "Netlink error delete link " + name };
    }
    return status;
  }

  void SetUp(const ::std::string &name) {
    ChangeFlags(name, IFF_UP, rtnl_link_set_flags);
  }

  void SetDown(const ::std::string &name) {
    ChangeFlags(name, IFF_UP, rtnl_link_unset_flags);
  }

  void SetParent(int child_index, int parent_index) {
    int res = rtnl_link_enslave_ifindex(sk_.get(), parent_index, child_index);
    LogNetlinkError(res, "Netlink error set parent failed: child index " + ::std::to_string(child_index) + " parent index: "
            + ::std::to_string(parent_index));
  }

  void ReleaseParent(int child_index) {
    int res = rtnl_link_release_ifindex(sk_.get(), child_index);
    LogNetlinkError(res, "Netlink error release parent failed: child index " + ::std::to_string(child_index));
  }

  void SetMac(const ::std::string &name, const ::std::string &mac) {

    auto link = GetLink(name);
    if (not link) {
      LogError("Netlink error get link failed for: " + name);
    }

    auto link_changes = wrap_link(rtnl_link_alloc());

    nl_addr *addr;
    int res = nl_addr_parse(mac.c_str(), AF_LLC, &addr);
    LogNetlinkError(res, "Netlink error parse mac address for: " + name + " mac: " + mac);

    rtnl_link_set_addr(link_changes.get(), addr);

    res = rtnl_link_change(sk_.get(), link.get(), link_changes.get(), 0);
    LogNetlinkError(res, "Netlink error change: " + name);

    nl_addr_put(addr);
  }

  auto GetLink(const ::std::string &name) -> rtnl_link_ptr {
    rtnl_link *link = nullptr;
    int res = rtnl_link_get_kernel(sk_.get(), 0, name.c_str(), &link);
    if(res != 0){
      LOG_DEBUG("Netlink get link failed for " << name << " interfaces does not exist");
    }
    return wrap_link(link);
  }

  auto GetLink(int index) -> rtnl_link_ptr {
    rtnl_link *link = nullptr;
    int res = rtnl_link_get_kernel(sk_.get(), index, nullptr, &link);
    if(res != 0){
      LOG_DEBUG("Netlink get link failed for index " << ::std::to_string(index) << " interfaces does not exist");
    }
    return wrap_link(link);
  }

  auto GetLinks(Links &links) {
    nl_cache_ptr cache = AllocLinkCache(*sk_.get(), AF_UNSPEC);

    auto func = [](nl_object *obj, void *arg) {
      auto link = reinterpret_cast<rtnl_link*>(obj); //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
      auto list = reinterpret_cast<Links*>(arg); //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
      auto link_info = NetlinkToLinkInfo(link);
      list->emplace_back(link_info);
    };

    nl_cache_foreach(cache.get(), func, &links);
  }

 private:
  nl_sock_ptr sk_;
  rtnl_link_ptr wrap_link(rtnl_link *link) {
    auto rtnl_link_put_deleter = [](rtnl_link *l) {
      if (l != nullptr) {
        rtnl_link_put(l);
      }
    };

    return rtnl_link_ptr { link, rtnl_link_put_deleter };
  }

  nl_cache_ptr AllocLinkCache(nl_sock &socket, int protocol) {
    auto nl_cache_deleter = [](nl_cache *c) {
      if (c != nullptr) {
        nl_cache_free(c);
      }
    };

    nl_cache *cache = nullptr;
    auto res = rtnl_link_alloc_cache(&socket, protocol, &cache);
    LogNetlinkError(res, "Netlink alloc cache failed");

    return nl_cache_ptr { cache, nl_cache_deleter };
  }

  auto ChangeFlags(const ::std::string &name, uint32_t flags, const ::std::function<void(rtnl_link*, uint32_t)>& change) ->
      Status {
    auto link = GetLink(name);
    if (not link) {
      return Status { StatusCode::GENERIC_ERROR, "Netlink error get link " + name };
    }

    auto lc = wrap_link(rtnl_link_alloc());

    change(lc.get(), flags);

    auto res = rtnl_link_change(sk_.get(), link.get(), lc.get(), 0);
    LogNetlinkError(res, "Netlink link change failed");

    return {};
  }
};

namespace {

void GetAdditionalLinkInfos(LinkInfo &link_info) {
  Netlink netlink;
  if (link_info.parent_index_.has_value()) {
    rtnl_link_ptr parent_link = netlink.GetLink(link_info.parent_index_.value());
    if(parent_link){
      link_info.parent_name_ = ToString(rtnl_link_get_name(parent_link.get()));
    }
  }
  if (link_info.index_link_.has_value()) {
    rtnl_link_ptr link_link = netlink.GetLink(link_info.index_link_.value());
    if(link_link){
      link_info.name_link_ = ToString(rtnl_link_get_name(link_link.get()));
    }
  }
}

void GetAdditionalLinkInfos(Links &links) {
  for(auto& link : links){
    GetAdditionalLinkInfos(link);
  }
}

}

Status NetlinkLink::AddDummy(const ::std::string &name) {
  LOG_DEBUG(name);
  Netlink link;
  return link.AddDummy(name);
}
Status NetlinkLink::AddVLAN(const ::std::string &name, uint16_t id, int link_index) {
  LOG_DEBUG(name);
  Netlink link;
  return link.AddVlan(name, id, link_index);
}

Status NetlinkLink::AddBridge(const ::std::string &name) {
  LOG_DEBUG(name);
  Netlink link;
  return link.AddBridge(name);
}

Status NetlinkLink::Delete(const ::std::string &name) {
  LOG_DEBUG(name);
  Netlink link;
  return link.Delete(name);
}

void NetlinkLink::SetInterfaceUp(const ::std::string &name) {
  LOG_DEBUG(name);
  Netlink link;
  link.SetUp(name);
}

void NetlinkLink::SetInterfaceDown(const ::std::string &name) {
  LOG_DEBUG(name);
  Netlink link;
  link.SetDown(name);
}

void NetlinkLink::SetMac(const ::std::string &name, const ::std::string &mac) {
  LOG_DEBUG(name << " mac: " << mac);
  Netlink link;
  link.SetMac(name, mac);
}

void NetlinkLink::DeleteParent(int child_index) {
  LOG_DEBUG(child_index);
  Netlink link;
  link.ReleaseParent(child_index);
}

void NetlinkLink::SetParent(int child_index, int parent_index) {
  LOG_DEBUG("set parent of " << child_index << " to " << parent_index);
  Netlink link;
  link.SetParent(child_index, parent_index);
}

::std::optional<LinkInfo> NetlinkLink::GetLinkInfo(const ::std::string &name) {
  Netlink link;
  rtnl_link_ptr link_ptr = link.GetLink(name);
  if (link_ptr) {
    LinkInfo link_info = NetlinkToLinkInfo(link_ptr.get());
    GetAdditionalLinkInfos(link_info);
    return link_info;
  }
  return ::std::nullopt;
}

::std::optional<LinkInfo> NetlinkLink::GetLinkInfo(int index) {
  Netlink link;
  rtnl_link_ptr link_ptr = link.GetLink(index);
  if (link_ptr) {
    LinkInfo link_info = NetlinkToLinkInfo(link_ptr.get());
    GetAdditionalLinkInfos(link_info);
    return link_info;
  }
  return ::std::nullopt;
}

LinkInfo NetlinkLink::GetLinkInfo(rtnl_link *link) {
  LinkInfo link_info = NetlinkToLinkInfo(link);
  GetAdditionalLinkInfos(link_info);
  return link_info;
}

Links NetlinkLink::GetLinkInfo() {
  Links links;
  Netlink netlink;
  netlink.GetLinks(links);
  GetAdditionalLinkInfos(links);
  return links;
}

} /* namespace netconf */
