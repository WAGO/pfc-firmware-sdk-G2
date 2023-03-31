// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>
#include <vector>

#include "DeviceType.hpp"
#include "LinkMode.hpp"

namespace netconf {

class InterfaceInformation {
 public:

  InterfaceInformation() = default;
  explicit InterfaceInformation(Interface interface,
                       bool ip_configuration_readonly = false,
                       AutonegotiationSupported autoneg_supported = AutonegotiationSupported::UNKNOWN,
                       LinkModes link_modes = LinkModes { });

  Interface GetInterface() const {
    return interface_;
  }

  bool IsIpConfigurationReadonly() const {
    return ip_configuration_readonly_;
  }

  DeviceType GetType() const {
    return interface_.GetType();
  }

  AutonegotiationSupported GetAutonegSupported() const {
    return autoneg_supported_;
  }

  LinkModes GetSupportedlinkModes() const {
    return link_modes_;
  }

  bool operator==(const InterfaceInformation &ii) const {
    return ::std::tie(this->interface_, this->ip_configuration_readonly_,
                      this->autoneg_supported_, this->link_modes_)
        == ::std::tie(ii.interface_, ii.ip_configuration_readonly_,
                      ii.autoneg_supported_, ii.link_modes_);
  }

 private:
  Interface interface_;
  bool ip_configuration_readonly_{ true };
  AutonegotiationSupported autoneg_supported_{ AutonegotiationSupported::UNKNOWN };
  LinkModes link_modes_;
};

using InterfaceInformations = ::std::vector<InterfaceInformation>;
using InterfaceInformationPred = bool(const InterfaceInformation&);

inline std::function<InterfaceInformationPred> byInterface(const Interface& interface){
  return [=](const InterfaceInformation& ii) -> bool {
    return ii.GetInterface() == interface;
  };
}

/**
 * Get an interface information object from the set of information objects that meets the given predicate.
 *
 * @param infos
 * @param predicate
 * @return a pair of a boolean and pointer, were the boolean indicates whet
 */
std::pair<bool, InterfaceInformation*> GetFirstOf(InterfaceInformations& infos, std::function<InterfaceInformationPred> predicate);
std::pair<bool, const InterfaceInformation*> GetFirstOf(const InterfaceInformations& infos, std::function<InterfaceInformationPred> predicate);


}  // namespace netconf

//---- End of header file ------------------------------------------------------

