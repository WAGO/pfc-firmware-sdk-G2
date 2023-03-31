// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "BaseTypes.hpp"
#include "FileEditor.hpp"
#include "LeaseFileParser.hpp"
#include "ILeaseFile.hpp"

namespace netconf {

class LeaseFile : public ILeaseFile {
 public:
  LeaseFile() noexcept = default ;
  ~LeaseFile() override = default;

  LeaseFile(const LeaseFile &other) = default;
  LeaseFile(LeaseFile &&other) = delete;
  LeaseFile& operator=(const LeaseFile &other) = delete;
  LeaseFile& operator=(LeaseFile &&other) = delete;

  void Parse(const ::std::string& lease_file_path) override;

  bool Exists();
  Address GetAddress() override;
  Netmask GetNetmask() override;
  ::std::string GetDHCPHostname() override;
  ::std::string GetDHCPDomain() override;

  static ::std::string GetLeaseFilePath(const Interface& interface){
    static ::std::string base_path = "/tmp/dhcp-bootp-data-";
    return base_path + interface.GetName();
  }

 private:
  bool lease_file_exists_ = false;
  LeaseFileParser parser_;
};

}
