//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     wp_edit.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wp_edit.hpp"
#include "wp_debug.hpp"
#include "wp_util.hpp"

#include <memory>
#include <pcapplusplus/PcapFileDevice.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define FREE_SPACE_EXTRA_BUFFER_PERC 10
#define FILE_EXT_INJECTED ".secret.pcapng"
#define FILE_EXT_TMP ".tmp"

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

//---- End of source file ------------------------------------------------------

bool wp::SecretsHandler::parse_secrets_type(const std::string & string_type, pcpp::DecryptionSecretType & type)
{
  if (string_type == SECRET_TYPE_TLS)
  {
    type = pcpp::DecryptionSecretType::TLS_KEY_LOG;
  }
  // TODO: other types not supported for now
  else
  {
    return false;
  }

  return true;
}

bool wp::SecretsHandler::parse_secrets_file(
  const std::string & file_path,
  const pcpp::DecryptionSecretType & file_type,
  pcpp::DecryptionSecret & parsed)
{
  if (!std::filesystem::exists(file_path))
  {
    Debug_Printf("Given secret log file does not exist.\n");
    return false;
  }
  std::ifstream secrets_file{file_path};
  std::stringstream secrets_buffer;
  secrets_buffer << secrets_file.rdbuf();
  std::string contents = secrets_buffer.str();
  uint8_t * bytes = reinterpret_cast<uint8_t *>(contents.data());

  parsed = pcpp::DecryptionSecret(file_type, contents.size(), bytes);
  return true;
}

bool wp::SecretsHandler::inject_secrets(
  const std::vector<pcpp::DecryptionSecret>& secrets,
  const std::string & src_pcapng_path,
  const std::string & dest_pcapng_path)
{
  if (!std::filesystem::exists(src_pcapng_path))
  {
    Debug_PrintAndLogToFile(INJECT_SECRETS_LOG_PATH, "Given pcap(ng) file \"%s\" does not exist\n", src_pcapng_path.c_str());
    return false;
  }

  std::string dest_path = dest_pcapng_path;
  if (dest_path == "")
  {
    dest_path = src_pcapng_path;
    if (!ends_with(dest_path, FILE_EXT_INJECTED))
    {
      auto path = std::filesystem::path(dest_path);
      dest_path = path.replace_extension(FILE_EXT_INJECTED);
    }
    else
    {
      dest_path += FILE_EXT_TMP;
    }
  }

  // check available space first, needs src file size + some margin in case the src gets converted from pcap to pcapng
  auto src_file_size = get_file_size(src_pcapng_path);
  auto free_space = get_free_space_left(src_pcapng_path);
  auto available_file_size = free_space - src_file_size * FREE_SPACE_EXTRA_BUFFER_PERC / 100;
  if (src_file_size > available_file_size || src_file_size > free_space)
  {
    Debug_PrintAndLogToFile(INJECT_SECRETS_LOG_PATH, "Not enough space left to create destination pcapng file \"%s\".\n", dest_path.c_str());
    return false;
  }

  pcpp::PcapNgFileWriterDevice writer{dest_path};
  if (!writer.open())
  {
    Debug_PrintAndLogToFile(INJECT_SECRETS_LOG_PATH, "Failed to open destination pcapng file \"%s\".\n", dest_path.c_str());
    return false;
  }
  // pcapng writer creates file with default permissions, fix them
  std::filesystem::permissions(dest_path, get_allowed_permissions());
  set_owner_group_webserver(dest_path);

  auto reader = pcpp::IFileReaderDevice::getReader(src_pcapng_path);
  if (reader == nullptr || !reader->open())
  {
    Debug_PrintAndLogToFile(INJECT_SECRETS_LOG_PATH, "Failed to open source pcap(ng) file \"%s\".\n", src_pcapng_path.c_str());
    return false;
  }

  // read existing secrets to carry them over
  std::vector<pcpp::DecryptionSecret> all_secrets = secrets;
  if (std::filesystem::path(src_pcapng_path).extension() == ".pcapng")
  {
    auto ng_reader = static_cast<pcpp::PcapNgFileReaderDevice *>(reader);
    ng_reader->getDecryptionSecrets(all_secrets);
    // remove duplicates to avoid growing the file every time injection is run
    std::sort(all_secrets.begin(), all_secrets.end());
    all_secrets.erase(std::unique(all_secrets.begin(), all_secrets.end()), all_secrets.end());
  }

  // secrets should be written *before* packets needing them
  if (!writer.writeDecryptionSecrets(all_secrets))
  {
    Debug_PrintAndLogToFile(INJECT_SECRETS_LOG_PATH, "Failed to write secrets into \"%s\".\n", dest_path.c_str());
    return false;
  }

  pcpp::RawPacket packet;
  while (reader->getNextPacket(packet))
  {
    writer.writePacket(packet);
  }

  writer.flush();
  writer.close();
  reader->close();
  delete reader;

  // if tmp file was written to, move it to original, otherwise delete original
  auto path = std::filesystem::path(dest_path);
  if (path.extension() == FILE_EXT_TMP)
  {
    path = path.replace_extension();
    std::filesystem::rename(dest_path, path);
  }
  else if (dest_pcapng_path == "")
  {
    std::filesystem::remove(src_pcapng_path);
  }

  Debug_PrintAndLogToFile(INJECT_SECRETS_LOG_PATH, "Injected secrets into \"%s\".\n", path.c_str());
  return true;
}
