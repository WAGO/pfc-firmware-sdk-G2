// SPDX-License-Identifier: GPL-2.0-or-later

#include "IPController.hpp"

#include <arpa/inet.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <gsl/gsl>
#include <system_error>

#include "Logger.hpp"
#include "Socket.hpp"

namespace netconf {

static Status SetIPParameter(int fd, uint16_t flag, const ::std::string &interface, const ::std::string &value_str) {
  uint32_t value;
  if (1 != inet_pton(AF_INET, value_str.c_str(), &value)) {
    return Status { StatusCode::IPV4_FORMAT, value_str };
  }

  ifreq if_req = { };
  ::gsl::span<char> if_req_name { if_req.ifr_name };  // NOLINT: do not access members of unions is unavoidable at this point

  strncpy(if_req_name.data(), interface.c_str(), if_req_name.size());
  if_req.ifr_addr.sa_family = AF_INET;  // NOLINT: do not access members of unions is unavoidable at this point

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
  sockaddr_in *sock_value = reinterpret_cast<sockaddr_in*>(&if_req.ifr_addr);  // NOLINT: do not access members of unions is unavoidable at this point
#pragma GCC diagnostic pop

  sock_value->sin_addr.s_addr = value;
  if (ioctl(fd, flag, &if_req) < 0) {
    return MAKE_SYSTEMCALL_ERROR("ioctl");
  }

  return {};
}

Status IPController::SetIPConfig(const ::std::string &interface, const Address &address, const Netmask &netmask) const {
  LOG_DEBUG(interface + " " + address + " " + netmask);
  Status status(StatusCode::OK);

  try {
    ::Socket socket { PF_INET, SOCK_DGRAM, IPPROTO_IP };

    status = SetIPParameter(socket.fd(), SIOCSIFADDR, interface, address);
    if (status.IsNotOk()) {
      auto user_status = Status { StatusCode::SET_IP, address, netmask, interface };
      LogError(user_status.ToString() + ": " + status.ToString());
      return user_status;
    }

    if (ZeroIP != address) {
      status = SetIPParameter(socket.fd(), SIOCSIFNETMASK, interface, netmask);
      if (status.IsNotOk()) {
        auto user_status = Status { StatusCode::SET_IP, address, netmask, interface };
        LogError(user_status.ToString() + ": " + status.ToString());
        return user_status;
      }
    }

  } catch (std::exception &e) {
    return Status { StatusCode::GENERIC_ERROR, e.what() };
  }

  return status;
}

Status IPController::Configure(const ::std::string &interface, const Address &address, const Netmask &netmask) const {
  return SetIPConfig(interface, address, netmask);
}

Status IPController::Configure(const IPConfig &config) const {
  return SetIPConfig(config.interface_, config.address_, config.netmask_);
}

void IPController::Flush(const ::std::string &interface) const {
  Configure(interface, ZeroIP, ZeroNetmask);
}

}  // namespace netconf
/* namespace netconf */
