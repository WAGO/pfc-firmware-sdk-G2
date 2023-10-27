// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DynamicIPClientFactory.hpp"
#include "IDynamicIPClientAdministrator.hpp"


#include <optional>
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

  IDynamicIPClientPtr AddClient(DynamicIPType type, const Interface &interface, const ::std::string &hostname, const ::std::string& client_id) override;
  void DeleteClient(const Interface &interface) override;
  IDynamicIPClientPtr GetClient(const Interface &interface) const override;

  void RestartAllClients(const ::std::string& hostname) override;

 private:
  ::std::string vendorclass_;

  DynamicIPClientFactory factory_;
  IDynamicIPClients clients_;

  void OnHostnameChanged();
};

}
