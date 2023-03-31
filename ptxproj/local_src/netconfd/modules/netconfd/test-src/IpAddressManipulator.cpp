// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------


#include "CommonTestDependencies.hpp"

#include "IpAddressManipulator.hpp"


namespace netconf {

class AnIpAddressManipulator : public ::testing::Test
{
  public:

  IPConfig test_config;

  void SetAddressToConfig(const Address& address)
  {
    test_config.address_ = address;
  }

  void SetNetmaskToConfig(const Netmask& netmask)
  {
    test_config.netmask_ = netmask;
  }
};

TEST_F(AnIpAddressManipulator, DoesNotChangeZeroAddress)
{

  SetAddressToConfig(ZeroIP);
  SetNetmaskToConfig(ZeroNetmask);


  auto new_config = IpAddressManipulator::ChangeLastAddressOctet(test_config, 123);

  EXPECT_EQ(ZeroIP, new_config.address_);

}

TEST_F(AnIpAddressManipulator, ChangesHostPartOfClassCAddress)
{

  SetAddressToConfig("192.168.5.42");
  SetNetmaskToConfig("255.255.255.0");


  auto new_config = IpAddressManipulator::ChangeLastAddressOctet(test_config, 123);

  EXPECT_STREQ("192.168.5.123", new_config.address_.c_str());

}

TEST_F(AnIpAddressManipulator, ChangesHostPartOfSmallSubnet)
{

  SetAddressToConfig("192.168.5.5");
  SetNetmaskToConfig("255.255.255.240");


  auto new_config = IpAddressManipulator::ChangeLastAddressOctet(test_config, 123);

  EXPECT_STREQ("192.168.5.123", new_config.address_.c_str());

}

TEST_F(AnIpAddressManipulator, ChangesHostPartOfBiggerSubnet)
{

  SetAddressToConfig("192.168.5.5");
  SetNetmaskToConfig("255.255.128.0");


  auto new_config = IpAddressManipulator::ChangeLastAddressOctet(test_config, 123);

  EXPECT_STREQ("192.168.5.123", new_config.address_.c_str());

}

}  // namespace netconf


//---- End of source file ------------------------------------------------------
