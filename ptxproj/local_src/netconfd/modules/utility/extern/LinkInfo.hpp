// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace netconf {

struct LinkInfo {
  int32_t index_  = 0;
  ::std::string name_ = "";
  ::std::string type_ = "";  // interface type (show types list via: ip link help)
  ::std::string mac_ = "00:00:00:00:00:00";
  uint32_t flags_ = 0;
  ::std::optional<int32_t> parent_index_ = ::std::nullopt;  // interface index of parent (e.g. bridge)
  ::std::optional<::std::string> parent_name_ = ::std::nullopt;
  ::std::optional<int32_t> index_link_ = ::std::nullopt;  // interface index of link (e.g. vlan base interface)
  ::std::optional<::std::string> name_link_ = ::std::nullopt;
  ::std::optional<::std::string> child_type_ = ::std::nullopt;  // interface child type (e.g. bridge)
  ::std::optional<int> vlanid_ = ::std::nullopt;  // vlan id

  bool operator==(const LinkInfo &rhs){
    return (index_ == rhs.index_) && (name_ == rhs.name_) && (type_ == rhs.type_) && (mac_ == rhs.mac_) && (flags_ == rhs.flags_)
        && (parent_index_.value_or(0) == rhs.parent_index_.value_or(0))
            && (parent_name_.value_or("") == rhs.parent_name_.value_or(""))
            && (index_link_.value_or(0) == rhs.index_link_.value_or(0))
            && (name_link_.value_or("") == rhs.name_link_.value_or(""))
            && (child_type_.value_or("") == rhs.child_type_.value_or(""))
            && (vlanid_.value_or(0) == rhs.vlanid_.value_or(0));
  }

  explicit operator ::std::string() const {
    return ToString();
  }

  ::std::string ToString() const {
    return "LinkInfo: index: " + ::std::to_string(index_) + "name " + name_ + " (" + type_ +
           ") parent_index: " + ::std::to_string(parent_index_.value_or(-1)) + " link: " + name_link_.value_or("none") +
           " flags: " + ::std::to_string(flags_) + " child_type: " + child_type_.value_or("none") + " mac: " + mac_ +
           " vlanid: " + ::std::to_string(vlanid_.value_or(-1));
  }
};

using Links = ::std::vector<LinkInfo>;

}  // namespace netconf
