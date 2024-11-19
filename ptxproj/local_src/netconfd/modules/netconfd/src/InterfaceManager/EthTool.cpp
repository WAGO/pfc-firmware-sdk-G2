// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EthTool.hpp"

#include <cerrno>
#include <climits>
#include <cstdint>
#include <tuple>
#include <cstdlib>
#include <cstring>
#include "Logger.hpp"
#include "NetworkInterfaceConstants.hpp"
#include "Status.hpp"

#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace netconf {


gsl::span<const uint32_t> EthToolSettings::GetSupported() const{
    if(e_.s_.link_mode_masks_nwords == 0){
      return gsl::span<const uint32_t>(&e_.link_mode_masks_[0], e_.maxwords());
    }
    return gsl::span<const uint32_t>(&e_.link_mode_masks_[0], static_cast<size_t>(e_.s_.link_mode_masks_nwords));
}

gsl::span<const uint32_t> EthToolSettings::GetAutonegAdvertising() const {
    return gsl::span<const uint32_t>(&gsl::at(e_.link_mode_masks_, e_.s_.link_mode_masks_nwords),
                                         static_cast<size_t>(e_.s_.link_mode_masks_nwords));
}

void EthToolSettings::SetAutonegAdvertising(gsl::span<const uint32_t> autoneg_advertising) {
        ::std::memcpy(
            &gsl::at(e_.link_mode_masks_, e_.s_.link_mode_masks_nwords),
            autoneg_advertising.data(),
            ::std::min(static_cast<size_t>(e_.s_.link_mode_masks_nwords), autoneg_advertising.size())
                * sizeof(e_.link_mode_masks_[0]));
}

eth::TpMdiType EthToolSettings::GetMdixState() const {
    return static_cast<eth::TpMdiType>(e_.s_.eth_tp_mdix);
}

void EthToolSettings::SetMdixState(eth::TpMdiType mdix_state) {
    e_.s_.eth_tp_mdix_ctrl = static_cast<decltype(e_.s_.eth_tp_mdix_ctrl)>(mdix_state);
}

bool EthToolSettings::IsAutonegEnabled() const {
    return e_.s_.autoneg == AUTONEG_ENABLE;
}

void EthToolSettings::SetAutonegEnabled(bool autoneg_enabled) {
    e_.s_.autoneg = autoneg_enabled ? AUTONEG_ENABLE : AUTONEG_DISABLE;
}

eth::Duplex EthToolSettings::GetDuplex() const {
    return static_cast<eth::Duplex>(e_.s_.duplex);
}

void EthToolSettings::SetDuplex(eth::Duplex duplex) {
    e_.s_.duplex = static_cast<decltype(e_.s_.duplex)>(duplex);
}

::std::uint32_t EthToolSettings::GetSpeed() const {
    return e_.s_.speed;
}

void EthToolSettings::SetSpeed(uint32_t speed) {
    e_.s_.speed = speed;
}

eth::InterfaceLinkState EthToolSettings::GetIfLinkState() const {
  return if_link_state_;
}

eth::DeviceState EthToolSettings::GetIfState() const {
  return if_state_;
}

void EthToolSettings::SetIfState(eth::DeviceState if_state) {
  if_state_ = if_state;
}

EthToolLinkSettings& EthToolSettings::GetLinkSettings() {
  return e_;
}

eth::MediaType EthToolSettings::GetMediaType() const {
    return static_cast<eth::MediaType>(e_.s_.port);
}

EthTool::EthTool(Socket s, const std::string &ifname) : socket_{std::move(s)}, ifreq_{}, ifname_{ifname} {
  ::std::memset(&ifreq_, 0, sizeof(ifreq_));
  strncpy(ifreq_.ifr_name, ifname.c_str(), IFNAMSIZ);  // NOLINT

}

void EthTool::DetermineLinkModeMasksNwords(){
  if(link_mode_masks_nwords_ != 0){
    return;
  }

  struct {
    ethtool_link_settings s;
    uint32_t link_mode_masks[SCHAR_MAX * 3];
  }ls;

  ifreq_.ifr_data = &ls;  //NOLINT(cppcoreguidelines-pro-type-union-access) must access union
  ::std::memset(&ls, 0, sizeof(ls));
  ls.s.cmd = ETHTOOL_GLINKSETTINGS;
  auto res = ioctl(socket_.fd(), SIOCETHTOOL, &ifreq_);
  if (res == 0) {
    link_mode_masks_nwords_ = static_cast<size_t>(abs(ls.s.link_mode_masks_nwords));
  }
}

EthToolSettings EthTool::Create() {
  return EthToolSettings { EthToolLinkSettings { } } ;
}

Status EthTool::IffFlagsGetIoctl() {
  auto res = ioctl(socket_.fd(), SIOCGIFFLAGS, &ifreq_);
  if (res < 0) {
    return Status{StatusCode::GENERIC_ERROR, ::std::string(__func__)  + ": ioctl failed for: " + ifname_};
  }
  return Status{};
}

Status EthTool::IffFlagsSetIoctl() {
  auto res = ioctl(socket_.fd(), SIOCSIFFLAGS, &ifreq_);
  if (res < 0) {
    return Status{StatusCode::GENERIC_ERROR, ::std::string(__func__)  + ": ioctl failed for: " + ifname_};
  }
  return Status{};
}

Status EthTool::EthToolIoctl() {
  auto res = ioctl(socket_.fd(), SIOCETHTOOL, &ifreq_);
  if (res < 0) {
    return Status{StatusCode::GENERIC_ERROR, ::std::string(__func__)  + ": ioctl failed for: " + ifname_};
  }
  return Status{};
}

Status EthTool::Update(EthToolSettings &ets) {
  Status s = IffFlagsGetIoctl();
  if (s.IsNotOk()) {
    return s;
  }
  ets.if_state_ = static_cast<eth::DeviceState>((ifreq_.ifr_flags & IFF_UP) == IFF_UP);  // NOLINT(cppcoreguidelines-pro-type-union-access) must access union
  if (ets.if_state_ == eth::DeviceState::Down) {
    ets.if_link_state_ = eth::InterfaceLinkState::Down;
  }

  ethtool_value eth_value{};
  eth_value.cmd   = ETHTOOL_GLINK;
  ifreq_.ifr_data = reinterpret_cast<__caddr_t>(&eth_value);  // NOLINT: need reinterpret_cast for legacy API handling.
  s               = EthToolIoctl();
  if (s.IsNotOk()) {
    return s;
  }
  ets.if_link_state_ = static_cast<eth::InterfaceLinkState>(eth_value.data);

  DetermineLinkModeMasksNwords();
  ets.e_.s_.link_mode_masks_nwords = static_cast<__s8>(link_mode_masks_nwords_);
  ets.e_.s_.cmd                    = ETHTOOL_GLINKSETTINGS;
  ifreq_.ifr_data                  = &ets.e_.s_;  // NOLINT(cppcoreguidelines-pro-type-union-access) must access union

  s = EthToolIoctl();
  if (s.IsNotOk()) {
    return s;
  }

  return s;
}

bool EthTool::SetRequestHasChanges(const EthToolSettings &getdata, const EthToolSettings &setdata) {
    auto gd= getdata.e_;
    auto sd = setdata.e_;
    if (sd.s_.link_mode_masks_nwords < 0 || gd.s_.link_mode_masks_nwords < 0) {
      return false;
    }

    auto nwords = static_cast<::std::uint8_t>(sd.s_.link_mode_masks_nwords);
    return std::tie(gd.s_.speed, gd.s_.duplex, gd.s_.autoneg)
        != std::tie(sd.s_.speed, sd.s_.duplex, sd.s_.autoneg)
        || (gd.s_.eth_tp_mdix != sd.s_.eth_tp_mdix_ctrl)
        || (memcmp(&gsl::at(gd.link_mode_masks_, nwords),
                   &gsl::at(sd.link_mode_masks_, nwords),
                   nwords * 3 * sizeof(uint32_t)) != 0);
}

Status EthTool::Commit(EthToolSettings &ets) {

  Status s = IffFlagsGetIoctl();
  if(s.IsNotOk()){
    return s;
  }
  ifreq_.ifr_flags = static_cast<int16_t>((ifreq_.ifr_flags & ~IFF_UP)  //NOLINT(cppcoreguidelines-pro-type-union-access) must access union
    | ((ets.if_state_ == eth::DeviceState::Up) ? IFF_UP : 0));

  s = IffFlagsSetIoctl();
  if(s.IsNotOk()){
    return s;
  }

  if (ets.if_state_ == eth::DeviceState::Up) {
    EthToolSettings current = Create();
    s = Update(current);
    if(s.IsNotOk()){
      return s;
    }

    DetermineLinkModeMasksNwords();
    ets.e_.s_.link_mode_masks_nwords = static_cast<__s8 >(link_mode_masks_nwords_);
    ::std::memcpy(ets.e_.link_mode_masks_, current.GetLinkSettings().link_mode_masks_, link_mode_masks_nwords_ * 3 * sizeof(uint32_t));
    ets.e_.s_.phy_address = current.GetLinkSettings().s_.phy_address;
    ets.e_.s_.eth_tp_mdix_ctrl = current.GetLinkSettings().s_.eth_tp_mdix_ctrl;
    ets.e_.s_.eth_tp_mdix = current.GetLinkSettings().s_.eth_tp_mdix;
    ets.e_.s_.cmd = ETHTOOL_SLINKSETTINGS;
    ifreq_.ifr_data = &ets.e_.s_;  //NOLINT(cppcoreguidelines-pro-type-union-access) must access union

    if (SetRequestHasChanges(current, ets)) {
      s = EthToolIoctl();
    }
  }
  return s;
}

} /* namespace netconf */
