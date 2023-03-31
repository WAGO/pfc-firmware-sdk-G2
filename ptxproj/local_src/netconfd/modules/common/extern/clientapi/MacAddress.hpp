// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>
#include <cstdint>
#include <gsl/gsl>
#include <type_traits>

namespace netconf {

struct MacAddress {
  static constexpr ::std::size_t LENGTH = 6;
  ::std::uint8_t addr_[LENGTH] = {0};

  MacAddress() = default;
  explicit constexpr MacAddress(const ::std::uint8_t(& array)[LENGTH]) noexcept: addr_{}{
    for (auto i = 0; i != LENGTH; ++i){
      ::gsl::at(addr_,i) = ::gsl::at(array,i);
    }
  }

  explicit MacAddress(::gsl::span<const ::std::uint8_t, LENGTH> mac) {
    std::copy(::std::begin(mac), ::std::end(mac), ::std::begin(addr_));
  }

  explicit MacAddress(const void* mac){
    ::std::memcpy(addr_, mac, LENGTH);  //NOLINT void* type must be supported because the system api only returns void*
  }

  ::std::string ToString(char delim = ':') const;

  static MacAddress FromString(const ::std::string& macstr, char delim = ':');

  /**
   * Increment the vendor specific part of the MAC address by the given value.
   * The OUI part is not altered.
   *
   * @param inc The increment value.
   * @return Copy of this MAC address incremented by @ref inc
   */
  MacAddress Increment(uint32_t inc) const;

  bool operator ==(const MacAddress& other) const{
    return std::equal(std::begin(addr_), std::end(addr_), std::begin(other.addr_));
  }

  constexpr const uint8_t* data() const{
    return &addr_[0];
  }
};

}  // namespace netconf
//---- End of header file ------------------------------------------------------
