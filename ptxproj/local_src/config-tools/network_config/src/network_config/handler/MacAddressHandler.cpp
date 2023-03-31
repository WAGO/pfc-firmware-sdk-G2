// SPDX-License-Identifier: GPL-2.0-or-later

#include "MacAddressHandler.hpp"

#include <iostream>
#include <string>
#include <exception>

#include "Utils.hpp"
#include "InterfaceConfig.hpp"
#include "Status.hpp"
#include "NetconfStatus.hpp"

namespace po = boost::program_options;

using netconf::Status;
using netconf::StatusCode;

namespace network_config {

MacAddressHandler::MacAddressHandler(const po::variables_map &vm)
    : vm_ { vm } {
}

void MacAddressHandler::GetMacAddress() {
  try {
    auto interface = GetDevice(vm_);

    ::std::cout << netconf::api::GetMacAddress(interface) << ::std::flush;
  } catch (std::exception &e) {
    throw NetconfStatus { Status { StatusCode::SYSTEM_CALL, e.what() } };
  }
}

void MacAddressHandler::Execute() {
  this->GetMacAddress();
}

} /* namespace network_config */
