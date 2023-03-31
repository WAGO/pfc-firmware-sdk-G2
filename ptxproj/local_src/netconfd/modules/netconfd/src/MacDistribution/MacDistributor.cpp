// SPDX-License-Identifier: GPL-2.0-or-later

#include "MacDistributor.hpp"

#include <boost/algorithm/string.hpp>
#include <gsl/gsl>

#include "Logger.hpp"
#include "alphanum.hpp"

namespace netconf {

namespace {

int DetermineBridgeMacIncrement(Interface &interface) {
  auto last_index = interface.GetName().find_last_not_of("0123456789");
  if (last_index != ::std::string::npos) {
    try {
      return std::stoi(interface.GetName().substr(last_index + 1));
    } catch (...) {
      /* NOP */
    }
  }
  return -1;
}

}  // namespace

MacDistributor::MacDistributor(MacAddress mac_address, uint32_t mac_inc,
                               INetDevManager &netdev_manager)
    : base_mac_address_{mac_address},
      mac_inc_{mac_inc},
      netdev_manager_{netdev_manager},
      mac_counter_{0} {
}

void MacDistributor::AssignMacs(NetDevs &net_devs) {
  auto sort_alphanum = [](const NetDevPtr &lhs, const NetDevPtr &rhs) {
    return doj::alphanum_comp(lhs->GetInterface().GetName(), rhs->GetInterface().GetName()) < 0;
  };
  ::std::sort(net_devs.begin(), net_devs.end(), sort_alphanum);

  mac_counter_ = 0;

  auto port_count = static_cast<uint32_t>(::std::count_if(net_devs.begin(), net_devs.end(), [&](NetDevPtr netdev) {
    return DeviceTypeIsAnyOf(netdev->GetDeviceType(), DeviceType::Port);
  }));

  if (IsMacAddressAssignmentFull(mac_inc_, port_count)) {
    AssignFullMacSupport(net_devs);
  } else if (IsMacAddressAssignmentMultiple(mac_inc_, port_count)) {
    AssignMultipleMacSupport(net_devs);
  } else {
    AssignSingleMacSupport(net_devs);
  }
}

void MacDistributor::AssignMac(NetDevPtr &net_dev, MacAddress mac) {
  netdev_manager_.SetMac(net_dev, mac.ToString());
}

void MacDistributor::AssignMac(NetDevPtr &net_dev) {
  auto mac = base_mac_address_.Increment(mac_counter_);
  AssignMac(net_dev, mac);
}

void MacDistributor::AssignMacAndIncrement(NetDevPtr &net_dev) {
  auto mac = base_mac_address_.Increment(mac_counter_);
  AssignMac(net_dev, mac);
  mac_counter_++;
}

void MacDistributor::AssignSingleMacSupport(NetDevs &net_devs) {
  auto assign_macs = [&](NetDevPtr &net_dev) {
    if (net_dev->GetDeviceType() == DeviceType::Bridge || net_dev->GetDeviceType() == DeviceType::Port) {
      AssignMac(net_dev, base_mac_address_);
    }
  };

  ::std::for_each(net_devs.begin(), net_devs.end(), assign_macs);
}

void MacDistributor::AssignMultipleMacSupport(NetDevs &net_devs) {
  auto assign_mac_to_bridge = [&](NetDevPtr &net_dev) {
    if (net_dev->GetDeviceType() == DeviceType::Bridge) {
      AssignMac(net_dev, base_mac_address_);
    }
  };

  ::std::for_each(net_devs.begin(), net_devs.end(), assign_mac_to_bridge);
  mac_counter_++;

  auto assign_mac_to_ports = [&](NetDevPtr &net_dev) {
    if (net_dev->GetDeviceType() == DeviceType::Port) {
      AssignMacAndIncrement(net_dev);
    }
  };

  ::std::for_each(net_devs.begin(), net_devs.end(), assign_mac_to_ports);
}

void MacDistributor::AssignFullMacSupport(NetDevs &net_devs) {
  ::std::list<uint32_t> mac_incs(mac_inc_);
  std::generate(mac_incs.begin(), mac_incs.end(), [n = 0]() mutable { return n++; });

  auto assign_mac_to_bridge = [&](NetDevPtr &net_dev) {
    if (net_dev->GetDeviceType() == DeviceType::Bridge) {
      LOG_DEBUG("assign_mac_to_bridge: " << net_dev->GetName());
      auto name    = net_dev->GetInterface();
      auto inc_num = DetermineBridgeMacIncrement(name);
      if ((inc_num >= 0) && (static_cast<uint32_t>(inc_num) < mac_inc_)) {
        auto it = ::std::find(mac_incs.begin(), mac_incs.end(), inc_num);
        if(it == mac_incs.end()){
          LogError("Mac for interface index " + ::std::to_string(inc_num) + " has already been assigned. " + net_dev->GetName() + " was not assigned a MAC.");
          return;
        }
        mac_incs.erase(it);
        AssignMac(net_dev, base_mac_address_.Increment(static_cast<uint32_t>(inc_num)));
      } else {
        LogWarning("MAC increment couldn't be determined, taking base: " + net_dev->GetInterface().GetName());
        AssignMac(net_dev, base_mac_address_);
      }
    }
  };

  ::std::for_each(net_devs.begin(), net_devs.end(), assign_mac_to_bridge);

  auto assign_from_mac_incs = [&](NetDevPtr &net_dev) {
    LOG_DEBUG("assign_from_mac_incs: " << net_dev->GetName());
    if (!mac_incs.empty()) {
      auto mac = base_mac_address_.Increment(mac_incs.front());
      mac_incs.pop_front();
      AssignMac(net_dev, mac);
    } else {
      LogWarning("No MAC increment left, taking base MAC: " + net_dev->GetInterface().GetName());
      AssignMac(net_dev, base_mac_address_);
    }
  };

  auto assign_mac_to_ports = [&](NetDevPtr &net_dev) {
    if (net_dev->GetDeviceType() == DeviceType::Port) {
      LOG_DEBUG("assign_mac_to_ports: " << net_dev->GetName());
      auto bridge = netdev_manager_.GetParent(net_dev);
      if (BridgeHasMoreThenOneAssignedPort(bridge)) {
        LOG_DEBUG("assign_mac_to_ports: " << net_dev->GetName() << " has more then one assigned port");
        assign_from_mac_incs(net_dev);
      }
      if (BridgeHasOneAssignedPort(bridge)) {
        LOG_DEBUG("assign_mac_to_ports: " << net_dev->GetName() << " has one assigned port");
        AssignMac(net_dev, bridge->GetMac());
      }
      if (not bridge) {
        LOG_DEBUG("assign_mac_to_ports: " << net_dev->GetName() << " has no bridge");
        assign_from_mac_incs(net_dev);
      }
    }
  };

  ::std::for_each(net_devs.begin(), net_devs.end(), assign_mac_to_ports);
}

bool MacDistributor::IsMacAddressAssignmentMultiple(uint32_t mac_count, uint32_t port_count) const {
  if (not IsMacAddressAssignmentFull(mac_count, port_count) && not(mac_count == 1)) {
    if (mac_count >= port_count) {
      return true;
    }
  }
  return false;
}

bool MacDistributor::IsMacAddressAssignmentFull(uint32_t mac_count, uint32_t port_count) const {
  auto max_bridges_with_exclusiv_mac = port_count / 2;
  auto required_macs                 = port_count + max_bridges_with_exclusiv_mac;

  return mac_count >= required_macs;
}

bool MacDistributor::BridgeHasMoreThenOneAssignedPort(NetDevPtr& net_dev) const {
  if (net_dev) {
    auto ports = netdev_manager_.GetChildren(net_dev);
    return (ports.size() > 1);
  }
  return false;
}

bool MacDistributor::BridgeHasOneAssignedPort(NetDevPtr& net_dev) const {
  if (net_dev) {
    auto ports = netdev_manager_.GetChildren(net_dev);
    return (ports.size() == 1);
  }
  return false;
}

}  // namespace netconf
