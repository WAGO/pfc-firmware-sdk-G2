// SPDX-License-Identifier: GPL-2.0-or-later

#include "InterfaceHandler.hpp"

#include <string>
#include <iostream>
#include <exception>
#include <utility>

#include "Utils.hpp"
#include "OptionStrings.hpp"
#include "NetconfStatus.hpp"
#include "Interface.hpp"

namespace network_config {

InterfaceHandler::InterfaceHandler(const ::boost::program_options::variables_map &vm)
    : vm_ { vm } {

  if (vm_.count("add") > 0) {
    execute_ = [this]() {
      this->AddInterface();
    };
  }
  else if (vm_.count("del") > 0) {
    execute_ = [this]() {
      this->DeleteInterface();
    };
  }
}

InterfaceHandler::InterfaceHandler(InterfaceHandler &&other) noexcept
    : execute_ { std::move(other.execute_) },
      vm_ { other.vm_ } {

}

void InterfaceHandler::Execute() {
  execute_();
}

void InterfaceHandler::AddInterface() {
  auto value = GetValueOfAdd(vm_);
  auto interface = CreateInterface(value);

  const auto &options = GetOptions();
  netconf::Status result;
  if (Contains(vm_, options.dryrun)) {
    // TODO(Team): Not supported yet
  } else {
    result = ::netconf::api::AddInterface(interface);
  }

  if (result.IsNotOk()) {
    throw NetconfStatus { result };
  }
}

void InterfaceHandler::DeleteInterface() {
  auto value = GetValueOfDel(vm_);
  auto interface = CreateInterface(value);

  const auto &options = GetOptions();
  netconf::Status result;
  if (Contains(vm_, options.dryrun)) {
    // TODO(Team): Not supported yet
  } else {
    result = ::netconf::api::DeleteInterface(interface);
  }

  if (result.IsNotOk()) {
    throw NetconfStatus { result };
  }
}


::netconf::Interface InterfaceHandler::CreateInterface(const ::std::string &value) {
  netconf::Interface interface;

  if (not value.empty()) {
    auto status = netconf::api::MakeInterface(value, interface);
    return status.IsOk() ? interface : throw NetconfStatus { status };
  }

  return interface;
}

} /* namespace network_config */
