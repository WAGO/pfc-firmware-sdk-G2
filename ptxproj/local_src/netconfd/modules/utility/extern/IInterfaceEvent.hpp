// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <cstdint>
#include <string>

#include "NetworkInterfaceConstants.hpp"
#include "LinkInfo.hpp"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

namespace netconf {

enum class InterfaceEventAction {
  UNSPEC,
  NEW,
  DEL,
  GET,
  SET,
  CHANGE,
};
class IInterfaceEvent {
 public:
  IInterfaceEvent() = default;
  virtual ~IInterfaceEvent() = default;

  IInterfaceEvent(const IInterfaceEvent &other) = delete;
  IInterfaceEvent& operator=(const IInterfaceEvent &other) = delete;
  IInterfaceEvent(IInterfaceEvent &&other) = delete;
  IInterfaceEvent& operator=(IInterfaceEvent &&other) = delete;

  virtual void LinkChange(LinkInfo attributes, InterfaceEventAction action) = 0;
};

inline ::std::string ToString(InterfaceEventAction action) {
  switch (action) {
    case InterfaceEventAction::UNSPEC:
      return "UNSPEC";
    case InterfaceEventAction::NEW:
      return "NEW";
    case InterfaceEventAction::DEL:
      return "DEL";
    case InterfaceEventAction::GET:
      return "GET";
    case InterfaceEventAction::SET:
      return "SET";
    case InterfaceEventAction::CHANGE:
      return "CHANGE";
    default:
      return "";
  }
}

}  // namespace netconf
