// SPDX-License-Identifier: GPL-2.0-or-later

#include "InterfaceConfigManager.hpp"
#include <algorithm>
#include <boost/format.hpp>

#include "DeviceType.hpp"
#include "IInterfaceInformation.hpp"
#include "Logger.hpp"
#include "LinkModeConversion.hpp"
#include "InterfaceConfigurationValidator.hpp"
#include "BridgeUtil.hpp"

namespace netconf {

using namespace std::literals;

using ::std::find_if;
using ::std::inserter;
using ::std::back_inserter;
using ::std::transform;

InterfaceConfigManager::InterfaceConfigManager(INetDevManager &netdev_manager,
                                               IPersistence<InterfaceConfigs> &persistence_provider,
                                               IEthernetInterfaceFactory &eth_factory)
    : netdev_manager_ { netdev_manager },
      persistence_provider_ { persistence_provider },
      ethernet_interface_factory_ { eth_factory } {

  InterfaceConfigs peristet_configs;
  auto read_persistence_data_status = persistence_provider.Read(peristet_configs);
  if (read_persistence_data_status.IsNotOk()) {
    LogWarning("InterfaceConfigManager: Failed to read persistence data, use default data set");
  }

  auto netdevs = netdev_manager.GetNetDevs({DeviceType::Port});

  InitializeEthernetInterfaceMap(netdevs);

  InitializeCurrentConfigs(netdevs, peristet_configs);

  persistence_provider_.Write(current_config_);
}

void InterfaceConfigManager::InitializePorts(InterfaceState initalPortState) {
  auto inital_config = current_config_;
  if (initalPortState != InterfaceState::UNKNOWN) {
    for (auto &portcfg : inital_config) {
      portcfg.state_ = initalPortState;
    }
  }

  ApplyPortConfigs(inital_config);
}

Status InterfaceConfigManager::Configure(const InterfaceConfigs &port_configs) {
  auto status = InterfaceConfigurationValidator::Validate(port_configs, GetInterfaceInformations());
  if (status.IsNotOk()) {
    return status;
  }

  UpdateCurrentInterfaceConfigs(port_configs);

  status = ApplyPortConfigs(current_config_);

  if (status.IsNotOk()) {
    InterfaceConfigs old_port_configs;
    ApplyPortConfigs(old_port_configs);
  } else {
    persistence_provider_.Write(current_config_);
  }

  return status;
}

[[gnu::const]]
const InterfaceConfigs& InterfaceConfigManager::GetPortConfigs() {
  return current_config_;
}

static InterfaceState EthDeviceStateToInterfaceState(eth::DeviceState state) {
  return (state == eth::DeviceState::Up) ? InterfaceState::UP : InterfaceState::DOWN;
}

static LinkState EthLinkStateToLinkState(eth::InterfaceLinkState state) {
  return (state == eth::InterfaceLinkState::Up) ? LinkState::UP : LinkState::DOWN;
}

Status InterfaceConfigManager::GetCurrentPortStatuses(InterfaceStatuses& itf_statuses) {
  for (auto& [itf, eth_itf] : ethernet_interfaces_) {
    InterfaceStatus itf_status{itf};

    Status s = eth_itf->UpdateConfig();
    if(s.IsNotOk()){
      return s;
    }
    auto state = eth_itf->GetState();
    itf_status.state_ = EthDeviceStateToInterfaceState(state);

    itf_status.duplex_ = static_cast<Duplex>(eth_itf->GetDuplex());
    itf_status.speed_ = static_cast<::std::int32_t>(eth_itf->GetSpeed());

    auto link_state = eth_itf->GetLinkState();
    itf_status.link_state_ = EthLinkStateToLinkState(link_state);

    itf_status.mac_ = eth_itf->GetMac();

    itf_status.mac_learning_ = GetMacLearning(eth_itf->GetInterfaceIndex());

    itf_statuses.emplace_back(itf_status);

  }
  return Status{};
}

InterfaceInformation InterfaceConfigManager::GetInterfaceInformation(const NetDev &netdev) const {

  auto device_type = netdev.GetDeviceType();
  if (device_type == DeviceType::Port && ethernet_interfaces_.count(netdev.GetInterface()) != 0) {
    auto &eif = ethernet_interfaces_.at(netdev.GetInterface());
    Status s = eif->UpdateConfig();
    if(s.IsOk()){
      return InterfaceInformation{ netdev.GetInterface(), not IsIpAddressable(device_type),
        eif->GetAutonegSupport() ? AutonegotiationSupported::YES : AutonegotiationSupported::NO, ToLinkModes(
        eif->GetSupportedLinkModes()) };
    }else{
      //If the ethtool link settings are not readable (e.g. WP400) then a default config is returned.
      //This means that at least a default json config is available for the frontend (WBM).
      return InterfaceInformation{ netdev.GetInterface(), not IsIpAddressable(device_type),
        AutonegotiationSupported::YES, {{100, Duplex::FULL}} };
    }

  }
  return InterfaceInformation{ netdev.GetInterface(), not IsIpAddressable(device_type) };

}

void InterfaceConfigManager::InitializeEthernetInterfaceMap(const NetDevs &netdevs) {
  auto create_ethernet_interface = [&](auto netdev) {
    return ::std::make_pair(Interface{netdev->GetInterface()}, ethernet_interface_factory_.getEthernetInterface(netdev->GetInterface()));
  };

  transform(netdevs.begin(), netdevs.end(), inserter(ethernet_interfaces_, ethernet_interfaces_.end()),
            create_ethernet_interface);
}

void InterfaceConfigManager::InitializeCurrentConfigs(const NetDevs &netdevs,
                                                      const InterfaceConfigs &persistet_configs) {
  for (auto &netdev : netdevs) {
    current_config_.push_back(InterfaceConfig::DefaultConfig(Interface{netdev->GetInterface()}));
  }
  UpdateCurrentInterfaceConfigs(persistet_configs);
}

Status InterfaceConfigManager::ApplyPortConfig(InterfaceConfig const &cfg) {

  auto &eif = ethernet_interfaces_.at(cfg.interface_);
  eif->UpdateConfig();
  if (cfg.autoneg_ != Autonegotiation::UNKNOWN) {
    eif->SetAutoneg((cfg.autoneg_ == Autonegotiation::ON) ? eth::Autoneg::On : eth::Autoneg::Off);
  }
  if (cfg.state_ != InterfaceState::UNKNOWN) {
    eif->SetState((cfg.state_ == InterfaceState::UP) ? eth::DeviceState::Up : eth::DeviceState::Down);
  }
  if (cfg.duplex_ != Duplex::UNKNOWN) {
    eif->SetDuplex((cfg.duplex_ == Duplex::FULL) ? eth::Duplex::Full : eth::Duplex::Half);
  }
  if (cfg.speed_ > 0) {
    eif->SetSpeed(static_cast<::std::uint32_t>(cfg.speed_));
  }
  if (cfg.mac_learning_ != MacLearning::UNKNOWN) {
    SetMacLearning(eif->GetInterfaceIndex(), cfg.mac_learning_);
  }

  return eif->Commit();

}

Status InterfaceConfigManager::ApplyPortConfigs(InterfaceConfigs &port_configs) {
  ::std::vector<Status> applyResults;
  std::transform(port_configs.begin(), port_configs.end(), ::std::back_inserter(applyResults),
                 [this](auto &port_config) {
                   return this->ApplyPortConfig(port_config);
                 });

  auto first_negative_status = std::find_if(applyResults.begin(), applyResults.end(), [](auto &status) {
    return status.IsNotOk();
  });

  if (first_negative_status != applyResults.end()) {
    return *first_negative_status;
  }

  return Status { StatusCode::OK };
}

void InterfaceConfigManager::UpdateCurrentInterfaceConfigs(const InterfaceConfigs &port_configs) {

  for (auto &port_config : port_configs) {

    auto device_name_equals = [&](auto &config) {
      return port_config.interface_ == config.interface_;
    };
    auto it = ::std::find_if(current_config_.begin(), current_config_.end(), device_name_equals);
    if (it == current_config_.end()) {
      continue;
    }

    auto apply_if_neq = [](auto &lhs, auto rhs, auto comp) {
      if (rhs != comp) {
        lhs = rhs;
      }
    };

    apply_if_neq(it->state_, port_config.state_, InterfaceState::UNKNOWN);
    apply_if_neq(it->autoneg_, port_config.autoneg_, Autonegotiation::UNKNOWN);
    apply_if_neq(it->duplex_, port_config.duplex_, Duplex::UNKNOWN);
    apply_if_neq(it->mac_learning_, port_config.mac_learning_, MacLearning::UNKNOWN);

    if (port_config.speed_ == 10 || port_config.speed_ == 100 || port_config.speed_ == 1000) {
      it->speed_ = port_config.speed_;
    }
  }
}

InterfaceInformations InterfaceConfigManager::GetInterfaceInformations() {
  auto netdevs = netdev_manager_.GetNetDevs();
  InterfaceInformations iis;
  std::transform(netdevs.begin(), netdevs.end(), ::std::back_inserter(iis), [&](const auto &netdev_ptr) {
    return GetInterfaceInformation(*netdev_ptr);
  });
  return iis;
}

}  // namespace netconf
