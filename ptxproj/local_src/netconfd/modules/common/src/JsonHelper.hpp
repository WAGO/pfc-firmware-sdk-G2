// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include "Status.hpp"
#include "Types.hpp"
#include "FirmwareVersion.hpp"
#include "DynamicIPEventAction.hpp"

namespace netconf {

using nlohmann::json;

constexpr int JSON_PRETTY_DUMP = 2;
constexpr int JSON_DUMP = -1;

NLOHMANN_JSON_SERIALIZE_ENUM(Duplex, { {Duplex::UNKNOWN, nullptr}, {Duplex::HALF, "half"}, {Duplex::FULL, "full"} })

NLOHMANN_JSON_SERIALIZE_ENUM(Autonegotiation, { {Autonegotiation::UNKNOWN, nullptr}, {Autonegotiation::ON, "on"},
                             {Autonegotiation::OFF, "off"}, {Autonegotiation::ON, true}, {Autonegotiation::OFF,
                             false} })

NLOHMANN_JSON_SERIALIZE_ENUM(AutonegotiationSupported, { {AutonegotiationSupported::UNKNOWN, nullptr},
                             {AutonegotiationSupported::YES, true}, {AutonegotiationSupported::NO, false} })

NLOHMANN_JSON_SERIALIZE_ENUM(InterfaceState, { {InterfaceState::UNKNOWN, nullptr}, {InterfaceState::DOWN, "down"},
                             {InterfaceState::UP, "up"} })

NLOHMANN_JSON_SERIALIZE_ENUM(LinkState, { {LinkState::UNKNOWN, nullptr}, {LinkState::DOWN, "down"}, {LinkState::UP,
                             "up"} })

NLOHMANN_JSON_SERIALIZE_ENUM(IPSource, { {IPSource::UNKNOWN, nullptr }, {IPSource::NONE, "none" }, {IPSource::STATIC,
                             "static" }, {IPSource::DHCP, "dhcp" }, {IPSource::BOOTP, "bootp" }, {IPSource::EXTERNAL,
                             "external"}, {IPSource::FIXIP, "fixip"}})

NLOHMANN_JSON_SERIALIZE_ENUM(DipSwitchMode, { {DipSwitchMode::OFF, "off"}, {DipSwitchMode::STATIC, "static"},
                             {DipSwitchMode::DHCP, "dhcp"}, {DipSwitchMode::HW_NOT_AVAILABLE, "hw-not-available"} })

NLOHMANN_JSON_SERIALIZE_ENUM(DeviceType, {{DeviceType::Other, "other" }, {DeviceType::Ethernet, "ethernet" }, {DeviceType::Bridge, "bridge" },
                             {DeviceType::Wwan, "wwan" }, {DeviceType::Loopback, "loopback" }, {DeviceType::Port, "port" },
                             {DeviceType::Dummy, "dummy" },{DeviceType::Vlan, "vlan" } })

NLOHMANN_JSON_SERIALIZE_ENUM(DynamicIPEventAction, { {DynamicIPEventAction::UNKNOWN, nullptr},
                             {DynamicIPEventAction::BOUND, "bound"}, {DynamicIPEventAction::RELEASE, "release"},
                             {DynamicIPEventAction::RENEW, "renew"}, {DynamicIPEventAction::NAK, "nak"}})

NLOHMANN_JSON_SERIALIZE_ENUM(MacLearning, { {MacLearning::UNKNOWN, nullptr}, {MacLearning::OFF, "off"}, {MacLearning::ON, "on"} })

static inline Status MissingJsonKeyError(const std::string &keyname) {
  return Status { StatusCode::JSON_KEY_MISSING, keyname };
}

template<typename T>
bool GetToIfExists(const ::std::string &key, const nlohmann::json &inner_json, T &to) {
  auto iter = inner_json.find(key);
  if (iter != inner_json.end()) {
    iter.value().get_to(to);
    return true;
  }
  return false;
}

template<typename T>
IPConfig ParseIpConfig(const T &json_iter) {
  IPConfig c;
  c.interface_ = Interface::NameFromLabel(json_iter.key());
  auto inner = json_iter.value();

  GetToIfExists("source", inner, c.source_);
  GetToIfExists("ipaddr", inner, c.address_);
  GetToIfExists("netmask", inner, c.netmask_);
  ::std::string dhcp_client_id;
  GetToIfExists("dhcp-client-id", inner, dhcp_client_id);
  if (!dhcp_client_id.empty()) {
    c.dhcp_client_id_ = dhcp_client_id;
  }
  return c;
}

template<typename T>
Status GetToOrError(const ::std::string &key, const nlohmann::json &inner_json, T &to){
  auto iter = inner_json.find(key);
  if (iter != inner_json.end()) {
    iter.value().get_to(to);
    return {};
  }
  return MissingJsonKeyError(key);
}

Status JsonToNJson(::std::string const &json_str, json &json_object);
json DipSwitchIpConfigToNJson(const DipSwitchIpConfig &ip_config);
json DipSwitchConfigToNJson(const DipSwitchConfig &config);
json IpConfigToNJson(const IPConfig &ip_config);
json IPConfigsToNJson(const IPConfigs &ip_configs);
json BridgeConfigToNJson(const BridgeConfig &bridge_config, const FirmwareVersion &target_fw = FirmwareVersion { });
json InterfaceConfigToNJson(const InterfaceConfig &interface_config);
json InterfaceConfigsToNJson(const InterfaceConfigs &interface_configs, const FirmwareVersion &target_fw =
                                 FirmwareVersion { });
json InterfaceStatusToNJson(const InterfaceStatus &interface_status);
json InterfaceStatusesToNJson(const InterfaceStatuses &interface_status);
json InterfaceInformationToNJson(const InterfaceInformation &interface_information);
json InterfaceToNJson(const Interface &interface);
json InterfacesToNJson(const Interfaces &interfaces);

::std::string DipSwitchIPConfigToJson(const DipSwitchIpConfig &config);
::std::string DipSwitchConfigToJson(const DipSwitchConfig &config);

Status JsonToDipSwitchConfig(const ::std::string &json_string, DipSwitchConfig &out_obj);

Status NJsonToBridgeConfig(const json &json_object, BridgeConfig &bridge_config);
Status NJsonToDipIPConfig(const json &json_object, DipSwitchIpConfig &ip_config);
Status NJsonToIPConfigs(const json &json_object, IPConfigs &ip_configs);
Status NJsonToInterfaceConfigs(const json &json_object, InterfaceConfigs &interface_configs);
Status NJsonToInterfaceStatuses(const nlohmann::json &json_object, InterfaceStatuses &interface_statuses);
Status NJsonToInterfaceInformation(const json &json_object, InterfaceInformation &interface_information);
Status NJsonToInterface(const json &json_object, Interface &interface);
Status NJsonToInterfaces(const json &json_array, Interfaces &interfaces);

Status GetAddressFromJson(const std::string &json_field, const json &from, std::string &to);
InterfaceConfig InterfaceConfigFromJson(const json &config);
InterfaceStatus InterfaceStatusFromJson(const json &config);

}
