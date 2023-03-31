// SPDX-License-Identifier: LGPL-3.0-or-later
#include "StringUtils.hpp"

#include <algorithm>
#include <cctype>

namespace netconf {

void RemoveCharFromString(::std::string &str, char c) {
  str.erase(std::remove(str.begin(), str.end(), c), str.end());
}

void StringToUpper(::std::string &str) {
  ::std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
    return std::toupper(c);
  }
  );
}
}
