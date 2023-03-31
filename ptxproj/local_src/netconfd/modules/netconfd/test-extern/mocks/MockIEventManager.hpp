// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "IEventManager.hpp"

namespace netconf {

class MockIEventManager : public IEventManager {
 public:
  MOCK_METHOD1(NotifyNetworkChanges, void(const EventLayer));
  MOCK_METHOD2(NotifyNetworkChanges, void(const EventLayer, ::std::optional<Interface>));
  MOCK_METHOD0(ProcessPendingEvents, void());
};

}  // namespace netconf
