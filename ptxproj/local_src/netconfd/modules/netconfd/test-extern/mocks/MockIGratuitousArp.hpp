// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>
#include "IGratuitousArp.hpp"

namespace netconf {

class MockIGratuitousArp : public IGratuitousArp {
 public:
  MOCK_CONST_METHOD3(SendGratuitousArpOnPort, void(NetDevPtr, NetDevPtr, Address));
  MOCK_CONST_METHOD2(SendGratuitousArpOnBridge, void(NetDevPtr, Address));
  MOCK_CONST_METHOD1(EnableGratuitousArp, void(IPLinkPtr));
};

}
