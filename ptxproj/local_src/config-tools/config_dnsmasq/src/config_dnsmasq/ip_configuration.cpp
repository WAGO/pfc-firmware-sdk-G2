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

#include "ip_configuration.hpp"

#include <boost/format.hpp>

#include "error_handling.hpp"

namespace configdnsmasq {

ip_configuration::ip_configuration()
    :
    host_name { "" } {

}

bool ip_configuration::portIsAvailable(const std::string &port) {
  auto p = std::find(port_name_list.begin(), port_name_list.end(), port);
  return p != port_name_list.end();
}

port_data ip_configuration::getPortData(const std::string &port) {
  auto data = std::find_if(port_data_list.begin(), port_data_list.end(), [&port](auto d) {
    return d.port_name_ == port;
  });

  erh_assert(data != port_data_list.end(), INVALID_PARAMETER,
             boost::format("There is no ip configuration for port %s") % port);

  return *data;
}

} // namespace configdnsmasq
