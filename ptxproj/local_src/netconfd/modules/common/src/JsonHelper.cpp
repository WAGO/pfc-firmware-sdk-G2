// SPDX-License-Identifier: LGPL-3.0-or-later

#include "JsonHelper.hpp"

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/network_v4.hpp>
#include <boost/system/error_code.hpp>
#include <nlohmann/json.hpp>

#include "CollectionUtils.hpp"
#include "JsonConverter.hpp"
#include "JsonHelper.hpp"
#include "JsonKeyList.hpp"
#include "NetworkHelper.hpp"
#include "Status.hpp"
#include "Types.hpp"
#include "TypesHelper.hpp"

namespace netconf {

using namespace ::std::literals;
using json = nlohmann::json;

using boost_address = boost::asio::ip::address;
using boost_error   = boost::system::error_code;

constexpr auto PORT_CFG_KEY_DEVICE                 = "device";
constexpr auto PORT_CFG_KEY_STATE                  = "state";
constexpr auto PORT_CFG_KEY_AUTONEG                = "autonegotiation";
constexpr auto PORT_CFG_KEY_AUTONEG_SUPPORTED      = "autonegsupported";
constexpr auto PORT_CFG_KEY_SPEED                  = "speed";
constexpr auto PORT_CFG_KEY_DUPLEX                 = "duplex";
constexpr auto PORT_CFG_KEY_LINKSTATE              = "link";
constexpr auto PORT_CFG_KEY_MAC                    = "mac";
constexpr auto PORT_CFG_KEY_SUPPORTED_LINK_MODES   = "supportedlinkmodes";
constexpr auto PORT_CFG_KEY_MAC_LEARNING           = "maclearning";
constexpr auto INTERFACE_KEY_NAME                  = "name";
constexpr auto INTERFACE_KEY_TYPE                  = "type";
constexpr auto INTERFACE_KEY_VLANID                = "vlanid";
constexpr auto INTERFACE_KEY_VLAN_LINKED_INTERFACE = "link";

static bool isValidInterfaceConfigKey(json const &j) {
  static JsonKeyList interface_config_keys{
      PORT_CFG_KEY_DEVICE,      PORT_CFG_KEY_STATE,  PORT_CFG_KEY_AUTONEG,   PORT_CFG_KEY_AUTONEG_SUPPORTED,
      PORT_CFG_KEY_SPEED,       PORT_CFG_KEY_DUPLEX, PORT_CFG_KEY_LINKSTATE, PORT_CFG_KEY_MAC,
      PORT_CFG_KEY_MAC_LEARNING};

  return interface_config_keys.matchesJsonObject(j);
}

static Status getToDeviceTypeOrError(const json &json_object, DeviceType &device_type) {
  Status status;

  auto iter = json_object.find(INTERFACE_KEY_TYPE);
  if (iter != json_object.end()) {
    if (iter.value().empty()) {
      device_type = DeviceType::Other;
    } else if (DeviceTypeIsValid(iter.value())) {
      iter.value().get_to(device_type);
    } else {
      status.Set(StatusCode::DEVICE_TYPE_NOT_SOPPORTED, ToString(device_type));
    }
  } else {
    // Device type was not specified
    device_type = DeviceType::Other;
  }

  return status;
}

Status NJsonToInterface(const json &json_object, Interface &interface) {
  DeviceType device_type = DeviceType::Other;
  ::std::string interface_name;

  Status status = getToDeviceTypeOrError(json_object, device_type);
  if (status.IsOk()) {
    status = GetToOrError(INTERFACE_KEY_NAME, json_object, interface_name);
  }

  if (status.IsOk()) {
    int vlan_id;
    ::std::string vlan_parent_interface;
    switch (device_type) {
      case DeviceType::Vlan:
        status = GetToOrError(INTERFACE_KEY_VLANID, json_object, vlan_id);
        if (status.IsOk()) {
          status = GetToOrError(INTERFACE_KEY_VLAN_LINKED_INTERFACE, json_object, vlan_parent_interface);
        }
        interface = Interface::CreateVLAN(interface_name, vlan_id, vlan_parent_interface);
        break;
      case DeviceType::Bridge:
        interface = Interface::CreateBridge(interface_name);
        break;
      case DeviceType::Dummy:
        interface = Interface::CreateDummy(interface_name);
        break;
      case DeviceType::Port:
        interface = Interface::CreatePort(interface_name);
        break;
      case DeviceType::Ethernet:
        interface = Interface::CreateEthernet(interface_name);
        break;
      case DeviceType::Loopback:
        interface = Interface::CreateLoopback(interface_name);
        break;
      case DeviceType::Wwan:
        interface = Interface::CreateWwan(interface_name);
        break;
      case DeviceType::Other:
        interface = Interface::CreateOther(interface_name);
        break;
      default:
        status.Set(StatusCode::DEVICE_TYPE_NOT_SOPPORTED, ToString(device_type));
        break;
    }
  }

  return status;
}

json DipSwitchIpConfigToNJson(const DipSwitchIpConfig &ip_config) {
  return json{{"ipaddr", ip_config.address_}, {"netmask", ip_config.netmask_}};
}

json IpConfigToNJson(const IPConfig &ip_config) {
  json json_ip_config = json{{"ipaddr", ip_config.address_},
                             {"netmask", ip_config.netmask_},
                             {"bcast", GetBroadcast(ip_config.address_, ip_config.netmask_)}};
  to_json(json_ip_config["source"], ip_config.source_);
  if(!ip_config.dhcp_client_id_.empty()){
    json_ip_config["dhcp-client-id"] = ip_config.dhcp_client_id_;
  }
  return json_ip_config;
}

json IPConfigsToNJson(const IPConfigs &ip_configs) {
  json json_out({});
  for (const auto &ip_config : ip_configs) {
    json_out[Interface::LabelFromName(ip_config.interface_)] = IpConfigToNJson(ip_config);
  }
  return json_out;
}

json BridgeConfigToNJson(const BridgeConfig &bridge_config, const FirmwareVersion &target_fw) {
  ::std::map<::std::string, ::std::vector<::std::string>> cfg;
  for (auto &bridge : bridge_config) {
    ::std::vector<::std::string> interface_labels;
    for (auto &interface : bridge.second) {
      interface_labels.emplace_back(interface.GetLabel());
    }
    cfg.emplace(bridge.first.GetName(), interface_labels);
  }
  auto j = json(cfg);

  if (target_fw.IsEqualToMajorMinorOf(FIRMWARE_15)) {
    /* FW 15 needs br1 in bridge config in order to work correctly */
    if (not j.contains("br1")) {
      j["br1"] = json::array();
    }
  }
  return j;
}

::std::string DipSwitchIPConfigToJson(const DipSwitchIpConfig &config) {
  return DipSwitchIpConfigToNJson(config).dump();
}

json DipSwitchConfigToNJson(const DipSwitchConfig &config) {
  return json{{"ipaddr", config.ip_config_.address_},
              {"netmask", config.ip_config_.netmask_},
              {"value", config.value_},
              {"mode", config.mode_}};
}

::std::string DipSwitchConfigToJson(const DipSwitchConfig &config) {
  return DipSwitchConfigToNJson(config).dump();
}

Status JsonToNJson(::std::string const &json_str, json &json_object) {
  try {
    class throwing_sax : public nlohmann::detail::json_sax_dom_parser<nlohmann::json> {
     public:
      explicit throwing_sax(nlohmann::json &j) : nlohmann::detail::json_sax_dom_parser<nlohmann::json>(j) {
      }

      bool key(json::string_t &val) {
        if (ref_stack.back()->contains(val)) {
          throw std::invalid_argument("key " + val + " was already stored");
        }
        return nlohmann::detail::json_sax_dom_parser<nlohmann::json>::key(val);
      }
    };
    throwing_sax sax_consumer(json_object);
    json::sax_parse(json_str, &sax_consumer);

  } catch (std::exception const &e) {
    return Status{StatusCode::JSON_CONVERT, e.what()};
  }
  return {};
}

Status GetAddressFromJson(const std::string &json_field, const json &from, std::string &to) {
  try {
    auto iter = from.find(json_field);
    if (iter != from.end()) {
      iter.value().get_to(to);

      boost_error error_code;
      boost_address::from_string(to, error_code);
      if (error_code) {
        return Status{StatusCode::IPV4_FORMAT, ::std::to_string(error_code.value())};
      }
    }

  } catch (std::exception const &e) {
    to.clear();
    return Status{StatusCode::JSON_CONVERT, e.what()};
  } catch (...) {
    to.clear();
    return Status{StatusCode::JSON_CONVERT, "General JSON error"};
  }

  return {};
}

Status NJsonToBridgeConfig(const json &json_object, BridgeConfig &bridge_config) {
  Status status;

  for (const auto &bridge_cfg_json : json_object.items()) {
    Interfaces interfaces;
    for (const auto &label : bridge_cfg_json.value()) {
      interfaces.emplace_back(Interface::CreatePort(label));
    }

    ::std::pair<Interface, Interfaces> bridge_cfg_pair{
      Interface::CreateBridge(bridge_cfg_json.key()),
      ::std::move(interfaces)  // NOLINT: Need to express types in make_pair to get it working.
    };

//    auto bridge_cfg_pair = std::make_pair<Interface, Interfaces>(
//        Interface::CreateBridge(bridge_cfg_json.key()),
//        ::std::move(interfaces));  // NOLINT: Need to express types in make_pair to get it working.

    if (bridge_cfg_pair.first.GetName().empty()) {
      status.Set(StatusCode::NAME_EMPTY);
    } else if (bridge_config.count(bridge_cfg_pair.first) > 0) {
      status.Set(StatusCode::ENTRY_DUPLICATE, bridge_cfg_pair.first.GetName());
    }

    if (status.IsNotOk()) {
      bridge_config.clear();
      break;
    }
    bridge_config.insert(bridge_cfg_pair);
  }

  return status;
}

Status NJsonToDipIPConfig(const nlohmann::json &json_object, DipSwitchIpConfig &ip_config) {
  Status status;

  status = GetAddressFromJson("ipaddr", json_object, ip_config.address_);

  if (status.IsOk()) {
    status = GetAddressFromJson("netmask", json_object, ip_config.netmask_);
  }

  return status;
}

Status JsonToDipSwitchConfig(const ::std::string &json_string, DipSwitchConfig &out_obj) {
  json json_object;
  auto status = JsonToNJson(json_string, json_object);
  if (status.IsOk()) {
    status = GetToOrError("mode", json_object, out_obj.mode_);
  }
  if (status.IsOk()) {
    status = GetToOrError("value", json_object, out_obj.value_);
  }
  if (status.IsOk()) {
    status = GetToOrError("ipaddr", json_object, out_obj.ip_config_.address_);
  }
  if (status.IsOk()) {
    status = GetToOrError("netmask", json_object, out_obj.ip_config_.netmask_);
  }

  return status;
}

Status NJsonToIPConfigs(const json &json_object, IPConfigs &ip_configs) {
  try {
    auto items = json_object.items();
    for (auto &item : items) {
      ip_configs.emplace_back(ParseIpConfig(item));
    }
  } catch (std::exception const &e) {
    return Status{StatusCode::JSON_CONVERT, e.what()};
  } catch (...) {
    return Status{StatusCode::JSON_CONVERT, "General JSON error"};
  }

  return {};
}

static json InterfaceBaseToNJson(const InterfaceBase &interface_config) {
  json j;

  j[PORT_CFG_KEY_DEVICE] = interface_config.interface_.GetLabel();

  if (interface_config.state_ != InterfaceState::UNKNOWN) {
    j[PORT_CFG_KEY_STATE] = interface_config.state_;
  }

  if (interface_config.autoneg_ != Autonegotiation::UNKNOWN) {
    j[PORT_CFG_KEY_AUTONEG] = interface_config.autoneg_;
  }

  if (interface_config.speed_ > 0) {
    j[PORT_CFG_KEY_SPEED] = interface_config.speed_;
  }

  if (interface_config.duplex_ != Duplex::UNKNOWN) {
    j[PORT_CFG_KEY_DUPLEX] = interface_config.duplex_;
  }

  if (interface_config.mac_learning_ != MacLearning::UNKNOWN) {
    j[PORT_CFG_KEY_MAC_LEARNING] = interface_config.mac_learning_;
  }
  return j;
}

json InterfaceConfigToNJson(const InterfaceConfig &interface_config) {
  return InterfaceBaseToNJson(interface_config);
}

static json InterfaceConfigToNJson_lower_than_fw22(const InterfaceConfig &interface_config) {
  auto interface_config_without_maclearning          = interface_config;
  interface_config_without_maclearning.mac_learning_ = MacLearning::UNKNOWN;
  return InterfaceBaseToNJson(interface_config_without_maclearning);
}

json InterfaceStatusToNJson(const InterfaceStatus &interface_status) {
  json j = InterfaceBaseToNJson(interface_status);

  if (interface_status.link_state_ != LinkState::UNKNOWN) {
    j[PORT_CFG_KEY_LINKSTATE] = interface_status.link_state_;
  }

  j[PORT_CFG_KEY_MAC] = interface_status.mac_.ToString();

  return j;
}

static json InterfaceConfigToNJson_fw15(const InterfaceConfig &interface_config) {
  json j;

  j[PORT_CFG_KEY_DEVICE] = interface_config.interface_.GetLabel();

  if (interface_config.state_ != InterfaceState::UNKNOWN) {
    j[PORT_CFG_KEY_STATE] = interface_config.state_;
  }

  if (interface_config.autoneg_ != Autonegotiation::UNKNOWN) {
    j[PORT_CFG_KEY_AUTONEG] = (interface_config.autoneg_ == Autonegotiation::ON);
  }

  if (interface_config.speed_ > 0) {
    j[PORT_CFG_KEY_SPEED] = interface_config.speed_;
  }

  if (interface_config.duplex_ != Duplex::UNKNOWN) {
    j[PORT_CFG_KEY_DUPLEX] = interface_config.duplex_;
  }
  return j;
}

json InterfaceConfigsToNJson(const InterfaceConfigs &interface_configs, const FirmwareVersion &target_fw) {
  /* We changed the interface config json format in fw 16, so we need this ugly hack to generate a different config */

  auto interfaceConfigToNJsonFn =
      (target_fw.IsEqualToMajorMinorOf(FIRMWARE_15)) ? &InterfaceConfigToNJson_fw15
      : (target_fw.IsGreaterThanMajorMinorOf(FIRMWARE_15) && target_fw.IsLessThanMajorMinorOf(FIRMWARE_22))
          ? &InterfaceConfigToNJson_lower_than_fw22
          : &InterfaceConfigToNJson;

  if (interface_configs.size() > 1) {
    nlohmann::json jarray{};
    for (const auto &config : interface_configs) {
      jarray.push_back(interfaceConfigToNJsonFn(config));
    }
    return jarray;
  }

  return interface_configs.size() == 1 ? InterfaceConfigToNJson(interface_configs.at(0)) : nlohmann::json({});
}

json InterfaceStatusesToNJson(const InterfaceStatuses &interface_status) {
  if (interface_status.size() > 1) {
    nlohmann::json jarray{};
    for (const auto &config : interface_status) {
      jarray.push_back(InterfaceStatusToNJson(config));
    }
    return jarray;
  }

  return interface_status.size() == 1 ? InterfaceStatusToNJson(interface_status.at(0)) : nlohmann::json({});
}

static InterfaceBase InterfaceBaseFromJson(const json &config) {
  ::std::string label = config.at(PORT_CFG_KEY_DEVICE).get<::std::string>();

  //TODO(Team) At this point we have not device type informations. We use string compare to detect ports. Others are undefined.
  ::std::string name = Interface::NameFromLabel(label);
  Interface interface;
  if(name.rfind("ethX", 0) == 0){
    interface = Interface::CreatePort(name);
  }else{
    interface = Interface::CreateOther(name);
  }
  InterfaceBase p{interface};

  auto autoneg_opt = config.find(PORT_CFG_KEY_AUTONEG) != config.end();
  if (autoneg_opt) {
    p.autoneg_ = config.at(PORT_CFG_KEY_AUTONEG).get<Autonegotiation>();
  }

  auto duplex_opt = config.find(PORT_CFG_KEY_DUPLEX) != config.end();
  if (duplex_opt) {
    p.duplex_ = config.at(PORT_CFG_KEY_DUPLEX).get<Duplex>();
  }

  auto state_opt = config.find(PORT_CFG_KEY_STATE) != config.end();
  if (state_opt) {
    p.state_ = config.at(PORT_CFG_KEY_STATE).get<InterfaceState>();
  }

  auto speed_opt = config.find(PORT_CFG_KEY_SPEED) != config.end();
  if (speed_opt) {
    p.speed_ = config.at(PORT_CFG_KEY_SPEED).get<int>();
  }

  auto mac_learning_opt = config.find(PORT_CFG_KEY_MAC_LEARNING) != config.end();
  if (mac_learning_opt) {
    p.mac_learning_ = config.at(PORT_CFG_KEY_MAC_LEARNING).get<MacLearning>();
  }
  return p;
}

InterfaceConfig InterfaceConfigFromJson(const json &config) {
  return InterfaceBaseFromJson(config);
}

InterfaceStatus InterfaceStatusFromJson(const json &config) {
  InterfaceStatus p{InterfaceBaseFromJson(config)};

  auto link_opt = config.find(PORT_CFG_KEY_LINKSTATE) != config.end();
  if (link_opt) {
    p.link_state_ = config.at(PORT_CFG_KEY_LINKSTATE).get<LinkState>();
  }

  auto mac_opt = config.find(PORT_CFG_KEY_MAC) != config.end();
  if (mac_opt) {
    auto mac = config.at(PORT_CFG_KEY_MAC).get<::std::string>();
    p.mac_   = MacAddress::FromString(mac);
  }

  return p;
}

Status NJsonToInterfaceConfigs(const nlohmann::json &json_object, InterfaceConfigs &interface_configs) {
  if (json_object.empty()) {
    return Status{StatusCode::JSON_INCOMPLETE};
  }
  if (json_object.is_array()) {
    for (auto &jentry : json_object) {
      if (!isValidInterfaceConfigKey(jentry)) {
        return Status{StatusCode::JSON_INCOMPLETE};
      }
      interface_configs.push_back(InterfaceConfigFromJson(jentry));
    }
  } else {
    if (!isValidInterfaceConfigKey(json_object)) {
      return Status{StatusCode::JSON_INCOMPLETE};
    }
    interface_configs.push_back(InterfaceConfigFromJson(json_object));
  }
  return {};
}

Status NJsonToInterfaceStatuses(const nlohmann::json &json_object, InterfaceStatuses &interface_statuses) {
  if (json_object.empty()) {
    return Status{StatusCode::JSON_INCOMPLETE};
  }
  if (json_object.is_array()) {
    for (auto &jentry : json_object) {
      if (!isValidInterfaceConfigKey(jentry)) {
        return Status{StatusCode::JSON_INCOMPLETE};
      }
      interface_statuses.push_back(InterfaceStatusFromJson(jentry));
    }
  } else {
    if (!isValidInterfaceConfigKey(json_object)) {
      return Status{StatusCode::JSON_INCOMPLETE};
    }
    interface_statuses.push_back(InterfaceStatusFromJson(json_object));
  }
  return {};
}

static json LinkModesToNJson(const LinkModes &link_modes) {
  json array;
  for (auto link_mode : link_modes) {
    json j;
    if (link_mode.speed_ > 0) {
      j[PORT_CFG_KEY_SPEED] = link_mode.speed_;
    }
    if (link_mode.duplex_ != Duplex::UNKNOWN) {
      j[PORT_CFG_KEY_DUPLEX] = link_mode.duplex_;
    }

    array.push_back(j);
  }
  return array;
}

static void InsertDeviceTypeSpecificInterfaceProperties(const Interface &interface, json &json_obj) {
    switch (interface.GetType()) {
      case DeviceType::Vlan:
        try {
          if (interface.GetProperties().count(InterfaceProperty::VlanID) > 0) {
            auto vlan_id = ::boost::get<int>(interface.GetProperties().at(InterfaceProperty::VlanID));
            json_obj[INTERFACE_KEY_VLANID] = vlan_id;
          }
          if (interface.GetProperties().count(InterfaceProperty::Link) > 0) {
            auto vlan_link = ::boost::get<::std::string>(interface.GetProperties().at(InterfaceProperty::Link));
            json_obj[INTERFACE_KEY_VLAN_LINKED_INTERFACE] = vlan_link;
          }
        } catch (::boost::bad_get& e) {
          // TODO(Team): Log error
//          LOG_ERROR("Failed to fill json for vlan interface \"" + interface.GetName() + "\". Failed to get vlan properties from map.");
        }
        break;
      default:
        break;
    }
}

json InterfaceInformationToNJson(const InterfaceInformation &interface_information) {
  auto interface = interface_information.GetInterface();
  json json_obj{{"name", interface.GetName()},
                {"label", interface.GetLabel()},
                {"type", interface_information.GetType()},
                {"ipreadonly", interface_information.IsIpConfigurationReadonly()}};

  if (interface_information.GetAutonegSupported() != AutonegotiationSupported::UNKNOWN) {
    json_obj[PORT_CFG_KEY_AUTONEG_SUPPORTED] = interface_information.GetAutonegSupported();
  }

  auto link_modes = interface_information.GetSupportedlinkModes();
  if (!link_modes.empty()) {
    json_obj[PORT_CFG_KEY_SUPPORTED_LINK_MODES] = LinkModesToNJson(link_modes);
  }

  InsertDeviceTypeSpecificInterfaceProperties(interface, json_obj);

  return json_obj;
}

json InterfaceToNJson(const Interface &interface) {
  json json_interface{{INTERFACE_KEY_NAME, interface.GetLabel()}, {INTERFACE_KEY_TYPE, ToString(interface.GetType())}};

  InsertDeviceTypeSpecificInterfaceProperties(interface, json_interface);

  return json_interface;
}

json InterfacesToNJson(const Interfaces &interfaces) {
  auto json_out = json::array();
  for (const auto &interface : interfaces) {
    json_out.push_back(InterfaceToNJson(interface));
  }
  return json_out;
}

static void parse_link_modes(const json &json_object, LinkModes &link_modes) {
  if (json_object.find(PORT_CFG_KEY_SUPPORTED_LINK_MODES) != json_object.end()) {
    for (auto &link_mode : json_object[PORT_CFG_KEY_SUPPORTED_LINK_MODES]) {
      LinkMode lm;
      auto duplex_opt = link_mode.find(PORT_CFG_KEY_DUPLEX) != link_mode.end();
      if (duplex_opt) {
        lm.duplex_ = link_mode.at(PORT_CFG_KEY_DUPLEX).get<Duplex>();
      }

      auto speed_opt = link_mode.find(PORT_CFG_KEY_SPEED) != link_mode.end();
      if (speed_opt) {
        lm.speed_ = link_mode.at(PORT_CFG_KEY_SPEED).get<int>();
      }

      link_modes.emplace_back(lm);
    }
  }
}

Status NJsonToInterfaceInformation(const json &json_object, InterfaceInformation &interface_information) {
  ::std::string name;
  bool ip_config_ro                          = true;
  AutonegotiationSupported autoneg_supported = AutonegotiationSupported::UNKNOWN;
  LinkModes link_modes{};

  ::std::string label;
  Status status = GetToOrError("label", json_object, label);
  if (status.IsOk()) {
    status = GetToOrError("name", json_object, name);
  }
  if (status.IsOk()) {
    status = GetToOrError("ipreadonly", json_object, ip_config_ro);
  }
  if (status.IsOk()) {
    GetToIfExists(PORT_CFG_KEY_AUTONEG_SUPPORTED, json_object, autoneg_supported);
  }
  if (status.IsOk()) {
    parse_link_modes(json_object, link_modes);
  }

  if (status.IsOk()) {
    Interface interface;
    status = NJsonToInterface(json_object, interface);

    interface_information = InterfaceInformation{interface, ip_config_ro, autoneg_supported, link_modes};
  }

  return status;
}

Status NJsonToInterfaces(const json &json_array, Interfaces &interfaces) {
  Status status;

  if (!json_array.is_array()) {
    return Status{StatusCode::JSON_CONVERT, "Json string does not match the expected format."};
  }

  for (const auto &j_item : json_array.items()) {
    Interface interface;
    status = NJsonToInterface(j_item.value(), interface);
    if (status.IsNotOk()) {
      break;
    }
    interfaces.push_back(interface);
  }

  if (status.IsNotOk()) {
    interfaces.clear();
  }
  return status;
}

}  // namespace netconf
