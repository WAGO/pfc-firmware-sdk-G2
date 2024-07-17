
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Redundancy.hpp"

#include <algorithm>
#include <functional>
#include <stp.hpp>
#include <string>
#include <vector>

#include "BaseTypes.hpp"
#include "Logger.hpp"
#include "Status.hpp"
#include "Types.hpp"

namespace netconf {

namespace libstp = wago::stp::lib;

namespace {

void AdjustSTPPortConfigs(::std::vector<libstp::stp_port_config>& stp_port_configs, const Interfaces& bridge_ports) {
  ::std::vector<::std::string> stp_ports;
  ::std::for_each(stp_port_configs.begin(), stp_port_configs.end(), [&](auto& pc) { stp_ports.emplace_back(pc.port); });

  for (auto& stp_port : stp_ports) {
    if (::std::none_of(bridge_ports.begin(), bridge_ports.end(),
                       [&](auto& bp) { return bp == Interface::CreatePort(stp_port); })) {
      stp_port_configs.erase(::std::remove_if(stp_port_configs.begin(), stp_port_configs.end(),
                                              [&](auto& stp_c) { return stp_c.port == stp_port; }));
    }
  }
}

template<typename Functor>
void AdjustStpConfig(const Interface& bridge, Functor adjust){
  libstp::stp_config stp_config{};
  libstp::status s = libstp::get_persisted(stp_config);

  if (s.ok() && stp_config.bridge == bridge.GetName()) {
    if(stp_config.enabled){
      adjust(stp_config);
      s = libstp::configure(stp_config);
    }
  }

  if (!s.ok()) {
    LogError(Status(StatusCode::REDUNDANCY, s.to_string()).ToString());
  }
}

}  // namespace


void Redundancy::OnBridgeRemove(const Interface& bridge) const {

  auto adjust = [&] (libstp::stp_config& stp_config) {
    stp_config.enabled = false;
    LOG_DEBUG("configure redundacy: disable stp for removed bridge" + bridge.GetName());
  };

  AdjustStpConfig(bridge, adjust);
}

void Redundancy::OnBridgeAddOrPortChange(const Interface& bridge, const Interfaces& ports) const {

  auto adjust = [&] (libstp::stp_config& stp_config) {
    AdjustSTPPortConfigs(stp_config.port_configs,ports);
    LOG_DEBUG("configure redundacy: for bridge" + bridge.GetName());
  };

  AdjustStpConfig(bridge, adjust);

}

}  // namespace netconf