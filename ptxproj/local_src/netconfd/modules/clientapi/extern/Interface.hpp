// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <optional>
#include <memory>
#include <string>
#include <cstring>

#include "Status.hpp"
#include "Types.hpp"

namespace netconf {
namespace api {

/**
 * Create an Interface from a json string.
 * @return Interfaces object.
 */
Status MakeInterface(const ::std::string &json_str, Interface& interface);

/**
 * @brief Add Interface objects for the netconfd network
 *
 * @param interfaces
 * @return Status
 */
Status AddInterface(const Interface &interface);

/**
 * @brief Delete Interface objects for the netconfd network
 *
 * @param interfaces
 * @return Status
 */
Status DeleteInterface(const Interface &interface);

Status ValidateInterface(const Interface &interface, const Interfaces &existing_interfaces);

}  // namespace api
}  // namespace netconf
