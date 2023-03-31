// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Status.hpp"
#include "Types.hpp"

namespace netconf {

Status Validate(const Interface &interface, const Interfaces &existing_interfaces, const Interfaces &persisted_interfaces);
Status ValidateName(const Interface &interface);

}
