// SPDX-License-Identifier: GPL-2.0-or-later

#include "DynamicIPClientAdministrator.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "DHCPClient.hpp"
#include "Logger.hpp"
#include "Status.hpp"

namespace netconf {

namespace {

Status ValidateDHCPClientID(const ::std::string& client_id){

  //the udhcpc supports a maximum clientID length of 249 bytes.
  Status s;
  if(client_id.length() > 249){
    s.Set(StatusCode::GENERIC_ERROR, "The maximum clientID length of 249 bytes was exceeded");
  }
  return s;

}

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

IDynamicIPClientPtr DynamicIPClientAdministrator::AddClient(DynamicIPType type, const ::std::string &itf_name,
                                                            const ::std::string &hostname) {
  auto client = factory_.Create(type, itf_name, hostname, vendorclass_, client_id_);
  clients_.emplace(itf_name, client);
  return client;
}

void DynamicIPClientAdministrator::DeleteClient(const ::std::string &itf_name) {
  clients_.erase(itf_name);
}

IDynamicIPClientPtr DynamicIPClientAdministrator::GetClient(const ::std::string &itf_name) const {
  auto it = clients_.find(itf_name);
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

Status DynamicIPClientAdministrator::SetClientIDAndRestartAllClients(::std::string client_id) {

  Status s = ValidateDHCPClientID(client_id);
  if (s.IsOk()) {
    client_id_ = client_id;
    for (auto &client : clients_) {
      client.second->RestartWithClientID(client_id_);
    }
  }
  return s;
}

} /* namespace netconf */
