// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EthernetInterface.hpp"

#include <ifaddrs.h>
#include <linux/ethtool.h>
#include <linux/if_arp.h>
#include <linux/rtnetlink.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <gsl/gsl>
#include <string>
#include <utility>

#include "Status.hpp"

extern "C" char* if_indextoname(unsigned int __ifindex, char* __ifname);
extern "C" unsigned int if_nametoindex(const char* __ifname);

namespace netconf {

using eth::Autoneg;
using eth::DeviceState;
using eth::InterfaceLinkState;
using eth::MediaType;
using gsl::span;
using namespace std::literals;

EthernetInterface::EthernetInterface(::std::string name)

    : mac_{0},
      name_(std::move(name)),
      ifreq_{},
      socket_{AF_INET, SOCK_DGRAM, IPPROTO_IP},
      mtu_{},
      ethtool_{socket_, name_},
      ethtool_settings_r_{ethtool_.Create()},
      ethtool_settings_w_{ethtool_.Create()} {
  strncpy(ifreq_.ifr_name, name_.c_str(),
          name_.size());  // NOLINT: must access union members in legacy data structures.

  UpdateConfig();
}

Status EthernetInterface::UpdateMac() {
  if (ioctl(socket_.fd(), SIOCGIFHWADDR, &ifreq_) < 0) {
    return Status{StatusCode::GENERIC_ERROR, ::std::string(__func__) + ": ioctl() SIOCGIFHWADDR failed for " + name_};
  }
  mac_[0] = static_cast<uint8_t>(
      ifreq_.ifr_hwaddr.sa_data[0]);  // NOLINT: must access union members in legacy data structures.
  mac_[1] = static_cast<uint8_t>(
      ifreq_.ifr_hwaddr.sa_data[1]);  // NOLINT: must access union members in legacy data structures.
  mac_[2] = static_cast<uint8_t>(
      ifreq_.ifr_hwaddr.sa_data[2]);  // NOLINT: must access union members in legacy data structures.
  mac_[3] = static_cast<uint8_t>(
      ifreq_.ifr_hwaddr.sa_data[3]);  // NOLINT: must access union members in legacy data structures.
  mac_[4] = static_cast<uint8_t>(
      ifreq_.ifr_hwaddr.sa_data[4]);  // NOLINT: must access union members in legacy data structures.
  mac_[5] = static_cast<uint8_t>(
      ifreq_.ifr_hwaddr.sa_data[5]);  // NOLINT: must access union members in legacy data structures.

  return Status{};
}

Status EthernetInterface::UpdateMtu() {
  if (ioctl(socket_.fd(), SIOCGIFMTU, &ifreq_) < 0) {
    return Status{StatusCode::GENERIC_ERROR, ::std::string(__func__) + ": ioctl() SIOCGIFMTU failed for " + name_};
  }
  mtu_ = static_cast<std::uint32_t>(ifreq_.ifr_mtu);  // NOLINT: must access union members in legacy data structures.
  return Status{};
}

Status EthernetInterface::UpdateConfig() {
  if_index_ = if_nametoindex(name_.c_str());
  if (if_index_ == 0) {
    return Status{StatusCode::GENERIC_ERROR, ::std::string(__func__) + ": failed to get interface index for " + name_};
  }
  Status s = UpdateMtu();
  if (s.IsNotOk()) {
    return s;
  }
  s = UpdateMac();
  if (s.IsNotOk()) {
    return s;
  }
  s = ethtool_.Update(ethtool_settings_r_);
  if (s.IsNotOk()) {
    return s;
  }
  ethtool_settings_w_ = ethtool_settings_r_;
  return s;
}

::std::uint32_t EthernetInterface::GetSpeed() const {
  return ethtool_settings_r_.GetSpeed();
}

eth::Duplex EthernetInterface::GetDuplex() const {
  return ethtool_settings_r_.GetDuplex();
}

MacAddress EthernetInterface::GetMac() const {
  return MacAddress(mac_);
}

bool EthernetInterface::GetAutonegSupport() const {
  return TestLinkModesBits(ethtool_settings_r_.GetSupported(), ETHTOOL_LINK_MODE_Autoneg_BIT);
}

bool EthernetInterface::GetAutonegEnabled() const {
  return ethtool_settings_r_.IsAutonegEnabled();
}

MediaType EthernetInterface::GetMediaType() const {
  return ethtool_settings_r_.GetMediaType();
}

gsl::span<const uint32_t> EthernetInterface::GetSupportedLinkModes() const {
  return ethtool_settings_r_.GetSupported();
}

[[gnu::pure]] ::std::uint32_t EthernetInterface::GetMTU() const {
  return mtu_;
}

Status EthernetInterface::SetIfFlags(int16_t if_flags) {
  ifreq_.ifr_flags = if_flags;  // NOLINT: must access union members in legacy data structures.
  if (ioctl(socket_.fd(), SIOCSIFFLAGS, &ifreq_) < 0) {
    return Status{StatusCode::GENERIC_ERROR, ::std::string(__func__) + ": ioctl() SIOCSIFFLAGS failed for " + name_};
  }
  return Status{};
}

Status EthernetInterface::GetIfFlags(int16_t& flags) const {
  if (ioctl(socket_.fd(), SIOCGIFFLAGS, &ifreq_) < 0) {
    return Status{StatusCode::GENERIC_ERROR, ::std::string(__func__) + ": ioctl() SIOCGIFFLAGS failed for " + name_};
  }
  flags = ifreq_.ifr_flags;  // NOLINT: must access union members in legacy data structures.
  return Status{};
}

DeviceState EthernetInterface::GetState() const {
  return ethtool_settings_r_.GetIfState();
}

InterfaceLinkState EthernetInterface::GetLinkState() const {
  return ethtool_settings_r_.GetIfLinkState();
}

Status EthernetInterface::Commit() {
  return ethtool_.Commit(ethtool_settings_w_);
}

void EthernetInterface::SetAutoneg(eth::Autoneg autoneg) {
  ethtool_settings_w_.SetAutonegEnabled(autoneg == eth::Autoneg::On);
}

void EthernetInterface::SetSpeed(::std::uint32_t speed) {
  if (speed > 0) {
    ethtool_settings_w_.SetSpeed(speed);
  }
}

void EthernetInterface::SetDuplex(eth::Duplex duplex) {
  ethtool_settings_w_.SetDuplex(duplex);
}

void EthernetInterface::SetState(DeviceState state) {
  ethtool_settings_w_.SetIfState(state);
}

[[gnu::pure]] ::std::uint32_t EthernetInterface::GetInterfaceIndex() const {
  return if_index_;
}

::std::string EthernetInterface::IndexToName(::std::uint32_t ifindex) {
  ::std::array<char, IFNAMSIZ + 1> nameBuf{};
  if_indextoname(ifindex, nameBuf.data());
  return ::std::string{nameBuf.data()};
}

}  // namespace netconf
