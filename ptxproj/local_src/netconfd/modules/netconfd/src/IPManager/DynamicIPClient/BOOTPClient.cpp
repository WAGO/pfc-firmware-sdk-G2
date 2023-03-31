// SPDX-License-Identifier: GPL-2.0-or-later

#include "BOOTPClient.hpp"
#include "Logger.hpp"
#include "CollectionUtils.hpp"

#include <glib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <boost/filesystem.hpp>

#include <csignal>
#include <cstdlib>
#include <thread>

namespace netconf {

namespace {

void RemoveFile(::std::string path) {
  const boost::filesystem::path file = path;
  if (boost::filesystem::exists(file)) {
    boost::filesystem::remove(file);
  }
}

}

BOOTPClient::BOOTPClient(Interface interface)
    : interface_ { ::std::move(interface) },
      pid_{}{

  pid_file_path_ = "/var/run/udhcpc_" + interface_.GetName() + ".pid";

  Start();
}

BOOTPClient::~BOOTPClient() {
  Stop();
}

DynamicIPType BOOTPClient::GetType() {
  return DynamicIPType::BOOTP;
}

void BOOTPClient::UpdateContentFromLease() {
  lease_file_.Parse(LeaseFile::GetLeaseFilePath(interface_));
}

Address BOOTPClient::GetAddressFromLease() {
  return lease_file_.GetAddress();
}
Netmask BOOTPClient::GetNetmaskFromLease() {
  return lease_file_.GetNetmask();
}
::std::string BOOTPClient::GetHostnameFromLease() {
  return lease_file_.GetDHCPHostname();
}
::std::string BOOTPClient::GetDomainFromLease() {
  return lease_file_.GetDHCPDomain();
}

void BOOTPClient::Start() {

  LOG_DEBUG("Run Bootp client for interface " << interface_.GetName());

  ::std::vector<const char *> options{BOOTP_SCRIPT_PATH.c_str(),
                                      "start",
                                      interface_.GetName().c_str(),
                                      nullptr};

  GError *g_error = nullptr;
  auto spawned = g_spawn_async(nullptr, const_cast<gchar**>(options.data()), nullptr, G_SPAWN_DEFAULT, nullptr,  // NOLINT(cppcoreguidelines-pro-type-const-cast)
                               nullptr, &pid_, &g_error);

  if (spawned != TRUE) {
    LogError("Start Bootp Client for interface " + interface_.GetName() + " failed!");
  }
}

void BOOTPClient::Stop() {

  if (0 == kill(pid_, SIGKILL)) {
    LOG_DEBUG("Stopped DHCP Client for interface " << interface_.GetName());
  }

  RemoveFile(LeaseFile::GetLeaseFilePath(interface_));
  RemoveFile(pid_file_path_);

  pid_ = 0;
}

void BOOTPClient::Release() {
  Stop();
}

void BOOTPClient::Renew() {
  Start();
}

void BOOTPClient::RestartWithHostname([[maybe_unused]] ::std::string hostname){}
  
void BOOTPClient::RestartWithClientID([[maybe_unused]] ::std::string clientID){}
  

} /* namespace netconf */
