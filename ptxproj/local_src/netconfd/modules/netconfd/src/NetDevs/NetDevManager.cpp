// SPDX-License-Identifier: GPL-2.0-or-later

#include "NetDevManager.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <functional>
#include <memory>
#include <optional>

#include "IInterfaceEvent.hpp"
#include "INetlinkLink.hpp"
#include "LinkHelper.hpp"
#include "Logger.hpp"
#include "NetDev.hpp"
#include "Status.hpp"

namespace netconf {

using std::make_shared;
using std::string;
using namespace std::literals;

namespace {

using NetDevPredicate = ::std::function<bool(const NetDevPtr &)>;

bool Exists(const NetDevs &netdevs, NetDevPredicate predicate) {
  auto it = ::std::find_if(netdevs.begin(), netdevs.end(), ::std::move(predicate));
  return (it != netdevs.end());
}

NetDevPtr FindIf(const NetDevs &netdevs, NetDevPredicate predicate) {
  auto it = ::std::find_if(netdevs.begin(), netdevs.end(), ::std::move(predicate));
  return it != netdevs.end() ? *it : NetDevPtr{nullptr};
}

::std::string ChangesToString(const ::std::string& info_name, const ::std::string& old_value, const ::std::string& new_value){
  if(old_value != new_value){
    return "[" + info_name + ": " + old_value + " -> " + new_value + "] ";
  }
  return "";
}

::std::string ChangesToString(const ::std::string& info_name, int old_value, int new_value){
  if(old_value != new_value){
    return "[" + info_name + ": " + ::std::to_string(old_value) + " -> " + ::std::to_string(new_value) + "] ";
  }
  return "";
}

::std::string ShowDiff(LinkInfo oldinfo, LinkInfo newinfo) {
  ::std::string changes{};

  changes += ChangesToString("index", oldinfo.index_, newinfo.index_);
  changes += ChangesToString("index link", oldinfo.index_link_.value_or(0), newinfo.index_link_.value_or(0));
  changes += ChangesToString("mac", oldinfo.mac_, newinfo.mac_);
  changes += ChangesToString("name", oldinfo.name_, newinfo.name_);
  changes += ChangesToString("name link", oldinfo.name_link_.value_or(""), newinfo.name_link_.value_or(""));
  changes += ChangesToString("parent index", oldinfo.parent_index_.value_or(0), newinfo.parent_index_.value_or(0));
  changes += ChangesToString("child type", oldinfo.child_type_.value_or(""), newinfo.child_type_.value_or(""));
  changes += ChangesToString("vlanid", oldinfo.vlanid_.value_or(0), newinfo.vlanid_.value_or(0));

  changes += ChangesToString("flags", static_cast<int>(oldinfo.flags_), static_cast<int>(newinfo.flags_));
  if (oldinfo.flags_ != newinfo.flags_) {
    changes += "-" + (::std::string(((newinfo.flags_ & IFF_LOWER_UP) > 0) ? "up" : "down")) + " ";
  }

  if(oldinfo == newinfo){
    changes += "[No changes]";
  }
  return changes;
}

}  // namespace

NetDevManager::NetDevManager(::std::shared_ptr<IInterfaceMonitor> interface_monitor, IEventManager &event_manager,
                             INetlinkLink &netlink)
    : interface_monitor_{::std::move(interface_monitor)},
      netdev_construction_{nullptr},
      netlink_{netlink},
      event_manager_{event_manager} {

  netlink_.SetInterfaceUp("eth0");

  Links links = netlink_.GetLinkInfo();
  for (auto &link_info : links) {
    UpdateOrCreateNetdev(link_info);
  }

  interface_monitor_->RegisterEventHandler(*this);
}

Status NetDevManager::NetlinkAddVLAN(const Interface &interface) {
  Status status;

  try {
    auto properties = interface.GetProperties();

    int id = ::boost::get<int>( properties.at(InterfaceProperty::VlanID));
    ::std::string vlan_link = ::boost::get<::std::string>(properties.at(InterfaceProperty::Link));

    auto netdev = GetByName(vlan_link);
    if(netdev){
      status = netlink_.AddVLAN(interface.GetName(), static_cast<::std::uint16_t>(id), netdev->GetIndex());
    }else {
      status.Set(StatusCode::GENERIC_ERROR, "Add VLAN failed. Parent interface " + vlan_link + " does not exist.");
    }

  }catch (const std::out_of_range& ex) {
    return Status(StatusCode::GENERIC_ERROR, ::std::string("Failed to add vlan interface. Missing properties: ") + ex.what());
  }catch (const boost::bad_get& ex) {
    return Status(StatusCode::GENERIC_ERROR, ::std::string("Failed to add vlan interface. Missing properties: ") + ex.what());
  }

  return status;
}

Status NetDevManager::AddInterface(const Interface &interface) {
  Status status;
  LOG_DEBUG("Adding interface " << interface.GetName() << " of type " << ToString(interface.GetType()));

  switch (interface.GetType()) {
    case DeviceType::Dummy:
      status = netlink_.AddDummy(interface.GetName());
      break;
    case DeviceType::Vlan:
      status = NetlinkAddVLAN(interface);
      break;
    case DeviceType::Bridge:
      status = netlink_.AddBridge(interface.GetName());
      break;
    default:
      status.Set(
          StatusCode::GENERIC_ERROR,
          "Adding interfaces (" + interface.GetName() + ") of type " + ToString(interface.GetType())
              + " is not supported." + interface.GetName());
      break;
  }

  if (status.IsOk()) {
    netlink_.SetInterfaceUp(interface.GetName());

    auto link_info = netlink_.GetLinkInfo(interface.GetName());
    if (link_info) {
      LOG_DEBUG("After add:" << link_info->ToString());
      UpdateOrCreateNetdev(link_info.value());
    } else {
      LogError("Expected interface " + interface.GetName() + " to be created but was not.");
    }
  }

  return status;
}

Status NetDevManager::AddInterfaces(const Interfaces &interfaces) {
  Status status;
  for (auto &interface : interfaces) {
    Status s = AddInterface(interface);
    if (s.IsNotOk()) {
      status = s;
      break;
    }
  }
  return status;
}

NetDevPtr NetDevManager::UpdateOrCreateNetdev(LinkInfo &link_info) {
  auto netdev = GetByIfIndex(link_info.index_);
  if (netdev) {
    netdev->SetLinkInfo(link_info);
  } else {
    Interface itf = LinkHelper::LinkToInterface(link_info);
    if(itf.GetType() != DeviceType::Other){
      netdev = make_shared<NetDev>(link_info);
      net_devs_.push_back(netdev);
      LOG_DEBUG("Create netdev " + link_info.name_ + " with index " + ::std::to_string(link_info.index_));
      OnNetDevCreated(netdev);
    }
    else{
      LogInfo("Ignore interface " + link_info.name_ + " of unsupported type " + link_info.type_);
    }
  }

  return netdev;
}

Status NetDevManager::Delete(const Interface &interface) {
  if (not IsDeletable(interface.GetType())) {
    return Status { StatusCode::GENERIC_ERROR, "Deletion of interface (" + interface.GetName() + ") of type "
        + ToString(interface.GetType()) + " is not supported." + interface.GetName() };
  }

  Status status = netlink_.Delete(interface.GetName());
  if (status.IsOk()) {
    Delete(GetByInterface(interface));
  }

  return status;
}

Status NetDevManager::Delete(const Interfaces &interfaces){
  Status status;
  for (auto &interface : interfaces) {
    Status s = Delete(interface);
    if (s.IsNotOk()) {
      status.Append(s.ToString());
    }
  }
  return status;
}

void NetDevManager::Delete(const NetDevPtr& netdev) {
  auto it = ::std::find(net_devs_.begin(), net_devs_.end(), netdev);
  if (it != net_devs_.end()) {
    LOG_DEBUG("Remove netdev " + netdev->GetInterface().GetName() + " with index " +
              ::std::to_string(netdev->GetIndex()));
    OnNetDevRemoved(netdev);
    net_devs_.erase(it);
  }
}

NetDevPtr NetDevManager::GetByInterface(Interface interface) {
  auto pred = [&interface](const auto &net_dev) { return net_dev->GetInterface() == interface; };
  return FindIf(net_devs_, pred);
}

NetDevPtr NetDevManager::GetByName(const ::std::string &name) {
  auto pred = [&name](const auto &net_dev) { return net_dev->GetInterface().GetName() == name; };
  return FindIf(net_devs_, pred);
}

NetDevPtr NetDevManager::GetByIfIndex(::std::int32_t if_index) {
  auto pred = [=](const auto &net_dev) { return net_dev->GetIndex() == if_index; };
  return FindIf(net_devs_, pred);
}

NetDevs NetDevManager::GetNetDevs() {
  NetDevs copy = {net_devs_};
  return copy;
}

NetDevs NetDevManager::GetNetDevs(::std::vector<DeviceType> types){
  NetDevs netdevs;

  ::std::copy_if(net_devs_.begin(), net_devs_.end(), std::back_inserter(netdevs),[&](NetDevPtr netdev){
    return ::std::any_of(types.begin(), types.end(), [&](DeviceType type){return type == netdev->GetDeviceType();});
  });

  return netdevs;
}

NetDevPtr NetDevManager::GetParent(NetDevPtr child) {
  auto index = child->GetParentIndex();
  return GetByIfIndex(index);
}

NetDevs NetDevManager::GetChildren(NetDevPtr parent) {
  NetDevs netdevs;

  ::std::copy_if(net_devs_.begin(), net_devs_.end(), std::back_inserter(netdevs),
                 [&](NetDevPtr netdev) { return netdev->GetParentIndex() == parent->GetIndex(); });

  return netdevs;
}

bool NetDevManager::IsAnyChildUp(NetDevPtr parent) {
  NetDevs children = GetChildren(parent);
  return ::std::any_of(children.begin(), children.end(), [](NetDevPtr child) { return child->IsLinkStateUp(); });
}

Interfaces NetDevManager::GetInterfacesByDeviceType(::std::vector<DeviceType> types) {
  Interfaces interfaces;
  for (auto &netdev : net_devs_) {
    if (::std::any_of(types.begin(), types.end(), [&](DeviceType type){return type == netdev->GetDeviceType();})) {
      interfaces.emplace_back(netdev->GetInterface());
    }
  }
  return interfaces;
}


Interfaces NetDevManager::GetInterfaces(){
  Interfaces interfaces;
  for (auto &netdev : net_devs_) {
    interfaces.emplace_back(netdev->GetInterface());
  }
  return interfaces;
}

Interfaces NetDevManager::GetBridgesWithAssignetPort() {
  Interfaces interfaces;

  NetDevs ports = GetNetDevs({DeviceType::Port});
  for (auto &port : ports) {
    NetDevPtr bridge = GetParent(port);
    if(bridge){
      if (std::find(interfaces.begin(), interfaces.end(), bridge->GetInterface()) == interfaces.end()) {
        interfaces.emplace_back(bridge->GetInterface());
      }
    }
  }

  return interfaces;
}

void NetDevManager::SetUp(const Interface &interface) {
  netlink_.SetInterfaceUp(interface.GetName());
}

void NetDevManager::SetDown(const Interface &interface) {
  netlink_.SetInterfaceDown(interface.GetName());
}

Interfaces NetDevManager::GetPorts(const Interface &interface) {
  Interfaces child_interfaces;
  auto parent_netdev = GetByInterface(interface);
  if(parent_netdev){
    auto parent_index  = parent_netdev->GetIndex();
    for (auto &netdev : net_devs_) {
      if (netdev->GetParentIndex() == parent_index) {
        child_interfaces.emplace_back(netdev->GetInterface());
      }
    }
  }

  return child_interfaces;
}

void NetDevManager::BridgePortLeave(const Interface &port) {
  auto port_netdev = GetByInterface(port);

  if (port_netdev) {
    auto bridge_netdev = GetByIfIndex(port_netdev->GetParentIndex());
    if (bridge_netdev) {
      netlink_.DeleteParent(port_netdev->GetIndex());
      port_netdev->SetLinkInfo(netlink_.GetLinkInfo(port_netdev->GetIndex()).value());

      OnBridgePortsChange(bridge_netdev);
    }
  }
}

Status NetDevManager::BridgePortJoin(const Interface &port, const Interface &bridge) {
  Status status;
  auto bridge_netdev = GetByInterface(bridge);
  auto port_netdev  = GetByInterface(port);
  if (bridge_netdev && port_netdev) {
    netlink_.SetParent(port_netdev->GetIndex(), bridge_netdev->GetIndex());
    auto link_info = netlink_.GetLinkInfo(port_netdev->GetIndex());
    if (link_info) {
      LOG_DEBUG("LinkInfo after set parent: " << link_info.value().ToString());
      UpdateOrCreateNetdev(link_info.value());
    }
    OnBridgePortsChange(bridge_netdev);
  } else {
    status = Status(StatusCode::GENERIC_ERROR, "set parent interface of " + port.GetName() +
                                                   " failed. Parent NetDev " + bridge.GetName() +
                                                   " does not exist.");
  }
  return status;
}

void NetDevManager::SetMac(NetDevPtr netdev, const ::std::string &mac) {
  netlink_.SetMac(netdev->GetName(), mac);
  auto link_info = netlink_.GetLinkInfo(netdev->GetIndex());
  if(link_info.has_value()){
    netdev->SetLinkInfo(link_info.value());
  }
}

bool NetDevManager::ExistsByInterface(Interface interface, const NetDevs &netdevs) {
  auto names_are_equal = [&](const auto &net_dev) { return net_dev->GetInterface() == interface; };
  return Exists(netdevs, names_are_equal);
}

bool NetDevManager::DoesNotExistByInterface(Interface interface, const NetDevs &netdevs) {
  return !ExistsByInterface(::std::move(interface), netdevs);
}

void NetDevManager::LinkChange(LinkInfo new_link, InterfaceEventAction action) {

  LinkInfo old_link{};
  NetDevPtr netdev = GetByIfIndex(new_link.index_);
  if(netdev){
    old_link = netdev->GetLinkInfo();
  }
  LOG_DEBUG(new_link.name_ << " action: " << ToString(action) << " " << ShowDiff(old_link, new_link));

  if (netdev) {
    auto old_linkstate = netdev->GetLinkState();
    netdev->SetLinkInfo(new_link);
    bool lower_layer_link_change = old_linkstate != netdev->GetLinkState();
    if (netdev_construction_ != nullptr && lower_layer_link_change) {
      netdev_construction_->OnLinkChange(netdev);
    }
  }

  if (action == InterfaceEventAction::NEW || action == InterfaceEventAction::CHANGE) {
    netdev = UpdateOrCreateNetdev(new_link);
  }

  if (action == InterfaceEventAction::DEL) {
    Delete(netdev);
  }

  if (netdev && (action == InterfaceEventAction::NEW || action == InterfaceEventAction::DEL)) {
    event_manager_.NotifyNetworkChanges(EventLayer::EVENT_FOLDER, netdev->GetInterface());
  }
}

void NetDevManager::RegisterForNetDevConstructionEvents(INetDevEvents &netdev_construction) {
  netdev_construction_ = &netdev_construction;
}

void NetDevManager::OnNetDevCreated(NetDevPtr netdev) const {
  if (netdev_construction_ != nullptr) {
    netdev_construction_->OnNetDevCreated(::std::move(netdev));
  }
}

void NetDevManager::OnNetDevRemoved(NetDevPtr netdev) const {
  if (netdev_construction_ != nullptr) {
    netdev_construction_->OnNetDevRemoved(::std::move(netdev));
  }
}

void NetDevManager::OnBridgePortsChange(NetDevPtr netdev) const {
  if (netdev_construction_ != nullptr) {
    netdev_construction_->OnBridgePortsChange(::std::move(netdev));
  }
}

}  // namespace netconf
