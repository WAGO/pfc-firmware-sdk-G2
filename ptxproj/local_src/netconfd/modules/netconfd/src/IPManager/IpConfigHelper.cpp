// SPDX-License-Identifier: GPL-2.0-or-later
#include "TypesHelper.hpp"
#include "IpConfigHelper.hpp"
#include "Logger.hpp"
#include "NetDevManager.hpp"
#include "CollectionUtils.hpp"
#include "IpAddressManipulator.hpp"

#include "IPValidator.hpp"

namespace netconf {

namespace {

const DipSwitchIpConfig& getDefaultDipSwitchIpConfig() {
  static const DipSwitchIpConfig dip_switch_default_ip_config = DipSwitchIpConfig("192.168.1.0", "255.255.255.0");
  return dip_switch_default_ip_config;
}

bool HasToApplyDipSwitchConfig(DipSwitchMode mode) {
  return (mode != DipSwitchMode::HW_NOT_AVAILABLE && mode != DipSwitchMode::OFF);
}

}

void CleanWithRespectToSystem(IPConfigs &ip_configs, const NetDevs &netdevs,
                              const Interfaces &not_assignable_interfaces) {

  auto first_deleted_it = ::std::remove_if(ip_configs.begin(), ip_configs.end(), [&](auto &ip_config) {

    auto it_netdev = ::std::find_if(netdevs.begin(), netdevs.end(), [&](const NetDevPtr netdev){
      return netdev->GetName() == ip_config.interface_;
    });

    auto it_itf = ::std::find_if(not_assignable_interfaces.begin(), not_assignable_interfaces.end(), [&](const Interface& interface){
      return interface.GetName() == ip_config.interface_;
    });

    return (it_netdev == netdevs.end() || it_itf != not_assignable_interfaces.end());

  });

  std::for_each(first_deleted_it, ip_configs.end(), [](auto &deleted_ipcfg) {
    LogWarning("Drop not applicable IP config: " + IPConfigToString(deleted_ipcfg));
  });

  ip_configs.erase(first_deleted_it, ip_configs.end());
}

IPConfig CreateIpConfigFromDipInfos(const DipSwitchIpConfig &dip_ip_Config, const DipSwitchState &state) {

  IPConfig ipConfig { DIP_SWITCH_BRIDGE };
  if (state.mode == DipSwitchMode::DHCP) {
    ipConfig.source_ = IPSource::DHCP;
  } else {
    if (dip_ip_Config.IsIncomplete()) {
      ipConfig.address_ = getDefaultDipSwitchIpConfig().address_;
      ipConfig.netmask_ = getDefaultDipSwitchIpConfig().netmask_;
    } else {
      ipConfig.address_ = IpAddressManipulator::ChangeLastAddressOctet(dip_ip_Config.address_, state.value);
      ipConfig.netmask_ = dip_ip_Config.netmask_;
    }
    ipConfig.source_ = IPSource::STATIC;
  }

  return ipConfig;
}

void ModifyIpConfigByDipSwitch(IPConfigs &configs, const DipSwitchIpConfig &dip_switch_config,
                               const DipSwitchState dip_switch_state) {
  if (!HasToApplyDipSwitchConfig(dip_switch_state.mode)) {
    return;
  }

  auto itDipIpConfig = GetIpConfigByInterface(configs, Interface::CreateBridge(DIP_SWITCH_BRIDGE));
  if (itDipIpConfig == configs.end()) {
    return;
  }

  *itDipIpConfig = CreateIpConfigFromDipInfos(dip_switch_config, dip_switch_state);
}

void DeactivateIpConfigsWithConflicsTo(const IPConfig& config, IPConfigs& from_configs, IPSource having_source)
{
    for(auto& ip_config : from_configs) {
        if(ip_config.interface_ != config.interface_ && ip_config.source_ == having_source
              && IPValidator::IsSameSubnet(ip_config, config)){
          ip_config.source_ = IPSource::NONE;
        }
    }
}

}  // namespace netconf
