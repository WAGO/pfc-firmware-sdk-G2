// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * EthernetInterfaceFactory.cpp
 *
 *  Created on: 23.10.2019
 *      Author: u014487
 */

#include "EthernetInterfaceFactory.hpp"

#include "EthernetInterface.hpp"

namespace netconf {

::std::unique_ptr<IEthernetInterface> EthernetInterfaceFactory::getEthernetInterface(Interface interface) {
  return ::std::make_unique<EthernetInterface>(interface.GetName());
}

} /* namespace netconf */
