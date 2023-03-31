// SPDX-License-Identifier: GPL-2.0-or-later

#include "DeviceTypeLabel.hpp"

#include <boost/foreach.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iomanip>

#include "CommandExecutor.hpp"
#include "EthernetInterface.hpp"
#include "Logger.hpp"

namespace netconf {

using boost::property_tree::ptree;
using namespace std::literals;

DeviceTypeLabel::DeviceTypeLabel(CommandExecutor& command_executer) : command_executer_{command_executer} {
  ::std::string type_label_ini;
  Status status = command_executer_.Execute("/etc/config-tools/get_typelabel_value -a", type_label_ini);

  if (status.IsNotOk()) {
    LogError("Failed to extract typelabel values! Taking fallback values."s);
    GetMacFallback();
    GetOrderFallback();
  }

  try {
    ptree type_label_values;
    ::std::istringstream type_label_stringstream(type_label_ini);

    boost::property_tree::read_ini(type_label_stringstream, type_label_values);

    order_number_ = type_label_values.get<::std::string>("ORDER");
    mac_          = MacAddress::FromString(type_label_values.get<::std::string>("MAC"));

    if (type_label_values.count("MAC_ID_INC") == 0) {
      mac_count_ = 1;
    } else {
      mac_count_ = type_label_values.get<uint32_t>("MAC_ID_INC");
    }

  } catch (std::exception& e) {
    LogError("Failed to extract typelabel values! Taking fallback values. ("s.append(e.what()).append(")"));
    if (order_number_.empty()) {
      GetOrderFallback();
    }
    GetMacFallback();
  }
}

::std::string DeviceTypeLabel::GetOrderNumber() const {
  return order_number_;
}

MacAddress DeviceTypeLabel::GetMac() const {
  return mac_;
}

uint32_t DeviceTypeLabel::GetMacCount() const {
  return mac_count_;
}

void DeviceTypeLabel::GetMacFallback() {
  EthernetInterface eth0If{"eth0"};
  mac_ = eth0If.GetMac();
}

void DeviceTypeLabel::GetOrderFallback() {
  order_number_ = "wago-pfc";
}

}  // namespace netconf
