// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPConfigHandler.hpp"

#include <string>
#include <iostream>
#include <exception>
#include <utility>

#include "Utils.hpp"
#include "OptionStrings.hpp"
#include "NetconfStatus.hpp"
#include "InterfaceInformationApi.hpp"

namespace network_config {

namespace napi = ::netconf::api;

IPConfigHandler::IPConfigHandler(const OptionParser &parser)
    : vm_ { parser.GetVariableMap() },
      parser_ { parser } {
  const auto &options = GetOptions();

  TypeFilter type_filter_opt;
  if (Contains(vm_, options.filter)) {
    type_filter_opt = netconf::DeviceTypeFromString(Get(vm_, options.filter));
  }

  if (vm_.count("get") > 0) {
    execute_ = [this, filter=type_filter_opt]() {
      this->GetConfig(filter);
    };
  } else if (vm_.count("get-current") > 0) {
    execute_ = [this, filter=type_filter_opt]() {
      this->GetCurrentConfig(filter);
    };
  } else if (vm_.count("set") > 0) {
    execute_ = [this]() {
      this->SetConfig();
    };
  }
}

void IPConfigHandler::Execute() {
  execute_();
}

static ::std::string IPSourceToString(::netconf::IPSource source) {
  ::std::string value = "unknown";
  if (source == ::netconf::IPSource::NONE) {
    value = "none";
  } else if (source == ::netconf::IPSource::STATIC) {
    value = "static";
  } else if (source == ::netconf::IPSource::DHCP) {
    value = "dhcp";
  } else if (source == ::netconf::IPSource::BOOTP) {
    value = "bootp";
  } else if (source == ::netconf::IPSource::EXTERNAL) {
    value = "external";
  }
  return value;
}

void IPConfigHandler::ParseConfig(netconf::api::IPConfigs &ip_configs) {
  auto format = GetFormat(vm_);
  const auto &fields = GetFields();

  if (Contains(vm_, fields.device)) {
    ::std::string interface = GetDevice(vm_);
    auto ip_config = ip_configs.GetIPConfig(interface);

    if (ip_config) {
      ::std::cout << ConstructString(*ip_config, format);
    } else {
      throw NetconfStatus { netconf::Status { netconf::StatusCode::INTERFACE_NOT_EXISTING, interface } };
    }
  } else {
    ::std::cout << ConstructString(ip_configs, format);
  }

  ::std::cout.flush();
}

void IPConfigHandler::GetCurrentConfig(TypeFilter filter) {

  napi::IPConfigs ip_configs;
  if (filter) {
    auto error = napi::GetCurrentIPConfigs(*filter, ip_configs);
    if (error.IsNotOk()) {
      throw NetconfStatus { error };
    }
    ParseConfig(ip_configs);
  } else {
    auto error = ::netconf::api::GetCurrentIPConfigs(ip_configs);
    if (error.IsNotOk()) {
      throw NetconfStatus { error };
    }
    ParseConfig(ip_configs);
  }
}

void IPConfigHandler::GetConfig(TypeFilter filter) {

  napi::IPConfigs ip_configs;
  if (filter) {
    auto error = napi::GetIPConfigs(*filter, ip_configs);
    if (error.IsNotOk()) {
      throw NetconfStatus { error };
    }
    ParseConfig(ip_configs);
  } else {
    auto error = napi::GetIPConfigs(ip_configs);
    if (error.IsNotOk()) {
      throw NetconfStatus { error };
    }
    ParseConfig(ip_configs);
  }

}

void IPConfigHandler::SetConfig() {
  auto ip_configs = CreateIPConfigs();
  const auto &options = GetOptions();
  netconf::Status result;
  if (Contains(vm_, options.dryrun)) {
    napi::IPConfigs current_configs;
    result = GetCurrentIPConfigs(current_configs);
    if (result.IsOk()) {
      result = napi::ValidateIpConfigs(ip_configs, current_configs, napi::GetInterfaceInformation());
    }
  } else {
    result = SetIPConfigs(ip_configs);
  }

  if (result.IsNotOk()) {
    throw NetconfStatus { result };
  }
}

::netconf::api::IPConfigs IPConfigHandler::CreateIPConfigs() {

  auto value = GetValueOfSet(vm_);
  napi::IPConfigs ip_configs;
  if (!value.empty()) {
    auto error = napi::MakeIPConfigs(value, ip_configs);
    return error.IsOk() ? ip_configs : throw NetconfStatus { error };
  }
  return ip_configs;

}

IPConfigHandler::IPConfigHandler(IPConfigHandler &&other) noexcept
    : execute_ { std::move(other.execute_) },
      vm_ { other.vm_ },
      parser_ { other.parser_ } {

}

::std::string IPConfigHandler::ConstructString(const netconf::IPConfig &ip_config, const ::std::string& format) const {
  ::std::stringstream output;
  if (format == "text") {
    output << "device=" << netconf::Interface::LabelFromName(ip_config.interface_);
    output << " source=" << IPSourceToString(ip_config.source_);
    output << " ipaddr=" << ip_config.address_;
    output << " netmask=" << ip_config.netmask_;
    output << " bcast=" << netconf::api::CalculateBroadcast(ip_config);
    if(!ip_config.dhcp_client_id_.empty()){
      output << " dhcp-client-id=" << ip_config.dhcp_client_id_;
    }
    output << " ";
  } else if (format == "json") {
    output << napi::ToJson(ip_config);
  } else if (format == "pretty-json") {
    output << napi::ToPrettyJson(ip_config);
  }
  return output.str();
}

::std::string IPConfigHandler::ConstructString(const netconf::api::IPConfigs &ip_configs,
                                               const ::std::string& format) const {
  ::std::string value{};
  if (format == "text") {
    for (auto &ip_config : ip_configs.GetConfig()) {
      value += ConstructString(ip_config, format);
    }
  } else if (format == "json") {
    value = napi::ToJson(ip_configs);
  } else if (format == "pretty-json") {
    value = napi::ToPrettyJson(ip_configs);
  }
  return value;
}

} /* namespace network_config */
