// SPDX-License-Identifier: GPL-2.0-or-later

#include "DeviceTypeLabel.hpp"

#include <boost/foreach.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <random>
#include <sstream>

#include "CommandExecutor.hpp"
#include "Logger.hpp"
#include "MacAddress.hpp"

namespace netconf {

using boost::property_tree::ptree;
using namespace std::literals;

namespace {

  ::std::string CreateRandomMac() {
    ::std::random_device rd;
    ::std::mt19937 gen(rd());
    ::std::uniform_int_distribution<> dis(0, 255);

    ::std::stringstream mac_stream;
    mac_stream << "02:30:DE:" << std::hex << dis(gen) << ":" << dis(gen) << ":" << dis(gen);
    return mac_stream.str();
  }
}

DeviceTypeLabel::DeviceTypeLabel(CommandExecutor& command_executer) :
  command_executer_{command_executer},
  order_number_{"wago-pfc"},
  mac_{MacAddress::FromString(CreateRandomMac())},
  mac_count_{1} {

  ::std::string type_label_ini;
  Status status = command_executer_.Execute("/etc/config-tools/get_typelabel_value -a", type_label_ini);


  if (status.IsNotOk()) {
    LogError("Failed to extract typelabel values! Taking fallback values."s);
  }

  try {
    ptree type_label_values;
    ::std::istringstream type_label_stringstream(type_label_ini);

    boost::property_tree::read_ini(type_label_stringstream, type_label_values);

    auto order_number = type_label_values.get_optional<::std::string>("ORDER");
    if (order_number) {
      order_number_ = order_number.value();
    } else {
      LogError("Failed to read ORDER from typelabel: using fallback " + order_number_ + " instead.");
    }

    auto mac = type_label_values.get_optional<::std::string>("MAC");
    if (mac) {
      auto tl_mac = MacAddress::FromString(mac.value());
      if (tl_mac.IsValid()) {
        mac_ = tl_mac;
      } else {
        LogError("Invalid MAC from typelabel: using fallback instead.");
      }
    } else {
      LogError("Failed to read MAC from typelabel: using fallback instead.");
    }

    mac_count_ = type_label_values.get_optional<uint32_t>("MAC_ID_INC").value_or(1);
  } catch (std::exception& e) {
    LogError("Failed to extract typelabel values! Taking fallback values. ("s.append(e.what()).append(")"));
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

}  // namespace netconf
