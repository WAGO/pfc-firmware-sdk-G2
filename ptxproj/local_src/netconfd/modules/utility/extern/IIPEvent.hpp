// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>
#include "BaseTypes.hpp"

namespace netconf {

enum class ChangeType {
  Add,
  Delete,
  Modify,
};


class IIPEvent{
 public:
  IIPEvent() = default;
  virtual ~IIPEvent() = default;

  IIPEvent(const IIPEvent &other) = delete;
  IIPEvent& operator=(const IIPEvent &other) = delete;
  IIPEvent(IIPEvent &&other) = delete;
  IIPEvent& operator=(IIPEvent &&other) = delete;

  virtual void OnAddressChange(ChangeType change_type, int index, Address address, Netmask netmask) = 0;

};

inline ::std::string ToString(ChangeType type) {
    switch (type) {
      case ChangeType::Add:
        return "ADD";
      case ChangeType::Delete:
        return "DELETE";
      case ChangeType::Modify:
        return "MODIFY";
      default:
        return "";
    }
  }


}  // namespace pfcspecific
