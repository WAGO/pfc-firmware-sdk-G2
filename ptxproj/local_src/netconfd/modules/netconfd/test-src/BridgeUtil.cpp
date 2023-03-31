// SPDX-License-Identifier: GPL-2.0-or-later

#include "BridgeUtil.hpp"


namespace netconf {
MacLearning GetMacLearning([[maybe_unused]]::std::uint32_t if_index) {
  return MacLearning::UNKNOWN;
}

void SetMacLearning([[maybe_unused]]::std::uint32_t if_index, [[maybe_unused]]MacLearning learning) {
}
}

