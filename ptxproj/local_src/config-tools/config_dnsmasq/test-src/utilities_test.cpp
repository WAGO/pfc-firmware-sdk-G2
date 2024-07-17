//------------------------------------------------------------------------------
// Copyright (c) 2020-2024 WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------


#include <gtest/gtest.h>

#include "utilities.hpp"

TEST(test_utilities, check_mac_validity) {
  ::std::string mac = "00:30:de:ad:be:ef";
  bool is_valid = configdnsmasq::conv_is_valid_mac(mac);
  ASSERT_TRUE(is_valid);

  ::std::string mac_uppercase = "00:30:DE:AD:BE:EF";
  is_valid = configdnsmasq::conv_is_valid_mac(mac_uppercase);
  ASSERT_TRUE(is_valid);

  ::std::string mac_to_short = "00:30:de:ad:be:e";
  is_valid = configdnsmasq::conv_is_valid_mac(mac_to_short);
  ASSERT_FALSE(is_valid);

  ::std::string mac_to_long = "00:30:de:ad:be:eff";
  is_valid = configdnsmasq::conv_is_valid_mac(mac_to_long);
  ASSERT_FALSE(is_valid);

  ::std::string mac_none_hex = "00:30:de:ad:be:ex";
  is_valid = configdnsmasq::conv_is_valid_mac(mac_none_hex);
  ASSERT_FALSE(is_valid);

  ::std::string mac_invalid_delimiter = "00:30:de-ad:be:ef";
  is_valid = configdnsmasq::conv_is_valid_mac(mac_invalid_delimiter);
  ASSERT_FALSE(is_valid);

  ::std::string mac_broadcast = "FF:FF:FF:FF:FF:FF";
  is_valid = configdnsmasq::conv_is_valid_mac(mac_broadcast);
  ASSERT_FALSE(is_valid);

  ::std::string mac_multicast = "01:30:de:ad:be:ef";
  is_valid = configdnsmasq::conv_is_valid_mac(mac_multicast);
  ASSERT_FALSE(is_valid);
}
