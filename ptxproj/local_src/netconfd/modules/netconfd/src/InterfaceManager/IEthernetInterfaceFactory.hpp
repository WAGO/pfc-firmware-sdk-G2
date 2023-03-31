// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IEthernetInterfaceFactory.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#pragma once
#include <memory>
#include "IEthernetInterface.hpp"
#include "BaseTypes.hpp"

namespace netconf {

class IEthernetInterfaceFactory {
 public:
  IEthernetInterfaceFactory() = default;
  virtual ~IEthernetInterfaceFactory() = default;

  IEthernetInterfaceFactory(const IEthernetInterfaceFactory &other) = delete;
  IEthernetInterfaceFactory& operator=(const IEthernetInterfaceFactory &other) = delete;
  IEthernetInterfaceFactory(IEthernetInterfaceFactory &&other) = delete;
  IEthernetInterfaceFactory& operator=(IEthernetInterfaceFactory &&other) = delete;

  virtual ::std::unique_ptr<IEthernetInterface> getEthernetInterface(Interface interface) = 0;
};

}  // namespace netconf

//---- End of header file ------------------------------------------------------

