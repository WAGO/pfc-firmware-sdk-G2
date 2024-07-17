//------------------------------------------------------------------------------
/// Copyright (c) 2020-2024 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Class representation of a dhcp ip range
///
///  \author   MSc : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef SRC_CONFIG_DNSMASQ_DHCPRANGE_HPP_
#define SRC_CONFIG_DNSMASQ_DHCPRANGE_HPP_

#include <cstdint>
#include <string>

#include "ip_address.hpp"

namespace configdnsmasq {

class dhcp_range {
 public:
  ip_address lower_ip_;
  ip_address upper_ip_;
  ip_address netmask_;

  static dhcp_range fromRangeString(const std::string &range, uint32_t netmask);
  static dhcp_range fromRangeString(const std::string &range, const std::string &netmask);

  dhcp_range(ip_address lower_ip, ip_address upper_ip, uint32_t netmask);
  dhcp_range(uint32_t lower_ip, uint32_t upper_ip, uint32_t netmask);
  dhcp_range(const std::string &lower_ip, const std::string &upper_ip, const std::string &netmask);
  dhcp_range() : dhcp_range(0, 0, 0) {}
  dhcp_range(const dhcp_range& other) = default;
  dhcp_range& operator=(const dhcp_range& other) = default;
  dhcp_range(dhcp_range&& other) = default;
  dhcp_range& operator=(dhcp_range&& other) = default;
  virtual ~dhcp_range() = default;

  uint32_t getLowerNetwork() const;
  uint32_t getLowerHost() const;
  uint32_t getUpperNetwork() const;
  uint32_t getUpperHost() const;

  void resetNetwork(uint32_t network);

  bool isValid() const;
  std::string toString() const;
 private:
  void validate() const;
};

} // namespace configdnsmasq

#endif /* SRC_CONFIG_DNSMASQ_DHCPRANGE_HPP_ */
