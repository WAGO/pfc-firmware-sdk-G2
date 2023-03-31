// SPDX-License-Identifier: GPL-2.0-or-later

#include "InterfaceValidator.hpp"
#include "CollectionUtils.hpp"

#include "Status.hpp"

#include <cstdio>

namespace netconf {

namespace {

Status CheckIfInterfaceIsAddable(const Interface &interface) {
  auto types = VirtualInterfaceDeviceTypes();
  if (::std::none_of(types.begin(), types.end(), [&](auto type){return type == interface.GetType();})) {
    return Status(StatusCode::INTERFACE_VALIDATION, "Interface of type " + ToString(interface.GetType()) + " cannot be added");
  }
  return Status{};
}

Status CheckInterfaceNameLength(const Interface &interface) {
  if (interface.GetName().empty() || interface.GetName().size() > 15) {
    return Status(StatusCode::INTERFACE_VALIDATION,
                  "The interface \"" + interface.GetName() + "\" name must be 1 to 15 characters long");
  }
  return Status{};
}

Status CheckIfInterfaceNameIsUnique(const Interface &interface, const Interfaces &interfaces) {

  auto number_of_identically_named_interfaces = ::std::count_if(interfaces.begin(), interfaces.end(), [&](const auto itf){
    return interface.GetName() == itf.GetName();
  });

  if(number_of_identically_named_interfaces > 0) {
    return Status(StatusCode::INTERFACE_VALIDATION,
                  "An identically named interface already exists: " + interface.GetName());
  }
  return Status{};
}

Status CheckIfVlanIDIsValid(const Interface &interface) {

  if( interface.GetProperties().count(InterfaceProperty::VlanID) < 1 ) {
    return Status(StatusCode::INTERFACE_VALIDATION, "Missing vlanID for interface " + interface.GetName());
  }

  try {
    int vlan_id = ::boost::get<int>(interface.GetProperties().at(InterfaceProperty::VlanID));
    if(vlan_id < 3 || vlan_id > 4094){
      throw std::invalid_argument("");
    }
  } catch (...) {
    return Status(StatusCode::INTERFACE_VALIDATION, "Wrong VLAN-ID for interface " + interface.GetName() +
                  ". VLAN-ID must be in the range 3-4094.");
  }

  return Status{};
}

Status CheckIfVlanIDIsDuplicate(const Interface &interface, const Interfaces &interfaces){

  Status status;

  try {
    auto vlan_id = ::boost::get<int>(interface.GetProperties().at(InterfaceProperty::VlanID));

    auto it = ::std::find_if(interfaces.begin(), interfaces.end(), [&](const Interface &itf) {
      if(itf.GetType() == DeviceType::Vlan) {
        auto itf_vlan_id = ::boost::get<int>(itf.GetProperties().at(InterfaceProperty::VlanID));
          return  itf_vlan_id == vlan_id;
      }
      return false;
    });
    if (it != interfaces.end()) {
      status = Status(StatusCode::INTERFACE_VALIDATION,
                      "VLAN-ID " + ::std::to_string(vlan_id) + " for interface " + interface.GetName() +
                      " is already in use for interface " + (*it).GetName());
    }

  } catch (...) {
    status = Status(StatusCode::INTERFACE_VALIDATION,
                    "Failed to get VLAN-ID while checking for duplicate VlanIDs for interface: " + interface.GetName());
  }

  return status;
}

Status CheckIfLinkedInterfaceIsValid(const Interface &interface, const Interfaces &interfaces){

  Status status;

  if(interface.GetProperties().count(InterfaceProperty::Link) < 1) {
    return Status(StatusCode::INTERFACE_VALIDATION, "Missing link interface for " + interface.GetName());
  }

  try {
    auto linked = ::boost::get<::std::string>(interface.GetProperties().at(InterfaceProperty::Link));
    auto it = ::std::find_if(interfaces.begin(), interfaces.end(), [&](const Interface &itf) {
      return itf.GetName() == linked;
    });
    if (it == interfaces.cend()) {
      status = Status(StatusCode::INTERFACE_VALIDATION,
                      "Linked interface \"" + linked + "\" for interface " + interface.GetName() + " does not exist");
    }
  } catch (::boost::bad_get& ex) {
    status = Status(StatusCode::INTERFACE_VALIDATION,
                   "Failed to check linked interface of VLAN interface: " + interface.GetName() + ". " + ex.what());
  }

  return status;
}

Status CheckIfVlanExistsWithDifferentParameters(const Interface &interface, const Interfaces &interfaces){

  Status status;

  try {
    auto vlan_id = ::boost::get<int>(interface.GetProperties().at(InterfaceProperty::VlanID));
    auto linked = ::boost::get<::std::string>(interface.GetProperties().at(InterfaceProperty::Link));

    auto it = ::std::find_if(interfaces.begin(), interfaces.end(), [&](const Interface &itf) {
          if (itf.GetName() == interface.GetName()) {
            auto itf_vlan_id = ::boost::get<int>(itf.GetProperties().at(InterfaceProperty::VlanID));
            auto itf_linked = ::boost::get<::std::string>(itf.GetProperties().at(InterfaceProperty::Link));
            return ( itf_vlan_id != vlan_id)
                || ( itf_linked != linked);
          }
          return false;
        });
    if (it != interfaces.end()) {
      status = Status(StatusCode::INTERFACE_VALIDATION,
                      "VLAN " + interface.GetName() + " already exists with other VLAN-ID or parent interface");
    }
  } catch(...) {
    status = Status(StatusCode::INTERFACE_VALIDATION,
                    "Failed to check if VLAN interface exists with different parameters for VLAN interface: " +
                    interface.GetName() + ".");
  }

  return status;
}

Status CheckIfMaximumNumberOfVlanInterfacesIsExceeded(const Interfaces &interfaces){

  auto number_of_vlan_interfaces = ::std::count_if(interfaces.begin(), interfaces.end(), [&](const auto itf) {
    return itf.GetType() == DeviceType::Vlan;
  });

  if (number_of_vlan_interfaces >= 16) {
    return Status(StatusCode::INTERFACE_VALIDATION, "Reached maximum number of 16 VLAN interfaces.");
  }
  return Status { };
}

}


Status Validate(const Interface &interface, const Interfaces &existing_interfaces, const Interfaces &persisted_interfaces) {

  Status status;

  status = CheckInterfaceNameLength(interface);
  if(status.IsOk()) {
    status = CheckIfInterfaceIsAddable(interface);
  }
  if(status.IsOk()) {
    status = CheckIfInterfaceNameIsUnique(interface, existing_interfaces);
  }

  // Check VLAN specific interface properties
  if(status.IsOk()) {
    if (interface.GetType() == DeviceType::Vlan) {
      status = CheckIfVlanIDIsValid(interface);
      if(status.IsOk()) {
        status = CheckIfLinkedInterfaceIsValid(interface, existing_interfaces);
      }
      if(status.IsOk()) {
        status = CheckIfVlanIDIsDuplicate(interface, existing_interfaces);
      }
      if(status.IsOk()) {
        status = CheckIfVlanExistsWithDifferentParameters(interface, existing_interfaces);
      }
      if(status.IsOk()) {
        status = CheckIfMaximumNumberOfVlanInterfacesIsExceeded(persisted_interfaces);
      }
    }
  }

  return status;
}

Status ValidateName(const Interface &interface) {
  return CheckInterfaceNameLength(interface);
}

}
