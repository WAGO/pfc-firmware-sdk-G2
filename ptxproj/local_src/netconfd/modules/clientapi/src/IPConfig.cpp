// SPDX-License-Identifier: LGPL-3.0-or-later

#include "IPConfig.hpp"

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/network_v4.hpp>
#include "ConfigBase.hpp"
#include "NetconfdDbusClient.hpp"
#include "JsonConverter.hpp"
#include "DipSwitchConfig.hpp"
#include "InterfaceInformationApi.hpp"
#include "IPValidator.hpp"
#include "TypesHelper.hpp"

namespace netconf {
namespace api {


namespace{

IPConfigs FilterCopyByType(const IPConfigs &ip_configs, const DeviceTypes& types) {
  auto interface_infos = GetInterfaceInformation(types);

  auto inner_configs = ip_configs.GetConfig();

  auto interface_name_not_in_infos = [&](const auto &ip_cfg) -> bool {
    return ::std::find_if(interface_infos.begin(), interface_infos.end(), [&](const auto &info) {
      return info.GetInterface().GetName() == ip_cfg.interface_;
    }) == interface_infos.end();
  };

  inner_configs.erase(std::remove_if(inner_configs.begin(), inner_configs.end(), interface_name_not_in_infos),
                      inner_configs.end());

  return IPConfigs { inner_configs };
}

}


::std::string ToString(IPSource source) {
  switch (source) {
    case IPSource::DHCP:
      return "dhcp";
    case IPSource::BOOTP:
      return "bootp";
    case IPSource::STATIC:
      return "static";
    default:
      return "none";
  }
}

IPConfigs::IPConfigs(const netconf::IPConfigs &configs) : detail::ConfigBase<netconf::IPConfig>(configs) {

}

void IPConfigs::AddIPConfig(IPConfig config) {
  AddConfig(config);
}

void IPConfigs::RemoveIPConfig(const ::std::string &interface) {
  RemoveConfig(interface);
}

::std::optional<IPConfig> IPConfigs::GetIPConfig(const ::std::string &interface) const {
  return GetConfig(interface);
}

::std::string IPConfigs::GetCompareValue(const IPConfig & config) const noexcept {
  return config.interface_;
}

IPConfig& IPConfigs::operator[](const ::std::string &interface) {
  auto matches_interface_name = [&](const IPConfig &c) {
    return GetCompareValue(c) == interface;
  };

  auto it = ::std::find_if(configs_.begin(), configs_.end(), matches_interface_name);
  if (it != configs_.end()) {
    return *it;
  }
  return configs_.emplace_back(interface);

}

::std::string ToJson(const IPConfigs& configs) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(configs.GetConfig());
}

::std::string ToPrettyJson(const IPConfigs& configs) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(configs.GetConfig(), JsonFormat::PRETTY);
}

Status GetIPConfigs(const DeviceTypes & types, IPConfigs& configs) {
  IPConfigs c;
  auto status = GetIPConfigs(c);
  if(status.IsNotOk())
  {
    return status;
  }
  configs = FilterCopyByType(c, types);
  return status;
}

Status GetCurrentIPConfigs(const DeviceTypes& types, IPConfigs& configs) {
  IPConfigs c;
  auto status = GetCurrentIPConfigs(c);
  if(status.IsNotOk())
  {
    return status;
  }
  configs = FilterCopyByType(c, types);
  return status;
}

void DeleteIPConfig(const ::std::string& interface) {
  IPConfigs ip_configs;
  IPConfig ip_config { interface, IPSource::NONE, "", "" };
  ip_configs.AddIPConfig(ip_config);
  SetIPConfigs(ip_configs);
}

::std::string CalculateBroadcast(const IPConfig &config) noexcept
{
  namespace bip = ::boost::asio::ip;
  try {
    auto netv4 = bip::make_network_v4(bip::make_address_v4(config.address_), bip::make_address_v4(config.netmask_));
    return netv4.network().to_string();
  } catch (...) {
    return std::string { };
  }
}

Status MakeIPConfigs(const ::std::string& json_str, IPConfigs& config) noexcept{
  JsonConverter jc;
  netconf::IPConfigs c;
  Status status = jc.FromJsonString(json_str, c);
  config = IPConfigs(c);
  return status;
}

::std::string ToJson(const netconf::IPConfig &ip_config) noexcept{
  JsonConverter jc;
  return jc.ToJsonString(ip_config);
}

::std::string ToPrettyJson(const netconf::IPConfig &ip_config) noexcept{
  JsonConverter jc;
  return jc.ToJsonString(ip_config, JsonFormat::PRETTY);
}


Status ValidateIpConfigs(const IPConfigs& candidate, const IPConfigs& current, const InterfaceInformations& interface_information){
  return IPValidator::Validate(candidate.GetConfig(), current.GetConfig(), interface_information);
}


}  // api
}  // netconf
