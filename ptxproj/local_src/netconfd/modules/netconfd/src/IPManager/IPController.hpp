// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IIPController.hpp"

namespace netconf {

class IPController : public IIPController {
 public:
  IPController() = default;
  ~IPController() override = default;

  IPController(const IPController&) = delete;
  IPController& operator=(const IPController&) = delete;
  IPController(IPController&&) = delete;
  IPController& operator=(IPController&&) = delete;

  Status Configure(const ::std::string &interface, const Address &address, const Netmask &netmask) const override;
  Status Configure(const IPConfig &config) const override;
  void Flush(const ::std::string &interface) const override;

 private:
  Status SetIPConfig(const ::std::string &interface, const Address &address, const Netmask &netmask) const;

};

} /* namespace netconf */
