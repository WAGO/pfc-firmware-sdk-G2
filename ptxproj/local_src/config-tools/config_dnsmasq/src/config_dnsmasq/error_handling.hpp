//------------------------------------------------------------------------------
/// Copyright (c) 2020-2024 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
///  \file     error_handling.hpp
///
///  \brief    Error handling.
///
///  \author   MSc : WAGO GmbH & Co. KG
///  \author   MOe : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef SRC_CONFIG_DNSMASQ_ERROR_HANDLING_HPP_
#define SRC_CONFIG_DNSMASQ_ERROR_HANDLING_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include <ct_liblog.h>


#ifdef __cplusplus
}
#endif

#include <boost/format.hpp>
#include <string>

#include "logger.hpp"

namespace configdnsmasq {

typedef struct {
    eStatusCode code;
    char const *text;
} erh_code_to_message_t;

/**
 * Set program name.
 *
 * @param prgname
 */
void erh_init(const ::std::string &prgname);

/**
 * Error handling: Set error message for WBM and terminate program.
 *
 * @param code
 * @param message
 * @param exit_on_error
 */
void erh_set_error(eStatusCode code, const std::string &message, bool exit_on_error = true);

/**
 * Assert a given condition
 *
 * @param condition
 * @param code
 * @param message
 * @param exit_on_error whether to terminate the program on error or not
 */
void erh_assert(bool condition, eStatusCode code, const std::string &message, bool exit_on_error = true);

/**
 * Assert a given condition
 *
 * @param condition
 * @param code
 * @param message
 * @param exit_on_error
 */
void erh_assert(bool condition, eStatusCode code, const boost::format &message, bool exit_on_error = true);


void erh_log(const std::string &message);

} // namespace configdnsmasq

#endif /* SRC_CONFIG_DNSMASQ_ERROR_HANDLING_HPP_ */
