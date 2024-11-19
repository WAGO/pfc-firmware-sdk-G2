// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <linux/if.h>
#include <mutex>
#include <gsl/gsl>

#include "Socket.hpp"
#include "NetworkInterfaceConstants.hpp"
#include "IEthernetInterface.hpp"
#include "EthTool.hpp"

namespace netconf {

class EthernetInterface : public IEthernetInterface {
 public:
  explicit EthernetInterface(::std::string name);
  ~EthernetInterface() override = default;
  EthernetInterface(const EthernetInterface& other) = delete;
  EthernetInterface& operator=(const EthernetInterface& other) = delete;
  EthernetInterface(EthernetInterface&& other) = delete;
  EthernetInterface& operator=(EthernetInterface&& other) = delete;

  Status UpdateConfig() override;
  MacAddress GetMac() const override;
  bool GetAutonegSupport() const override;
  bool GetAutonegEnabled() const override;
  eth::MediaType GetMediaType() const override;
  eth::DeviceState GetState() const override;
  eth::InterfaceLinkState GetLinkState() const override;
  ::std::uint32_t GetInterfaceIndex() const override;
  ::std::uint32_t GetMTU() const override;
  gsl::span<const uint32_t> GetSupportedLinkModes() const override;

  ::std::uint32_t GetSpeed() const override;
  eth::Duplex GetDuplex() const override;

  Status Commit() override;

  void SetAutoneg(eth::Autoneg autoneg) override;
  void SetState(eth::DeviceState state) override;
  void SetSpeed(::std::uint32_t speed) override;
  void SetDuplex(eth::Duplex duplex) override;

  static ::std::string IndexToName(::std::uint32_t ifindex);

 private:
  Status SetIfFlags(int16_t if_flags);
  Status GetIfFlags(int16_t& flags) const;
  Status UpdateMac();
  Status UpdateMtu();

  ::std::array<uint8_t, IFHWADDRLEN> mac_;

  ::std::string name_;
  ::std::uint32_t if_index_;

  ::ifreq ifreq_;
  Socket socket_;
  ::std::uint32_t mtu_;

  EthTool ethtool_;
  EthToolSettings ethtool_settings_r_;
  EthToolSettings ethtool_settings_w_;

};

}
