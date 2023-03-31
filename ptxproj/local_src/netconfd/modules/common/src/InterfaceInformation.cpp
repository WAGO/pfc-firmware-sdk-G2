// SPDX-License-Identifier: LGPL-3.0-or-later
#include "InterfaceInformation.hpp"
#include "JsonConverter.hpp"

namespace netconf {


InterfaceInformation::InterfaceInformation(Interface interface, bool ip_configuration_readonly,
                                           AutonegotiationSupported autoneg_supported, LinkModes link_modes)
    : interface_{ ::std::move(interface) },
      ip_configuration_readonly_ { ip_configuration_readonly },
      autoneg_supported_ { autoneg_supported },
      link_modes_ { ::std::move(link_modes) } {
}

std::pair<bool, const InterfaceInformation*> GetFirstOf(const InterfaceInformations &infos,
                                                        std::function<InterfaceInformationPred> predicate) {
  auto it = std::find_if(infos.begin(), infos.end(), ::std::move(predicate));
  if (it != infos.end()) {
    return {true, &(*it)};
  }
  return {false, nullptr};

}

std::pair<bool, InterfaceInformation*> GetFirstOf(InterfaceInformations &infos,
                                                  std::function<InterfaceInformationPred> predicate) {
  auto it = std::find_if(infos.begin(), infos.end(), ::std::move(predicate));
  if (it != infos.end()) {
    return {true, &(*it)};
  }
  return {false, nullptr};

}

}  // namespace netconf
