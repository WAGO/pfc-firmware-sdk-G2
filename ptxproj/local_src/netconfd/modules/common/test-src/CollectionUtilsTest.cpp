// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommonTestDependencies.hpp"

#include <string>

#include "CollectionUtils.hpp"
#include "BaseTypes.hpp"
#include "Types.hpp"

namespace netconf {

TEST(CollectionUtilsTest, ReduceCevtorByValues) {

  Interfaces interfaces_to_be_reduced {
    { Interface::CreateDummy("dummy0") },
    { Interface::CreateVLAN("vlan0", 100, "br0") },
    { Interface::CreateDummy("dummy1") },
    { Interface::CreateVLAN("vlan1", 101, "br1") },
    { Interface::CreateDummy("dummy2") },
    { Interface::CreateVLAN("vlan2", 102, "br2") } };

  Interfaces interfaces {
    { Interface::CreateDummy("dummy1") },
    { Interface::CreateVLAN("vlan1", 101, "br1") },
    { Interface::CreateVLAN("XXX", 111, "br123") }};

  ReduceVectorByValues(interfaces_to_be_reduced, interfaces);
  EXPECT_EQ(4, interfaces_to_be_reduced.size());

  Interfaces expected_interfaces {
    { Interface::CreateDummy("dummy0") },
    { Interface::CreateVLAN("vlan0", 100, "br0") },
    { Interface::CreateDummy("dummy2") },
    { Interface::CreateVLAN("vlan2", 102, "br2") } };
  EXPECT_EQ(expected_interfaces, interfaces_to_be_reduced);

}

TEST(CollectionUtilsTest, Concatenate){
  Interfaces first {
  { Interface::CreateDummy("dummy0") },
  { Interface::CreateVLAN("vlan0", 100, "br0") }};

  Interfaces secound {
  { Interface::CreateDummy("dummy1") },
  { Interface::CreateVLAN("vlan1", 101, "br1") }};

  auto concatenated = Concatenate(first, secound);
  EXPECT_EQ(4, concatenated.size());

  Interfaces expected {
  { Interface::CreateDummy("dummy0") },
  { Interface::CreateVLAN("vlan0", 100, "br0") },
  { Interface::CreateDummy("dummy1") },
  { Interface::CreateVLAN("vlan1", 101, "br1") }};
  EXPECT_EQ(expected, concatenated);

}


}
