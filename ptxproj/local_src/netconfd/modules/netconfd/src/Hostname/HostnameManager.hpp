#pragma once

#include <string>
#include <map>
#include <tuple>

#include "HostConfFile.hpp"
#include "IHostnameManager.hpp"
#include "IHostnameWillChange.hpp"
#include "HostnameController.hpp"
#include "MacAddress.hpp"
#include "PrioritizedHostAndDomainname.hpp"

namespace netconf {

class HostnameManager : public IHostnameManager, public IHostnameWillChange {
 public:
  explicit HostnameManager(const MacAddress& mac_address);
  ~HostnameManager() override = default;

  HostnameManager(const HostnameManager &other) = delete;
  HostnameManager(HostnameManager &&other)      = delete;
  HostnameManager &operator=(const HostnameManager &other) = delete;
  HostnameManager &operator=(HostnameManager &&other) = delete;

  ::std::string GetHostname() override;

  void OnReloadHostConf() override;
  void OnLeaseFileChange(const Interface &interface) override;
  void OnInterfaceIPChange() override;
  void OnLeaseFileRemove(const Interface &interface) override;

  void RegisterIPManager(IIPManager& ip_manager) override;

 private:
  IIPManager* ip_manager_ = nullptr;

  ::std::string default_hostname_;
  ::std::string hostname_;
  ::std::string domain_;

  PrioritizedHostAndDomainname prioritized_dhcp_host_domainname_;

  IPConfigs GetCurrentIPConfigs();
  void SetPrioritizedDHCPHostAndDomainname();

};

} /* namespace netconf */
