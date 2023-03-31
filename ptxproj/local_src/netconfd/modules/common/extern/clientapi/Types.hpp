// SPDX-License-Identifier: LGPL-3.0-or-later
//------------------------------------------------------------------------------
#pragma once


#include <utility>
#include <map>
#include <string>
#include <vector>

#include "BaseTypes.hpp"
#include "DeviceType.hpp"
#include "InterfaceInformation.hpp"
#include "MacAddress.hpp"

namespace netconf {


using Interfaces = ::std::vector<Interface>;
using BridgeConfig = ::std::map<Interface, Interfaces>;

using IPConfigs = ::std::vector<IPConfig>;
using InterfaceConfigs = ::std::vector<InterfaceConfig>;
using InterfaceStatuses = ::std::vector<InterfaceStatus>;

}

