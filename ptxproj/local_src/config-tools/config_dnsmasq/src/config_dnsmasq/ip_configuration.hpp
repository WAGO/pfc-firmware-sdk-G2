//------------------------------------------------------------------------------
/// Copyright (c) 2020-2024 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Class representation of a port configuration
///
///  \author   MSc : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef SRC_CONFIG_DNSMASQ_IPCONFIGURATION_HPP_
#define SRC_CONFIG_DNSMASQ_IPCONFIGURATION_HPP_

#include <list>

#include "port_data.hpp"

namespace configdnsmasq {

class ip_configuration {
 public:
  std::string host_name;
  std::list<std::string> port_name_list;
  std::list<port_data> port_data_list;

  ip_configuration();
  ip_configuration(const ip_configuration& other) = default;
  ip_configuration& operator=(const ip_configuration& other) = default;
  ip_configuration(ip_configuration&& other) = delete;
  ip_configuration& operator=(ip_configuration&& other) = delete;
  virtual ~ip_configuration() = default;

  bool portIsAvailable(const std::string &port);
  port_data getPortData(const std::string& port);
};

} // namespace configdnsmasq

#endif /* SRC_CONFIG_DNSMASQ_IPCONFIGURATION_HPP_ */
