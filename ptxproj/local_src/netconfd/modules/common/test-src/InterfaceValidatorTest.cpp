// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "CommonTestDependencies.hpp"

#include "InterfaceValidator.hpp"

#include <string>

namespace netconf {

namespace {

Status Validate(const Interface &interface, const Interfaces &existing_interfaces) {
  return Validate(interface, existing_interfaces, existing_interfaces );
}

}

TEST(InterfaceValidator, CheckInterfaceNameSize) {
  Interfaces existing_interfaces = { Interface::CreateBridge("br0") };

  Interface interface = Interface::CreateDummy("name_tooooo_long");
  Status status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);

  interface = Interface::CreateVLAN("name_tooooo_long", 100, "br0");
  status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);

  interface = Interface::CreateVLAN("name_not_to_lon", 100, "br0");
  status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::OK);
}

TEST(InterfaceValidator, CheckIfInterfaceIsAddable) {
  Interfaces existing_interfaces;

  Interface interface = Interface::CreateBridge("bridge0");
  Status status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);

  interface = Interface::CreatePort("port1");
  status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);

  interface = Interface::CreateDummy("dummy42");
  status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::OK);
}

TEST(InterfaceValidator, CheckIfInterfaceNameIsUnique) {
  Interfaces existing_interfaces { Interface::CreateBridge("br0"), Interface::CreateDummy("dummy42") };

  Interface interface = Interface::CreateDummy("dummy42");
  Status status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);

  interface = Interface::CreateDummy("br0");
  status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);
}

TEST(InterfaceValidator, CheckIfVlanIDIsValid) {
  Interfaces existing_interfaces = { Interface::CreateBridge("br0") };

  Interface interface = Interface::CreateVLAN("virt0", -123, "br0");
  Status status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);

  interface = Interface::CreateVLAN("virt0", 0, "br0");
  status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);

  interface = Interface::CreateVLAN("virt0", 4095, "br0");
  status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);

  interface = Interface::CreateVLAN("virt0", 1234, "br0");
  status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::OK);
}

TEST(InterfaceValidator, CheckIfLinkedInterfaceIsValid) {
  Interfaces existing_interfaces = { Interface::CreateBridge("br0") };

  Interface interface = Interface::CreateVLAN("virt0", 123, "");
  Status status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);

  interface = Interface::CreateVLAN("virt0", 123, "br6");
  status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);

  interface = Interface::CreateVLAN("virt0", 123, "br0");
  status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::OK);
}

TEST(InterfaceValidator, CheckIfVlanIDIsDuplicate) {
  Interfaces existing_interfaces = { Interface::CreateBridge("br0"), Interface::CreateVLAN("virt0", 123, "br0") };

  Interface interface = Interface::CreateVLAN("virt2", 123, "br0");
  Status status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);
}

TEST(InterfaceValidator, CheckIfVlanExistWithOtherParameters) {
  Interfaces existing_interfaces = { Interface::CreateBridge("br0"), Interface::CreateBridge("br1"),
      Interface::CreateVLAN("virt1", 123, "br1") };

  Interface interface = Interface::CreateVLAN("virt0", 123, "br0");
  Status status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);

  interface = Interface::CreateVLAN("virt1", 1234, "br1");
  status = Validate(interface, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);
}

TEST(InterfaceValidator, CheckIfMaximumNumberOfVlanInterfacesIsExceeded) {
  Interfaces existing_interfaces { Interface::CreateBridge("br0"), Interface::CreateDummy("dummy1")};

  for(int i = 0 ; i < 15; i++) {
    existing_interfaces.push_back(Interface::CreateVLAN("virt" + ::std::to_string(i), 100 + i, "br0"));
  }

  Interface interface = Interface::CreateVLAN("virt15", 115, "br0");
  Status status = Validate(interface, existing_interfaces, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::OK);

  existing_interfaces.push_back(Interface::CreateVLAN("virt15", 115, "br0"));
  interface = Interface::CreateVLAN("virt16", 116, "br0");
  status = Validate(interface, existing_interfaces, existing_interfaces);
  EXPECT_EQ(status.GetStatusCode(), StatusCode::INTERFACE_VALIDATION);
}

}  // namespace netconf
