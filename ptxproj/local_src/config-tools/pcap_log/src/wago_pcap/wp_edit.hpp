//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     wp_edit.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_WAGO_PCAP_WP_EDIT_HPP_
#define SRC_WAGO_PCAP_WP_EDIT_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wp_parameterc.hpp"

#include <filesystem>
#include <fstream>

#include <pcapplusplus/DecryptionSecret.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define SECRET_TYPE_TLS "tls"

#define INJECT_SECRETS_LOG_PATH FOLDER_FLASH SUBFOLDER_PCAP "error_status.log"

namespace wp {

class SecretsHandler
{
public:
  static bool parse_secrets_type(const std::string & string_type, pcpp::DecryptionSecretType & type);

  static bool parse_secrets_file(
    const std::string & file_path,
    const pcpp::DecryptionSecretType & file_type,
    pcpp::DecryptionSecret & parsed);

  static bool inject_secrets(
    const std::vector<pcpp::DecryptionSecret> & secrets,
    const std::string & src_pcapng_path,
    const std::string & dest_pcapng_path = "");

};

}

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* SRC_WAGO_PCAP_WP_DEBUG_HPP_ */
//---- End of source file ------------------------------------------------------

