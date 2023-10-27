// SPDX-License-Identifier: GPL-2.0-or-later

#include "DynamicIPClientAdministrator.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "DHCPClient.hpp"
#include "Logger.hpp"
#include "Status.hpp"

namespace netconf {

namespace {

void DeleteAllLeaseFiles() {
  try {
    ::std::vector<::std::string> files;
    for (const auto &file : ::std::filesystem::directory_iterator("/tmp")) {
      files.emplace_back(file.path());
    }

    for (const auto &file : files) {
      if (file.rfind("/tmp/dhcp-bootp-data-", 0) == 0) {
        ::std::remove(file.c_str());
      }
    }
  } catch (const std::exception &e) {
    LogError("Error during initialization. An error occurred while deleting the existing lease files: " +
             ::std::string{e.what()});
  }
}

}  // namespace

DynamicIPClientAdministrator::DynamicIPClientAdministrator(::std::string vendorclass)
    : vendorclass_{::std::move(vendorclass)} {
  DeleteAllLeaseFiles();
}

IDynamicIPClientPtr DynamicIPClientAdministrator::AddClient(DynamicIPType type, const Interface &interface,
                                                            const ::std::string &hostname, const ::std::string& client_id) {
  auto client = factory_.Create(type, interface, hostname, vendorclass_, client_id);
  clients_.emplace(interface, client);
  return client;
}

void DynamicIPClientAdministrator::DeleteClient(const Interface &interface) {
  clients_.erase(interface);
}

IDynamicIPClientPtr DynamicIPClientAdministrator::GetClient(const Interface &interface) const {
  auto it = clients_.find(interface);
  if (it != clients_.end()) {
    return (*it).second;
  }
  return nullptr;
}

void DynamicIPClientAdministrator::RestartAllClients(const ::std::string &hostname) {
  for (auto &client : clients_) {
    client.second->RestartWithHostname(hostname);
  }
}

} /* namespace netconf */
