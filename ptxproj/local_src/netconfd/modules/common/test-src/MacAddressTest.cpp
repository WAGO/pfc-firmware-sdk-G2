// SPDX-License-Identifier: LGPL-3.0-or-later

#include "CommonTestDependencies.hpp"

#include <string>

#include <nlohmann/json.hpp>
#include <gsl/gsl>

#include "MacAddress.hpp"

namespace netconf {

TEST(MacAddressTest, MacAddressToString)
{
  const uint8_t macbytes[] = {0x01,0x02,0x03,0x04,0x05,0x06};
  const uint8_t macbytes2[] = {0xAB,0xBC,0xCD,0xDE,0xEF,0xFF};
  MacAddress mac(macbytes);
  MacAddress mac2(macbytes2);

  EXPECT_EQ("01:02:03:04:05:06", mac.ToString());
  EXPECT_EQ("01-02-03-04-05-06", mac.ToString('-'));
  EXPECT_EQ("AB:BC:CD:DE:EF:FF", mac2.ToString());
  EXPECT_EQ("AB-BC-CD-DE-EF-FF", mac2.ToString('-'));
}

TEST(MacAddressTest, MacAddressFromString)
{
  MacAddress expected{{0x01,0x02,0x03,0x04,0x05,0x06}};
  MacAddress converted = MacAddress::FromString("01:02:03:04:05:06");
  MacAddress converted_dash = MacAddress::FromString("01-02-03-04-05-06",'-');
  EXPECT_EQ(expected, converted);
  EXPECT_EQ(expected, converted_dash);

  EXPECT_THROW(MacAddress::FromString("01:02:03:04:05"), std::invalid_argument);
  EXPECT_THROW(MacAddress::FromString("01:02:03:04:05:06",'-'), std::invalid_argument);
  EXPECT_THROW(MacAddress::FromString("010203040506"), std::invalid_argument);
  EXPECT_THROW(MacAddress::FromString("01:02:03:0g:05:06",'-'), std::invalid_argument);
}

//TEST(MacAddressTest, MacAddressCtors)
//{
//  uint8_t bytes[] = {0x01,0x02,0x03,0x04,0x05,0x06};
//  MacAddress created{&bytes[0]};
//  EXPECT_EQ("01:02:03:04:05:06", created.ToString());
//
//
//}

TEST(MacAddressTest, MacAddressIncrement){
  MacAddress base1{{0x01,0x02,0x03,0x04,0x05,0x06}};
  EXPECT_EQ("01:02:03:04:05:07",base1.Increment(1).ToString());

  MacAddress base2{{0x01,0x02,0x03,0x04,0xFF,0xFF}};
  EXPECT_EQ("01:02:03:05:00:04",base2.Increment(5).ToString());

  MacAddress base3{{0x01,0x02,0x03,0x04,0xFF,0xFF}};
  EXPECT_EQ("01:02:03:04:FF:FF",base3.Increment(0).ToString());

  MacAddress base4{{0x01,0x02,0x03,0xFF,0xFF,0xFF}};
  EXPECT_EQ("01:02:03:00:00:00",base4.Increment(1).ToString());

}

TEST(MacAddressTest, CheckEqualityOfMAC){
  MacAddress mac1{{0x01,0x02,0x03,0x04,0x05,0x06}};
  MacAddress mac2{{0x01,0x02,0x03,0x04,0x05,0x06}};

  EXPECT_TRUE(mac1 == mac2);

  MacAddress mac3{{0x01,0x02,0x03,0x04,0x05,0x07}};

  EXPECT_FALSE(mac1 == mac3);
}

TEST(MacAddressTest, ConstructMacaddressFromSpan){
  uint8_t mac[] = {0x01,0x02,0x03,0x04,0x05,0x06};

  MacAddress m = MacAddress{::gsl::make_span(mac)};

  MacAddress expected_mac{{0x01,0x02,0x03,0x04,0x05,0x06}};

  EXPECT_TRUE(m == expected_mac);
}




}  // namespace netconf

