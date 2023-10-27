// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <string>
#include <string_view>
#include <utility>
#include <map>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include "MacAddress.hpp"
#include "DeviceType.hpp"

namespace netconf {

using InterfacePropertyValue = ::boost::variant<::std::string, int>;
using InterfacePropertyMap = ::std::map<InterfaceProperty, InterfacePropertyValue>;

class Interface {
 public:
  Interface() = default;
  ~Interface() = default;

  Interface(const Interface& interface) = default;
  Interface& operator=(const Interface& interface) = default;
  Interface(Interface&& interface) = default;
  Interface& operator=(Interface&& interface) = default;

  bool operator==(const Interface& other) const;
  bool operator!=(const Interface& other) const;
  bool operator>(const Interface& other) const;
  bool operator<(const Interface& other) const;


  DeviceType GetType() const;
  ::std::string GetName() const;
  ::std::string GetLabel() const;
  InterfacePropertyMap GetProperties() const;

  static Interface CreateEthernet(const ::std::string &name);
  static Interface CreatePort(const ::std::string& name);
  static Interface CreateBridge(const ::std::string& name);
  static Interface CreateWwan(const ::std::string &name);
  static Interface CreateDummy(const ::std::string& name);
  static Interface CreateVLAN(const ::std::string& name, int vlanid, const ::std::string& link);
  static Interface CreateLoopback(const ::std::string &name);
  static Interface CreateOther(const ::std::string &name);

  static ::std::string NameFromLabel(const ::std::string &label);
  static ::std::string LabelFromName(const ::std::string &name);

 private:
  Interface(::std::string name, DeviceType type, InterfacePropertyMap properties);
  Interface(::std::string name, DeviceType type);

  ::std::string name_{};
  DeviceType type_{DeviceType::Other};
  InterfacePropertyMap properties_;
};

struct Address : public ::std::string {
  Address() = default;
  explicit Address(const ::std::string &str)
      : ::std::string(str) {
  }
  using ::std::string::string;
};

struct Netmask : public ::std::string {
  Netmask() = default;
  explicit Netmask(const ::std::string &str)
      : ::std::string(str) {
  }
  using ::std::string::string;
};

// TODO(SB): modernize with constexpr type when new cpp-std is available.
// Use static string to prevent Lint warning [cppcoreguidelines-pro-bounds-array-to-pointer-decay]
static const Address ZeroIP = "0.0.0.0"; //NOLINT
static const Netmask ZeroNetmask = "0.0.0.0"; //NOLINT


enum class IPSource {
  UNKNOWN,
  NONE,
  STATIC,
  DHCP,
  BOOTP,
  EXTERNAL,
  FIXIP
};

/**
 * @brief POCO class to hold the ip-config for one interface
 * @note The fields must be filled depending on the @see IPSource.
 *  The @see IPConfig::interface_ field must be set always.
 *  For the sources @see IPSource::NONE, @see IPSource::DHCP, @see IPSource::BOOTP and @see IPSource::EXTERNAL
 *  setting set the @see IPConfig::source_ field is sufficient.
 *  For the sources @see IPSource::STATIC and @see IPConfig::address_ fields needs to be
 * set also.
 *
 */
struct IPConfig {
  IPConfig()
      : source_ { IPSource::NONE },
        address_ { ZeroIP },
        netmask_ { ZeroNetmask } {
  }

  explicit IPConfig(::std::string interface)
      : interface_ { ::std::move(interface) },
        source_ { IPSource::NONE },
        address_ { ZeroIP },
        netmask_ { ZeroIP } {
  }

  IPConfig(::std::string interface, const IPSource source)
      : interface_ { ::std::move(interface) },
        source_ { source },
        address_ { ZeroIP },
        netmask_ { ZeroNetmask } {
  }

  IPConfig(::std::string interface, const IPSource source, ::std::string dhcp_client_id)
      : interface_ { ::std::move(interface) },
        source_ { source },
        address_ { ZeroIP },
        netmask_ { ZeroNetmask },
        dhcp_client_id_{dhcp_client_id} {
  }

  IPConfig(::std::string interface, const IPSource source, Address address, Netmask netmask)
      : interface_ { ::std::move(interface) },
        source_ { source },
        address_ { ::std::move(address) },
        netmask_ { ::std::move(netmask) } {
  }

  static IPConfig CreateDefault(::std::string interface) {
    return {::std::move(interface), IPSource::STATIC, ZeroIP, ZeroNetmask};
  }

  void Clear() {
    source_ = IPSource::NONE;
    interface_.clear();
    address_ = ZeroIP;
    netmask_ = ZeroNetmask;
    dhcp_client_id_.clear();
  }

  bool operator==(const IPConfig &config) const {
    return ((this->interface_ == config.interface_) && (this->source_ == config.source_)
        && (this->address_ == config.address_) && (this->netmask_ == config.netmask_) && (this->dhcp_client_id_ == config.dhcp_client_id_));
  }

  bool operator <(const IPConfig &config) const {
    return (interface_ < config.interface_);
  }

  template<typename ... Args>
  static constexpr bool SourceChangesToAnyOf(const IPConfig &old, const IPConfig &new_, Args ... sources) {
    bool changed = false;
    if (old.source_ != new_.source_) {
      if (sizeof...(sources) > 0) {
        changed = SourceIsAnyOf(new_, sources...);
      } else {
        changed = true;
      }
    }
    return changed;
  }

  template<typename ... Args>
  static constexpr bool SourceIsAnyOf(const IPConfig &config, Args ... sources) {
    bool match = false;
    (void) std::initializer_list<int> { (match |= (config.source_ == (sources)),0)... };
    return match;
  }

  ::std::string interface_;
  IPSource source_;
  Address address_;
  Netmask netmask_;
  ::std::string dhcp_client_id_;

};

enum class DipSwitchMode {
  OFF,
  STATIC,
  DHCP,
  HW_NOT_AVAILABLE
};

struct DipSwitchIpConfig {

  DipSwitchIpConfig() = default;

  DipSwitchIpConfig(Address address, Netmask netmask)
      : address_(::std::move(address)),
        netmask_(::std::move(netmask)) {
  }

  bool IsIncomplete() const {
    return (address_.empty() || netmask_.empty());
  }

  void Clear() {
    netmask_.clear();
    address_.clear();
  }

  bool operator==(const DipSwitchIpConfig &config) const {
    return ((this->address_ == config.address_) && (this->netmask_ == config.netmask_));
  }

  Address address_;
  Netmask netmask_;
};

struct DipSwitchConfig {
  DipSwitchConfig()
      : DipSwitchConfig(DipSwitchIpConfig { }) {
  }

  explicit DipSwitchConfig(const DipSwitchIpConfig &ip_config)
      : DipSwitchConfig(ip_config, DipSwitchMode::OFF, 0) {
  }

  DipSwitchConfig(DipSwitchIpConfig ip_config, DipSwitchMode mode, int value)
      : ip_config_(::std::move(ip_config)),
        mode_(mode),
        value_(value) {
  }

  DipSwitchIpConfig ip_config_;
  DipSwitchMode mode_;
  int value_;
};

enum class InterfaceState {
  UNKNOWN,
  DOWN,
  UP,
};

enum class LinkState {
  UNKNOWN,
  DOWN,
  UP,
};

enum class Duplex {
  UNKNOWN = 0xFF,
  HALF = 0,
  FULL = 1
};

enum class Autonegotiation {
  UNKNOWN,
  ON,
  OFF
};

enum class AutonegotiationSupported {
  UNKNOWN,
  NO,
  YES
};

enum class MacLearning {
  UNKNOWN,
  ON,
  OFF,
};

struct InterfaceBase {
  static constexpr int UNKNOWN_SPEED = -1;

  explicit InterfaceBase(
      Interface interface,
      InterfaceState state = InterfaceState::UNKNOWN,
      Autonegotiation autoneg = Autonegotiation::UNKNOWN,
      int speed = UNKNOWN_SPEED,
      Duplex duplex = Duplex::UNKNOWN,
      MacLearning mac_learning = MacLearning::UNKNOWN) noexcept

      : interface_ { std::move(interface) },
        state_ { state },
        autoneg_ { autoneg },
        speed_ { speed },
        duplex_ { duplex },
        mac_learning_{ mac_learning}{
  }

  InterfaceBase() noexcept
      : state_ { InterfaceState::UNKNOWN },
        autoneg_ { Autonegotiation::UNKNOWN },
        speed_ { UNKNOWN_SPEED },
        duplex_ { Duplex::UNKNOWN },
        mac_learning_ { MacLearning::UNKNOWN }{
  }

  bool operator==(const InterfaceBase &config) const {
    return ((this->autoneg_ == config.autoneg_)
        && (this->interface_ == config.interface_)
        && (this->duplex_ == config.duplex_)
        && (this->speed_ == config.speed_)
        && (this->state_ == config.state_)
        && (this->mac_learning_ == config.mac_learning_));
  }

  void FillUpDefaults() {
    state_ = (state_ == InterfaceState::UNKNOWN) ? InterfaceState::UP : state_;
    autoneg_ = (autoneg_ == Autonegotiation::UNKNOWN) ? Autonegotiation::ON : autoneg_;
    speed_ = (speed_ == UNKNOWN_SPEED) ? 100 : speed_;
    duplex_ = (duplex_ == Duplex::UNKNOWN) ? Duplex::FULL : duplex_;
    mac_learning_ = (mac_learning_ == MacLearning::UNKNOWN) ? MacLearning::ON : mac_learning_;
  }

  static InterfaceBase DefaultConfig(Interface device_name) {
    return InterfaceBase{ ::std::move(device_name), InterfaceState::UP, Autonegotiation::ON, 100, Duplex::FULL, MacLearning::ON };
  }

  Interface interface_;
  InterfaceState state_;
  Autonegotiation autoneg_;
  int speed_;
  Duplex duplex_;
  MacLearning mac_learning_;

};

using InterfaceConfig = InterfaceBase;

struct InterfaceStatus : InterfaceBase {

  explicit InterfaceStatus(
      Interface interface,
      InterfaceState state = InterfaceState::UNKNOWN,
      Autonegotiation autoneg = Autonegotiation::UNKNOWN,
      int speed = UNKNOWN_SPEED,
      Duplex duplex = Duplex::UNKNOWN,
      LinkState link_state = LinkState::UNKNOWN,
      MacAddress mac = MacAddress { }) noexcept

      : InterfaceBase { ::std::move(interface), state, autoneg, speed, duplex },
        link_state_ { link_state },
        mac_ { mac } {
  }

  explicit InterfaceStatus(InterfaceBase base) noexcept
      : InterfaceBase { ::std::move(base) },
        link_state_ { LinkState::UNKNOWN }{
  }

  InterfaceStatus() noexcept
      : InterfaceBase { Interface(), InterfaceState::UNKNOWN, Autonegotiation::UNKNOWN, UNKNOWN_SPEED, Duplex::UNKNOWN },
        link_state_ { LinkState::UNKNOWN }{
  }

  bool operator==(const InterfaceStatus &config) const {
    return ((this->autoneg_ == config.autoneg_)
        && (this->interface_ == config.interface_)
        && (this->duplex_ == config.duplex_)
        && (this->speed_ == config.speed_)
        && (this->state_ == config.state_)
        && (this->link_state_ == config.link_state_)
        && (this->mac_ == config.mac_));
  }

  LinkState link_state_;
  MacAddress mac_;
};

}  // netconf
