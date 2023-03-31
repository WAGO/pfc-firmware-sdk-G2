// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "IDeviceTypeLabel.hpp"

namespace netconf {

class MockIDeviceTypeLabel : public IDeviceTypeLabel {
 public:
  MOCK_CONST_METHOD0(GetOrderNumber,
      ::std::string());
  MOCK_CONST_METHOD0(GetMac,
      MacAddress());
  MOCK_CONST_METHOD0(GetMacCount,
                     uint32_t());
};


}
