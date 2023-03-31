// SPDX-License-Identifier: LGPL-3.0-or-later
#include "DeviceType.hpp"
#include <sstream>
#include <tuple>
#include <array>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace netconf {

using KindConversion = ::std::tuple<const char*, DeviceType>;

constexpr ::std::array<KindConversion,8> conversions {{ //NOLINT(cert-err58-cpp)
    KindConversion{"bridge", DeviceType::Bridge},
    KindConversion{"wwan", DeviceType::Wwan},
    KindConversion{"port", DeviceType::Port},
    KindConversion{"loopback", DeviceType::Loopback},
    KindConversion{"ethernet", DeviceType::Ethernet},
    KindConversion{"other", DeviceType::Other},
    KindConversion{"vlan", DeviceType::Vlan},
    KindConversion{"dummy", DeviceType::Dummy}
}};


DeviceTypes DeviceTypeFromString(const ::std::string &str) {

  DeviceTypes types;

  ::std::vector<::std::string> parts;
  boost::algorithm::split(parts, str, boost::is_any_of("|"), boost::algorithm::token_compress_on );

  for(const auto& p : parts){
    auto it = std::find_if(std::begin(conversions),std::end(conversions), [&](const auto& conv){return std::get<0>(conv) == p;});
    if(it != std::end(conversions)){
      types.emplace_back(std::get<1>(*it));
    }
  }

  return types;
}

bool DeviceTypeIsValid(const ::std::string& str)
{
  return !DeviceTypeFromString(str).empty();
}

::std::string ToString(DeviceType device_type) {
  for(const auto& conv: conversions){
    const auto& type = ::std::get<1>(conv);
    const auto& text = ::std::get<0>(conv);
    if(device_type == type){
      return  text;
    }
  }
  return ::std::string();
}

}  // namespace netconf
