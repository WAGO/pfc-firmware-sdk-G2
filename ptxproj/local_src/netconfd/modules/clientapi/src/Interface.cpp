// SPDX-License-Identifier: LGPL-3.0-or-later
#include "Interface.hpp"

#include "JsonConverter.hpp"
#include "NetconfdDbusClient.hpp"

namespace netconf {
namespace api {

static ::std::string ToJson(const Interface &interface) noexcept {
  JsonConverter jc;
  return jc.ToJsonString(interface);
}

Status MakeInterface(const ::std::string &json_str, Interface& interface) {
  JsonConverter jc;
  netconf::Interface itf;
  Status status = jc.FromJsonString(json_str, itf);
  interface = itf;
  return status;
}

Status AddInterface(const Interface &interface) {
  NetconfdDbusClient client;
  auto result = client.AddInterface(ToJson(interface));
  return result.error_;
}

Status DeleteInterface(const Interface &interface) {
  NetconfdDbusClient client;
  auto result = client.DeleteInterface(ToJson(interface));
  return result.error_;
}

}  // namespace api
}  // namespace netconf
