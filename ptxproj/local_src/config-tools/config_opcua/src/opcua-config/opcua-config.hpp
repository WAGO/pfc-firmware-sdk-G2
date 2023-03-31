// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <vector>

#include <glib-2.0/glib.h>

namespace wago::opcua_config
{

// config tool parameters
namespace param_keys
{

const std::string param_enabled = "enabled";
const std::string param_ctrl_config_name = "customCtrlConfigName";
const std::string param_log_level = "logLevel";
const std::string param_sec_policy = "securityPolicyMin";
const std::string param_sec_mode = "securityModes";
const std::string param_plain_password = "allowPlaintextPassword";
const std::string param_anon_access = "anonymousAccess";

const std::string param_cert_name = "certAppName";
const std::string param_cert_company = "certCompany";
const std::string param_cert_city = "certCity";
const std::string param_cert_state = "certState";
const std::string param_cert_country = "certCountry";

const std::vector<std::string> all_keys
{
  param_plain_password,
  param_anon_access,
  param_cert_name,
  param_cert_city,
  param_cert_company,
  param_cert_country,
  param_cert_state,
  param_ctrl_config_name,
  param_enabled,
  param_log_level,
  param_sec_mode,
  param_sec_policy,
};

} // namespace wago::opcua_config::param_keys

class config_store final
{
public:
  config_store();
  ~config_store();

  /**
   * \brief Clear the store
   *
   */
  void reset();

  /**
   * \brief Check if the store has modified values
   *
   * \return bool
   */
  bool get_is_modified();
  /**
   * \brief Set if the store contains modified values
   *
   * \param is_modified
   */
  void set_is_modified(bool is_modified);

  /**
   * \brief Get the key file object
   *
   * \return GKeyFile*
   */
  GKeyFile * get_key_file();
  /**
   * \brief Load the config values from the config file or skip if already initialised
   *
   * \param file_path Path to the config file
   * \return bool Success
   */
  bool init_key_file(const std::string & file_path);

  /**
   * \brief Save the current config to a file, if changed
   *
   * \param file_path Path to the config file
   * \return bool Success
   */
  bool save_config_to_file(const std::string & file_path);

private:
  bool _is_modified;
  GKeyFile * _key_file;
};

/**
 * \brief Get just the value from a json object
 *
 * \param json_obj The source object
 * \param parameter_key The parameter to get the value for
 * \return std::string Empty string if the json could not be parsed or did not contain the key
 */
std::string get_raw_config_value_from_json(const std::string & json_obj, const std::string & parameter_key);

/**
 * \brief Get a config value in json
 *
 * \param store The source store
 * \param parameter_key The parameter to get the value for
 * \return std::string Empty string if any errors occured
 */
std::string get_config_value_json(config_store * store, const std::string & parameter_key);

/**
 * \brief Get a json array of all config values
 *
 * \param store The source store
 * \return std::string Empty json array is any errors occured
 */
std::string get_all_config_value_json(config_store * store);

/**
 * \brief Set a config value
 *
 * \param store The target store
 * \param parameter_key The parameter to set the value for
 * \param value The new value
 * \return bool True if the given \p parameter_key is valid
 */
bool set_config_value(config_store * store, const std::string & parameter_key, const std::string & value);

/**
 * \brief Perform a restart of the runtime
 *
 * \return bool Success
 */
bool trigger_runtime_restart();

} // namespace wago::opcua_config
