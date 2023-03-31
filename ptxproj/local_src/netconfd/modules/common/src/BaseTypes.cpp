// SPDX-License-Identifier: LGPL-3.0-or-later
#include "BaseTypes.hpp"

namespace netconf {

Interface::Interface(::std::string name, DeviceType type, InterfacePropertyMap properties)
    : name_ { ::std::move(name) },
      type_ { type },
      properties_ { ::std::move(properties) } {
}

Interface::Interface(::std::string name, DeviceType type)
    : name_ { ::std::move(name) },
      type_ { type } {
}

bool Interface::operator==(const Interface &other) const {
  return (name_ == other.name_) and (type_ == other.type_) and (properties_ == other.properties_);
}

bool Interface::operator!=(const Interface &other) const {
  return not(*this == other);
}

bool Interface::operator>(const Interface &other) const {
  return name_ > other.name_;
}

bool Interface::operator<(const Interface &other) const {
  return name_ < other.name_;
}

DeviceType Interface::GetType() const {
  return type_;
}

::std::string Interface::GetLabel() const {
  return LabelFromName(name_);
}

::std::string Interface::GetName() const {
  return name_;
}

InterfacePropertyMap Interface::GetProperties() const {
  return properties_;
}

::std::string Interface::NameFromLabel(const ::std::string &label){
  if (not label.empty() && label.at(0) == 'X') {
    using namespace std::string_literals;
    return "eth"s.append(label);
  }
  return label;
}

::std::string Interface::LabelFromName(const ::std::string &name){
  ::std::string label = name;
  if (label.rfind("ethX", 0) == 0) {
    using namespace std::string_literals;
    label.erase(0, "eth"s.size());
  }
  return label;
}

Interface Interface::CreatePort(const ::std::string &name) {
  return Interface(NameFromLabel(name), DeviceType::Port);
}

Interface Interface::CreateBridge(const ::std::string &name) {
  return Interface(name, DeviceType::Bridge);
}

Interface Interface::CreateDummy(const ::std::string &name) {
  return Interface(name, DeviceType::Dummy);
}

Interface Interface::CreateEthernet(const ::std::string &name) {
  return Interface(name, DeviceType::Ethernet);
}

Interface Interface::CreateWwan(const ::std::string &name) {
  return Interface(name, DeviceType::Wwan);
}

Interface Interface::CreateLoopback(const ::std::string &name) {
  return Interface(name, DeviceType::Loopback);
}

Interface Interface::CreateVLAN(const ::std::string &name, int vlanid, const ::std::string &link) {
  InterfacePropertyMap properties { { InterfaceProperty::VlanID, vlanid }, { InterfaceProperty::Link, link } };
  return Interface(name, DeviceType::Vlan, properties);
}

Interface Interface::CreateOther(const ::std::string &name) {
  return Interface(name, DeviceType::Other);
}

}  // namespace netconf
