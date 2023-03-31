// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IpConfigHelper.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#pragma once

#include "Types.hpp"
#include "NetDev.hpp"
#include "IDipSwitch.hpp"

namespace netconf {

void CleanWithRespectToSystem(IPConfigs &ip_configs, const NetDevs &netdevs,
                              const Interfaces &not_assignable_interfaces);


IPConfig CreateIpConfigFromDipInfos(const DipSwitchIpConfig& dip_ip_config, const DipSwitchState& state);

void ModifyIpConfigByDipSwitch(IPConfigs &config, const DipSwitchIpConfig &dip_switch_config, DipSwitchState dip_switch_state);

void DeactivateIpConfigsWithConflicsTo(const IPConfig& config, IPConfigs& from_configs, IPSource having_source = IPSource::STATIC);
}  // namespace netconf

//---- End of header file ------------------------------------------------------

