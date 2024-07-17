// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "info_parser.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

#include "stp.hpp"

namespace wago::stp::lib {

namespace {

status json_to_njson(::std::string const& json_str, nlohmann::json& json_object) {
  try {
    class throwing_sax : public nlohmann::detail::json_sax_dom_parser<nlohmann::json> {
     public:
      explicit throwing_sax(nlohmann::json& j) : nlohmann::detail::json_sax_dom_parser<nlohmann::json>(j) {
      }

      bool key(nlohmann::json::string_t& val) {
        if (ref_stack.back()->contains(val)) {
          throw std::invalid_argument("key " + val + " was already stored");
        }
        return nlohmann::detail::json_sax_dom_parser<nlohmann::json>::key(val);
      }
    };
    throwing_sax sax_consumer(json_object);
    nlohmann::json::sax_parse(json_str, &sax_consumer);

  } catch (std::exception const& e) {
    return status{status_code::JSON_PARSE_ERROR, e.what()};
  }
  return {};
}

template <typename T>
bool get_to(const ::std::string& key, const nlohmann::json& inner_json, T& to) {
  auto iter = inner_json.find(key);
  if (iter != inner_json.end()) {
    iter.value().get_to(to);
    return true;
  }
  return false;
}

template <typename T>
bool get_string_to_int(const ::std::string& key, const nlohmann::json& inner_json, T& to) {
  auto iter = inner_json.find(key);
  if (iter != inner_json.end()) {
    ::std::string v = iter.value();
    to              = static_cast<T>(::std::stoi(v));
    return true;
  }
  return false;
}

template <typename T>
bool get_string_to_bool(const ::std::string& key, const nlohmann::json& inner_json, T& to) {
  auto iter = inner_json.find(key);
  if (iter != inner_json.end()) {
    ::std::string v = iter.value();
    to              = v == "yes";
    return true;
  }
  return false;
}

status njson_to_bridge_info(nlohmann::json& j, stp_info& info) {
  if (j.empty()) {
    return status{status_code::JSON_PARSE_ERROR, "empty json string"};
  }
  if (not j.is_array()) {
    return status{status_code::JSON_PARSE_ERROR, "wrong json content"};
  }

  auto bridge_info = j.front();

  get_to("bridge", bridge_info, info.bridge);
  get_to("bridge-id", bridge_info, info.priority);
  get_to("force-protocol-version", bridge_info, info.protocol);

  // INFO: Here we have to deviate from the standard json parsing
  //  because integers in mstpctl are not specified as integers but as strings
  get_string_to_int("forward-delay", bridge_info, info.forward_delay);
  get_string_to_int("hello-time", bridge_info, info.hello_time);
  get_string_to_int("max-age", bridge_info, info.max_age);
  get_string_to_int("max-hops", bridge_info, info.max_hops);
  get_string_to_int("path-cost", bridge_info, info.path_cost);
  return {};
}

status njson_to_port_info(nlohmann::json& j, stp_info& info) {
  if (j.empty()) {
    return status{status_code::JSON_PARSE_ERROR, "empty json string"};
  }
  if (not j.is_array()) {
    return status{status_code::JSON_PARSE_ERROR, "wrong json content"};
  }

  for (auto& port_info : j) {
    ::std::string port;
    get_to("port", port_info, port);
    if (!port.empty()) {
      stp_port_info pi;
      pi.port = port;

      get_to("role", port_info, pi.role);
      get_to("state", port_info, pi.status);
      get_to("port-id", port_info, pi.priority);

      // INFO: Here we have to deviate from the standard json parsing
      //  because integers in mstpctl are not specified as integers but as strings
      get_string_to_bool("admin-edge-port", port_info, pi.edge_port);
      get_string_to_bool("bpdu-filter-port", port_info, pi.bpdu_filter);
      get_string_to_bool("bpdu-guard-port", port_info, pi.bpdu_guard);
      get_string_to_bool("restricted-role", port_info, pi.root_guard);
      get_string_to_int("external-port-cost", port_info, pi.path_cost);  // TODO(Team): is external-port-cost correct???

      info.ports.emplace_back(pi);
    }
  }
  return status{};
}

}  // namespace

status parse_stp_info(::std::string& json_str, stp_info& info) {
  status s;
  try {
    nlohmann::json json_object;
    s = json_to_njson(json_str, json_object);
    if (s.ok()) {
      s = njson_to_bridge_info(json_object, info);
    }
    if (s.ok()) {
      s = njson_to_port_info(json_object, info);
    }

  } catch (std::exception const& e) {
    s = status{status_code::JSON_PARSE_ERROR, e.what()};
  } catch (...) {
    s = status{status_code::JSON_PARSE_ERROR, "general JSON error"};
  }
  return s;
}

}  // namespace wago::stp::lib