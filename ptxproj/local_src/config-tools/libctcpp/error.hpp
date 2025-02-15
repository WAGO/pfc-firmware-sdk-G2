//------------------------------------------------------------------------------ 
// Copyright (c) WAGO GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file error.hpp
/// 
/// \version $Id$ 
/// 
/// \brief Error helper routines, such as standard messages, logging function
///        and extended exception class.
/// 
/// \author Mariusz Podlesny : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------


#ifndef WAGO_ERROR_HPP
#define WAGO_ERROR_HPP


//
//  BEFORE YOU START
//
//  The module should be treated as an extension for standard exception module,
//  as defined by C++, and is aimed at config-tools like applications, whose
//  error messages are directly used by GUI and communicated to the end user.
//  In case of standard exceptions, before they are reported to the user,
//  their original message should be logged but a replacement message should be
//  shown to the user instead.
//


#ifdef __cplusplus
extern "C" {
#endif

#include "../liblog/ct_liblog.h"

#ifdef __cplusplus
}
#endif


#include <stdexcept>
#include <string>


namespace wago
{


//
/// Common error messages.
//
extern const std::string error_msg_unknown;
extern const std::string error_msg_missing_param;
extern const std::string error_msg_invalid_param;
extern const std::string error_msg_file_open_error;
extern const std::string error_msg_file_read_error;
extern const std::string error_msg_file_close_error;
extern const std::string error_msg_system_call_error;
extern const std::string error_msg_invalid_config;


//------------------------------------------------------------------------------
/// Logs a message into a standard config-tools error file.
/// \param msg a message to be logged.
//------------------------------------------------------------------------------
void log_error_message(const std::string& msg);


//------------------------------------------------------------------------------
/// Execution error exception class.
/// A base class for all config-tools specific exceptions.
/// This is an extension of standard std::runtime_error, containing additional
/// 'code' member. Semantics of its use should be the same as in case of
/// std::runtime_error.
//------------------------------------------------------------------------------
class execution_error : public std::runtime_error
{
public:
    /// \param _what detailed error description, development level
    /// \param _code config-tools error code
    /// \param _msg config-tools public error message
    explicit inline execution_error(const std::string& _what, const int _code, const std::string& _msg)
        : std::runtime_error(_what), __code(_code), __msg(_msg) {}

    explicit inline execution_error(const int _code, const std::string& _msg)
        : std::runtime_error(std::string()), __code(_code), __msg(_msg) {}

    inline int code(void) const
            { return __code; }

    inline std::string msg(void) const
            { return __msg; }

private:
    int __code;
    std::string __msg; // String which may be presented to the user.
};


//
// Standard config-tools exceptions.
// Values carried by these exceptions are meant to be used directly by a GUI
// application for user interaction.
//

class unknown_error : public execution_error
{
public:
    inline unknown_error(const std::string& _what = std::string(),
                         const std::string& _msg = error_msg_unknown)
        : execution_error(_what, SYSTEM_CALL_ERROR, _msg) {}
};


class missing_param_error : public execution_error
{
public:
    inline missing_param_error(const std::string& _what = std::string(),
                               const std::string& _msg = error_msg_missing_param)
        : execution_error(_what, MISSING_PARAMETER, _msg) {}
};


class invalid_param_error : public execution_error
{
public:
    inline invalid_param_error(const std::string& _what = std::string(),
                               const std::string& _msg = error_msg_invalid_param)
        : execution_error(_what, INVALID_PARAMETER, _msg) {}
};


class file_open_error : public execution_error
{
public:
    inline file_open_error(const std::string& _what = std::string(),
                           const std::string& _msg = error_msg_file_open_error)
        : execution_error(_what, FILE_OPEN_ERROR, _msg) {}
};


class file_read_error : public execution_error
{
public:
    inline file_read_error(const std::string& _what = std::string(),
                           const std::string& _msg = error_msg_file_read_error)
        : execution_error(_what, FILE_READ_ERROR, _msg) {}
};


class file_close_error : public execution_error
{
public:
    inline file_close_error(const std::string& _what = std::string(),
                            const std::string& _msg = error_msg_file_close_error)
        : execution_error(_what, FILE_CLOSE_ERROR, _msg) {}
};


class system_call_error : public execution_error
{
public:
    inline system_call_error(const std::string& _what = std::string(),
                             const std::string& _msg = error_msg_system_call_error)
        : execution_error(_what, SYSTEM_CALL_ERROR, _msg) {}
};


class invalid_config_error : public execution_error
{
public:
    inline invalid_config_error(const std::string& _what = std::string(),
                                const std::string& _msg = error_msg_invalid_config)
        : execution_error(_what, CONFIG_FILE_INCONSISTENT, _msg) {}
};


} // namespace wago


#endif // WAGO_ERROR_HPP

