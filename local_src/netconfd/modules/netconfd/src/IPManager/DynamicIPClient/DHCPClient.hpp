// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <glib.h>
#include <string>

#include "IDynamicIPClient.hpp"
#include "LeaseFile.hpp"

namespace netconf {

enum class DHCPClientStatus {
  RUNNING,
  STOPPED,
};

class DHCPClient : public IDynamicIPClient {
 public:
  DHCPClient(::std::string itf_name, ::std::string hostname, ::std::string vendorclass,
             ::std::string clientid);
  virtual ~DHCPClient();

  DHCPClient(const DHCPClient &other)            = delete;
  DHCPClient(DHCPClient &&other)                 = delete;
  DHCPClient &operator=(const DHCPClient &other) = delete;
  DHCPClient &operator=(DHCPClient &&other)      = delete;

  void Release() override;
  void Renew() override;

  DynamicIPType GetType() override;

  void RestartWithHostname(::std::string hostname) override;
  void RestartWithClientID(::std::string clientID) override;

  void UpdateContentFromLease() override;
  Address GetAddressFromLease() override;
  Netmask GetNetmaskFromLease() override;
  ::std::string GetHostnameFromLease() override;
  ::std::string GetDomainFromLease() override;

 private:
  const ::std::string DHCP_CLIENT_PATH = "/sbin/udhcpc";

  LeaseFile lease_file_;

  GPid pid_;
  ::std::string itf_name_;
  ::std::string hostname_;
  ::std::string vendorclass_;
  ::std::string pid_file_path_;
  ::std::string clientID_;

  void Start();
  void Stop();

};

} /* namespace netconf */
