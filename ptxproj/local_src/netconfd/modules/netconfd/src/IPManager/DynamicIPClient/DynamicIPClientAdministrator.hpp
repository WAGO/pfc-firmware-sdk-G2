// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DynamicIPClientFactory.hpp"
#include "IDynamicIPClientAdministrator.hpp"


#include <string>


namespace netconf {

class DynamicIPClientAdministrator : public IDynamicIPClientAdministrator {
 public:
  explicit DynamicIPClientAdministrator(::std::string vendorclass);
  ~DynamicIPClientAdministrator() override = default;

  DynamicIPClientAdministrator(const DynamicIPClientAdministrator &other) = delete;
  DynamicIPClientAdministrator(DynamicIPClientAdministrator &&other) = delete;
  DynamicIPClientAdministrator& operator=(const DynamicIPClientAdministrator &other) = delete;
  DynamicIPClientAdministrator& operator=(DynamicIPClientAdministrator &&other) = delete;

  IDynamicIPClientPtr AddClient(DynamicIPType type, const Interface &interface, const ::std::string &hostname) override;
  void DeleteClient(const Interface &interface) override;
  IDynamicIPClientPtr GetClient(const Interface &interface) const override;

  void RestartAllClients(const ::std::string& hostname) override;
  Status SetClientIDAndRestartAllClients(::std::string client_id) override;
 private:
  ::std::string vendorclass_;
  ::std::string client_id_;

  DynamicIPClientFactory factory_;
  IDynamicIPClients clients_;

  void OnHostnameChanged();
};

}
