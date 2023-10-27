// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IDynamicIPClient.hpp"
#include "LeaseFile.hpp"

#include <string>
#include <glib.h>

namespace netconf {

class BOOTPClient : public IDynamicIPClient {
 public:
  explicit BOOTPClient(Interface interface);
  ~BOOTPClient() override;

  BOOTPClient(const BOOTPClient &other) = delete;
  BOOTPClient(BOOTPClient &&other) = delete;
  BOOTPClient& operator=(const BOOTPClient &other) = delete;
  BOOTPClient& operator=(BOOTPClient &&other) = delete;

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
  ::std::string GetClientID() override {
    return "";
  }

 private:
  LeaseFile lease_file_;

  Interface interface_;
  ::std::string pid_file_path_;

  GPid pid_;
  const ::std::string BOOTP_SCRIPT_PATH = "/sbin/bootpc-startup";

  void Start();
  void Stop();
};

} /* namespace netconf */
