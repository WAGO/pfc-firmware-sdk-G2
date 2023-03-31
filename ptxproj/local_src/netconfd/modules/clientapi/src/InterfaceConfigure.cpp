// SPDX-License-Identifier: LGPL-3.0-or-later

#include "InterfaceConfig.hpp"

#include <boost/algorithm/string/join.hpp>
#include <vector>
#include <sstream>

#include "NetconfdDbusClient.hpp"
#include "EthernetInterface.hpp"
#include "JsonConverter.hpp"
#include "Convert.hpp"

namespace netconf {
namespace api {

Status SetInterfaceConfigs(const InterfaceConfigs &config) {
  NetconfdDbusClient client;
  auto result = client.SetInterfaceConfigs(ToJson(config));
  return result.error_;
}

Status GetInterfaceConfigs(InterfaceConfigs& config) {
  NetconfdDbusClient client;
  auto result = client.GetInterfaceConfigs();

  auto error = result.error_;
  if(error.IsOk()){
    JsonConverter jc;
    netconf::InterfaceConfigs nic;
    error = jc.FromJsonString(result.value_json_, nic);
    config = InterfaceConfigs{nic};
  }

  return error;
}

Status GetInterfaceStatuses(InterfaceStatuses &statuses){
  NetconfdDbusClient client;
  auto result = client.GetInterfaceStatuses();

  auto error = result.error_;
  if(error.IsOk()){
    JsonConverter jc;
    netconf::InterfaceStatuses nis;
    error = jc.FromJsonString(result.value_json_, nis);
    statuses = InterfaceStatuses{nis};
  }

  return error;
}


::std::string GetMacAddress(const ::std::string &interface) {
  EthernetInterface eth_interface { interface };
  return eth_interface.GetMac().ToString();
}

Status SetInterfaceState(const ::std::string &interface, InterfaceState state)
{
  try{
    EthernetInterface eth_interface { interface};
    eth_interface.SetState(state);
  }catch(::std::exception& e){
    return Status {StatusCode::SYSTEM_CALL, e.what()};
  }
  return {};
}

Status GetInterfaceState(const ::std::string &interface, InterfaceState &state)
{
  try{
    EthernetInterface eth_interface { interface};
    state = eth_interface.GetState();
  }catch(::std::exception& e){
    return Status {StatusCode::SYSTEM_CALL, e.what()};
  }
  return {};
}



}  // namespace api
}  // namespace netconf
