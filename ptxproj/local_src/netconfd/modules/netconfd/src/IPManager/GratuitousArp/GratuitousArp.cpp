// SPDX-License-Identifier: GPL-2.0-or-later

#include "GratuitousArp.hpp"

#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>

#include <cassert>
#include <cerrno>
#include <fstream>
#include <gsl/gsl>

#include "Logger.hpp"
#include "Socket.hpp"
#include "Status.hpp"

namespace netconf {

#define MAC_LEN 6
#define ETHER_TYPE_LEN 2
#define ETH_HDRLEN (MAC_LEN + MAC_LEN + ETHER_TYPE_LEN)

static uint32_t ARP_DELAY_US = (750*1000);

struct __attribute__((packed)) ArpHeader {
  uint16_t htype;
  uint16_t ptype;
  uint8_t hlen;
  uint8_t plen;
  uint16_t opcode;
  uint8_t sender_mac[6];
  uint32_t sender_ip;
  uint8_t target_mac[6];
  uint32_t target_ip;
};

static ArpHeader getArpHeader(const MacAddress src_mac, uint32_t src_ip) {
  ArpHeader header = {};
  header.htype     = htons(1);  // Hardware type (16 bits): 1 for ethernet
  header.ptype     = htons(ETH_P_IP);
  header.hlen      = MAC_LEN;
  header.plen      = 4;         // 4 bytes for IPv4 address
  header.opcode    = htons(2);  // OpCode: 2 for Gratuitous ARP
  memcpy(&header.sender_mac, src_mac.data(), MAC_LEN);
  header.sender_ip = src_ip;
  memset(&header.target_mac, 0xff, MAC_LEN);
  header.target_ip = src_ip;

  return header;
}

static void prepareArpFrame(::gsl::span<uint8_t> frame, const MacAddress src_mac, ArpHeader& arp_header) {
  memset(frame.data(), 0xff, MAC_LEN);  // Set destination MAC address: broadcast address
  memcpy(&frame[6], src_mac.data(), MAC_LEN);
  frame[12] = ETH_P_ARP / 256;
  frame[13] = ETH_P_ARP % 256;
  memcpy(&frame[14], &arp_header, sizeof(arp_header));
}

static sockaddr_ll getDevice(MacAddress src_mac, int itf_index) {
  sockaddr_ll device = {};
  device.sll_family         = AF_PACKET;
  device.sll_ifindex        = itf_index;
  memcpy(&device.sll_addr[0], src_mac.data(), MAC_LEN);
  device.sll_halen = static_cast<uint8_t>(htons(MAC_LEN));

  return device;
}

Status GratuitousArp::Send(const Address& ip_address, const NetDevPtr& bridge_netdev,
                           const NetDevPtr& port_netdev) const {
  uint32_t src_ip;
  if (1 != inet_pton(AF_INET, ip_address.c_str(), &src_ip)) {
    return Status {StatusCode::IPV4_FORMAT, ip_address};
  }

  uint8_t buffer[ETH_HDRLEN + sizeof(ArpHeader)] = {};
  ::gsl::span<uint8_t> frame(buffer);

  ArpHeader arp_hdr = getArpHeader(bridge_netdev->GetMac(), src_ip);
  prepareArpFrame(frame, bridge_netdev->GetMac(), arp_hdr);
  sockaddr_ll device = getDevice(port_netdev->GetMac(), port_netdev->GetIndex());
  auto device_ptr = reinterpret_cast<sockaddr*>(&device);  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

  Socket s(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  int ret = 0;
  if ((ret = sendto(s.fd(), frame.data(), frame.size(), 0, device_ptr, sizeof(device))) == -1) {
    return Status {StatusCode::SYSTEM_CALL, ::std::string{strerror(errno)}};
  }

  return {};
}

void GratuitousArp::SendGratuitousArpOnBridge(NetDevPtr bridge_netdev, Address address) const {
  LogDebug("Sending gratis arp on " + bridge_netdev->GetName());
  if (bridge_netdev->GetDeviceType() == DeviceType::Bridge && address != ::std::string(ZeroIP)) {

    usleep(ARP_DELAY_US);  // immediatly after change of link state it seams not possible to transmit data -> delay sending of arps
    auto s = Send(address, bridge_netdev, bridge_netdev);

    if (!s.IsOk()) {
      LogWarning("Failed to send gratis arp on " + bridge_netdev->GetName());
    }
  } else {
    LogDebug("arp not sent on bridge " + bridge_netdev->GetName());
  }
}

void GratuitousArp::SendGratuitousArpOnPort(NetDevPtr port_netdev, NetDevPtr bridge_netdev, Address address) const {
  ::std::string ze{ZeroIP};
  LogDebug("Sending gratis arp on " + port_netdev->GetName());
  if (port_netdev->GetDeviceType() == DeviceType::Port && address != ze) {
    if (bridge_netdev) {
      usleep(ARP_DELAY_US);  // immediatly after change of link state it seams not possible to transmit data -> delay sending of arps
      auto s = Send(address, bridge_netdev, port_netdev);

      if (!s.IsOk()) {
        LogWarning(s.ToString());
      }

      LogDebug("Sent gratis arp on " + port_netdev->GetName());
    } else {
      LogDebug("(no bridge dev) Not sent gratis arp on " + port_netdev->GetName());
    }
  } else {
    LogDebug("arp not send on port " + port_netdev->GetName());
  }
}

void GratuitousArp::EnableGratuitousArp(IPLinkPtr ip_link) const {
  //(arp_notify = 1) generates gratuitous arp requests when device is brought up or hardware address changes.
  // It does not if ip address changes!

  auto interface = ip_link->GetInterface();

  auto buffer_len = std::snprintf(nullptr, 0, "/proc/sys/net/ipv4/conf/%s/arp_notify", interface.GetName().c_str());

  assert(buffer_len > 0);  // NOLINT: do not implicitly decay an array into a pointer

  auto len_with_zero = static_cast<std::size_t>(buffer_len + 1);

  auto file_path = std::unique_ptr<char[]>{new char[len_with_zero]};

  std::snprintf(file_path.get(), len_with_zero, "/proc/sys/net/ipv4/conf/%s/arp_notify", interface.GetName().c_str());

  auto arp_notify = std::ofstream{file_path.get()};

  if (arp_notify.good()) {
    arp_notify << '1';
    arp_notify.flush();
    arp_notify.close();
  }

  if (not arp_notify.good()) {
    LogError(Status{StatusCode::GENERIC_ERROR, "Failed to enable gratuitous arp"}.ToString());
  }
}
}  // namespace netconf
