// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "NetworkInterfaceConstants.hpp"
#include "MacAddress.hpp"
#include <gsl/gsl>

#include "Status.hpp"

namespace netconf {

class IEthernetInterface {
 public:
  IEthernetInterface() = default;
  virtual ~IEthernetInterface() = default;

  IEthernetInterface(const IEthernetInterface &other) = delete;
  IEthernetInterface& operator=(const IEthernetInterface &other) = delete;
  IEthernetInterface(IEthernetInterface &&other) = delete;
  IEthernetInterface& operator=(IEthernetInterface &&other) = delete;

  virtual Status UpdateConfig() = 0;
  virtual MacAddress GetMac() const = 0;
  virtual bool GetAutonegSupport() const = 0;
  virtual bool GetAutonegEnabled() const = 0;
  virtual eth::MediaType GetMediaType() const = 0;
  virtual ::std::uint32_t GetSpeed() const = 0;
  virtual eth::Duplex GetDuplex() const = 0;
  virtual eth::DeviceState GetState() const = 0;
  virtual eth::InterfaceLinkState GetLinkState() const = 0;
  virtual ::std::uint32_t GetInterfaceIndex() const = 0;
  virtual ::std::uint32_t GetMTU() const = 0;
  virtual gsl::span<const uint32_t> GetSupportedLinkModes() const = 0;
  virtual Status Commit() = 0;
  virtual void SetAutoneg(eth::Autoneg autoneg) = 0;
  virtual void SetState(eth::DeviceState state) = 0;
  virtual void SetSpeed(::std::uint32_t speed) = 0;
  virtual void SetDuplex(eth::Duplex duplex) = 0;

};


}  // namespace netconf


//---- End of header file ------------------------------------------------------
