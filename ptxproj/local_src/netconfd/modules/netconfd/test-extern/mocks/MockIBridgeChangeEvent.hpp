// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "IBridgeChangeEvent.hpp"
#include "Types.hpp"

namespace netconf {

class MockIBridgeChangeEvent : public IBridgeChangeEvent {
 public:

  MOCK_CONST_METHOD1(OnBridgeRemove, void(const Interface& bridge) );
  MOCK_CONST_METHOD2(OnBridgeAddOrPortChange, void(const Interface& bridge, const Interfaces& ports) );
};


}  // namespace netconf
