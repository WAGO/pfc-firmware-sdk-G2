// SPDX-License-Identifier: GPL-2.0-or-later

#include "DynamicIPClientFactory.hpp"

#include "BOOTPClient.hpp"
#include "DHCPClient.hpp"
#include "LeaseFile.hpp"

namespace netconf {

IDynamicIPClientPtr DynamicIPClientFactory::Create(DynamicIPType type, const Interface &interface,
                                                   const ::std::string &hostname, const ::std::string &vendorclass,
                                                   const ::std::string &clientid) {
  if (type == DynamicIPType::DHCP) {
    return ::std::make_shared<DHCPClient>(interface, hostname, vendorclass, clientid);
  }
  if (type == DynamicIPType::BOOTP) {
    return ::std::make_shared<BOOTPClient>(interface);
  }
  return nullptr;
}

} /* namespace netconf */
