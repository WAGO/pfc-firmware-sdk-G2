// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <gmock/gmock.h>

#include "IIPLinks.hpp"

namespace netconf {

class MockIIPLinks : public IIPLinks {
 public:

  MOCK_METHOD1(CreateOrGet, ::std::shared_ptr<IPLink>(const Interface& ip_config) );

};

}
