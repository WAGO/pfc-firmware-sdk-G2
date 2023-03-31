// SPDX-License-Identifier: GPL-2.0-or-later

#include "DHCPClientIDHandler.hpp"

#include <string>

#include "IPConfig.hpp"
#include "InterfaceInformationApi.hpp"
#include "NetconfStatus.hpp"
#include "OptionStrings.hpp"
#include "Utils.hpp"

namespace network_config {

namespace napi = ::netconf::api;

DCHPClientIDHandler::DCHPClientIDHandler(const ::boost::program_options::variables_map &vm) {
  if (vm.count("set") > 0) {
    execute_ = [&]() {
      auto clientid = GetValueOfSet(vm);
      auto error    = napi::SetTempDHCPClientID(clientid);
      if (error.IsNotOk()) {
        throw NetconfStatus(error);
      }
    };
  }
}

void DCHPClientIDHandler::Execute() {
  execute_();
}

} /* namespace network_config */
