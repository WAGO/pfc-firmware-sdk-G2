// SPDX-License-Identifier: GPL-2.0-or-later

#include "DHCPClientIDHandler.hpp"

#include <string>
#include <iostream>

#include "IPConfig.hpp"
#include "NetconfStatus.hpp"
#include "OptionStrings.hpp"
#include "Utils.hpp"

namespace network_config {

namespace napi = ::netconf::api;

DCHPClientIDHandler::DCHPClientIDHandler(const ::boost::program_options::variables_map &vm) {
  if (vm.count("set") > 0) {
    execute_ = [&]() {
      auto clientid = GetValueOfSet(vm);
      auto error = napi::SetTempDHCPClientID(clientid);
      if (error.IsNotOk()) {
        throw NetconfStatus(error);
      }
    };
  } else if (vm.count("get") > 0) {
    execute_ = [&]() {
      ::std::string client_id;
      auto error = napi::GetTempDHCPClientID(client_id);
      if (error.IsNotOk()) {
        throw NetconfStatus(error);
      }
      ::std::cout << client_id;
    };
  }
}

void DCHPClientIDHandler::Execute() {
  execute_();
}

} /* namespace network_config */
