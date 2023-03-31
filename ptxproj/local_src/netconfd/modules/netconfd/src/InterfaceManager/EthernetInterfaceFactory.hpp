// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IEthernetInterfaceFactory.hpp"

namespace netconf {

class EthernetInterfaceFactory : public IEthernetInterfaceFactory {
 public:
  EthernetInterfaceFactory() = default;
  ~EthernetInterfaceFactory() override = default;

  EthernetInterfaceFactory(const EthernetInterfaceFactory &other) = delete;
  EthernetInterfaceFactory& operator=(const EthernetInterfaceFactory &other) = delete;
  EthernetInterfaceFactory(EthernetInterfaceFactory &&other) = delete;
  EthernetInterfaceFactory& operator=(EthernetInterfaceFactory &&other) = delete;

  ::std::unique_ptr<IEthernetInterface> getEthernetInterface(Interface interface) override;

};

} /* namespace netconf */

