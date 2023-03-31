// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "opcua-config.hpp"
#include "loglevel-util.hpp"

#include <algorithm>
#include <cstdlib>
#include <nlohmann/json.hpp>

#include <typelabel_API.h>

using json = nlohmann::json;

namespace
{

constexpr int max_string_size = 500;

const std::string cmd_restart_runtime = "/etc/init.d/runtime stop; /usr/bin/sleep 1; /etc/init.d/runtime start";

// cfg file sections
const std::string section_server = "CmpOPCUAServer";
const std::string section_cds_provider = "CmpOPCUAProviderIecVarAccess";
const std::string section_stack = "CmpOPCUAStack";

// cfg file keys
const std::string cfg_key_enabled = "SECURITY.Activation";
const std::string cfg_key_ctrl_config_name = "CustomNodeName";
const std::string cfg_key_log_level = "TraceLevel";
const std::string cfg_key_sec_policy = "SECURITY.CommunicationPolicy";
const std::string cfg_key_sec_mode = "SECURITY.CommunicationMode";
const std::string cfg_key_plain_password = "SECURITY.AllowUserPasswordOnPlaintext";
const std::string cfg_key_anon_access = "SECURITY.UserAuthentication";
const std::string cfg_key_cert_name = "SECURITY.ApplicationName";
const std::string cfg_key_cert_company = "SECURITY.CompanyOrOrganizationName";
const std::string cfg_key_cert_city = "SECURITY.City";
const std::string cfg_key_cert_state = "SECURITY.State";
const std::string cfg_key_cert_country = "SECURITY.Country";

// bool "variants"
const std::map<std::string, bool> bool_de_activated
{
  {"ACTIVATED", true},
  {"DEACTIVATED", false}
};
const std::map<std::string, bool> bool_yes_no
{
  {"YES", true},
  {"NO", false}
};
const std::map<std::string, bool> bool_enabled_enforced
{
  {"ENABLED", true},
  {"ENFORCED", false}
};

// allowed security policy values
const std::map<std::string, std::string> sec_policy_values
{
  // {"POLICY_BASIC128RSA15", "Basic128Rsa15"},
  // {"POLICY_BASIC256", "Basic256"},
  {"POLICY_AES128SHA256RSAOAEP", "Aes128Sha256RsaOaep"},
  {"POLICY_BASIC256SHA256", "Basic256Sha256"},
  {"POLICY_AES256SHA256RSAPSS", "Aes256Sha256RsaPss"},
};

// allowed security mode values
const std::map<std::string, std::string> sec_mode_values
{
  {"ONLY_PLAINTEXT", "None"},
  {"ALL", "None+Sign+SignEncrypt"},
  {"MIN_SIGNED", "Sign+SignEncrypt"},
  {"SIGNED_AND_ENCRYPTED", "SignEncrypt"},
};

template <typename T>
std::string find_reverse_map_value(const T & value, const std::map<std::string, T> & mapping)
{
  auto it = find_if(mapping.begin(), mapping.end(), [value](const auto & m) {
    return m.second == value;
  });
  if (it != mapping.end())
  {
    return it->first;
  }

  return "";
}

std::string get_config_value_string(wago::opcua_config::config_store * store, const std::string & section, const std::string & key, const std::string & fallback_value, size_t max_size)
{
  if (store->get_key_file() == nullptr)
  {
    return fallback_value;
  }

  GError * gerror = nullptr;

  char * raw_result = g_key_file_get_string(store->get_key_file(), section.c_str(), key.c_str(), &gerror);
  std::string result = fallback_value;

  if (raw_result != nullptr && (strlen(raw_result) + 1) <= max_size)
  {
    result = raw_result;
  }

  if (gerror != nullptr)
  {
    g_error_free(gerror);
  }

  if (raw_result != nullptr)
  {
    g_free(raw_result);
  }

  return result;
}

void set_config_value_string(wago::opcua_config::config_store * store, const std::string & section, const std::string & key, const std::string & value)
{
  if (store->get_key_file() == nullptr)
  {
    return;
  }

  g_key_file_set_string(store->get_key_file(), section.c_str(), key.c_str(), value.c_str());
  store->set_is_modified(true);
}

bool get_config_value_bool(wago::opcua_config::config_store * store, const std::string & section, const std::string & key, bool fallback_value, const std::map<std::string, bool> & bool_mapping)
{
  if (store->get_key_file() == nullptr)
  {
    return fallback_value;
  }

  bool result = fallback_value;

  std::string raw_result = get_config_value_string(store, section, key, "", max_string_size);

  if (raw_result != "")
  {
    auto map_search = bool_mapping.find(raw_result);
    if (map_search != bool_mapping.end())
    {
      result = map_search->second;
    }
  }

  return result;
}

void set_config_value_bool(wago::opcua_config::config_store * store, const std::string & section, const std::string & key, bool value, const std::map<std::string, bool> & bool_mapping)
{
  if (store->get_key_file() == nullptr)
  {
    return;
  }

  std::string mapped_value = find_reverse_map_value(value, bool_mapping);
  if (mapped_value != "")
  {
    set_config_value_string(store, section, key, mapped_value);
  }
}

int get_config_value_int(wago::opcua_config::config_store * store, const std::string & section, const std::string & key, int fallback_value)
{
  if (store->get_key_file() == nullptr)
  {
    return fallback_value;
  }

  GError * gerror = nullptr;

  int result = g_key_file_get_integer(store->get_key_file(), section.c_str(), key.c_str(), &gerror);

  if (gerror != nullptr)
  {
    result = fallback_value;

    g_error_free(gerror);
  }

  return result;
}

void set_config_value_int(wago::opcua_config::config_store * store, const std::string & section, const std::string & key, int value)
{
  if (store->get_key_file() == nullptr)
  {
    return;
  }

  g_key_file_set_integer(store->get_key_file(), section.c_str(), key.c_str(), value);
  store->set_is_modified(true);
}

template <typename T>
std::string key_value_to_json(const std::string & key, const T & value)
{
  json j;
  j[key] = value;
  return j.dump();
}

std::string get_typelabel_descr()
{
  std::string result = "";

  tTypeLabel typelabel = typelabel_OBJECT_New();
  if (typelabel_OBJECT_Open(typelabel) == TYPELABEL_RETURN_OK)
  {
    char * value = nullptr;
    if (typelabel_OBJECT_Sync(typelabel) == TYPELABEL_RETURN_OK && (value = typelabel_OBJECT_GetValue(typelabel, "DESCR")) != nullptr)
    {
      result = value;
    }

    typelabel_OBJECT_Close(typelabel);
    typelabel_OBJECT_Destroy(typelabel);
  }

  return result;
}

} // namespace

namespace wago::opcua_config
{

config_store::config_store() : _is_modified{false}, _key_file{nullptr}
{
}

config_store::~config_store()
{
  reset();
}

void config_store::reset()
{
  _is_modified = false;
  if (_key_file != nullptr)
  {
    g_key_file_free(_key_file);
  }
}

bool config_store::get_is_modified()
{
  return _is_modified;
}

void config_store::set_is_modified(bool is_modified)
{
  _is_modified = is_modified;
}

GKeyFile * config_store::get_key_file()
{
  return _key_file;
}

bool config_store::init_key_file(const std::string & file_path)
{
  if (_key_file != nullptr)
  {
    return true;
  }

  bool result;
  GError * gerror = nullptr;

  _key_file = g_key_file_new();
  result = g_key_file_load_from_file(_key_file, file_path.c_str(), G_KEY_FILE_NONE, &gerror);

  if (!result)
  {
    if (gerror != nullptr && gerror->code == G_FILE_ERROR_NOENT)
    {
      result = true;
    }
    else
    {
      g_key_file_free(_key_file);
    }

  }

  if (gerror != nullptr)
  {
    g_error_free(gerror);
  }

  return result;
}

bool config_store::save_config_to_file(const std::string & file_path)
{
  if (!get_is_modified())
  {
    return true;
  }
  if (get_key_file() == nullptr)
  {
    return false;
  }

  bool result = false;

  gsize data_length;
  GError * gerror = nullptr;
  gchar * data = g_key_file_to_data(get_key_file(), &data_length, &gerror);

  if (data != nullptr)
  {
    FILE * file;
    file = fopen(file_path.c_str(), "w");

    if (file != nullptr)
    {
      size_t written = fwrite(data, 1, data_length, file);
      if (written == data_length)
      {
        result = true;
      }

      fclose(file);
    }

    g_free(data);
  }

  if (gerror != nullptr)
  {
    g_error_free(gerror);
  }

  return result;
}

std::string get_raw_config_value_from_json(const std::string & json_obj, const std::string & parameter_key)
{
  std::string result = "";
  json j = json::parse(json_obj);
  if (j.is_object() && j.contains(parameter_key))
  {
    result = j[parameter_key].dump();
  }

  return result;
}

std::string get_config_value_json(config_store * store, const std::string & parameter_key)
{
  std::string result = "";
  if (parameter_key == param_keys::param_enabled)
  {
    bool value = get_config_value_bool(store, section_server, cfg_key_enabled, true, bool_de_activated);
    result = key_value_to_json(param_keys::param_enabled, value);
  }
  else if (parameter_key == param_keys::param_ctrl_config_name)
  {
    const std::string typelabel_descr = get_typelabel_descr();
    std::string value = get_config_value_string(store, section_cds_provider, cfg_key_ctrl_config_name, typelabel_descr, max_string_size);
    if (value == "")
    {
      value = typelabel_descr;
    }

    result = key_value_to_json(param_keys::param_ctrl_config_name, value);
  }
  else if (parameter_key == param_keys::param_log_level)
  {
    int value = get_config_value_int(store, section_stack, cfg_key_log_level, log_levels::warning);
    result = key_value_to_json(param_keys::param_log_level, log_levels::string_from_log_level(value));
  }
  else if (parameter_key == param_keys::param_sec_policy)
  {
    const std::string fallback = "POLICY_AES128SHA256RSAOAEP";
    std::string raw_value = get_config_value_string(store, section_server, cfg_key_sec_policy, fallback, max_string_size);
    auto it = sec_policy_values.find(raw_value);

    std::string value;
    if (it == sec_policy_values.end())
    {
      value = sec_policy_values.find(fallback)->second;
    }
    else
    {
      value = it->second;
    }

    result = key_value_to_json(param_keys::param_sec_policy, value);
  }
  else if (parameter_key == param_keys::param_sec_mode)
  {
    const std::string fallback = "ALL";
    std::string raw_value = get_config_value_string(store, section_server, cfg_key_sec_mode, fallback, max_string_size);
    auto it = sec_mode_values.find(raw_value);

    std::string value;
    if (it == sec_mode_values.end())
    {
      value = sec_mode_values.find(fallback)->second;
    }
    else
    {
      value = it->second;
    }

    result = key_value_to_json(param_keys::param_sec_mode, value);
  }
  else if (parameter_key == param_keys::param_plain_password)
  {
    bool value = get_config_value_bool(store, section_server, cfg_key_plain_password, true, bool_yes_no);
    result = key_value_to_json(param_keys::param_plain_password, value);
  }
  else if (parameter_key == param_keys::param_anon_access)
  {
    bool value = get_config_value_bool(store, section_server, cfg_key_anon_access, true, bool_enabled_enforced);
    result = key_value_to_json(param_keys::param_anon_access, value);
  }
  else if (parameter_key == param_keys::param_cert_name)
  {
    std::string value = get_config_value_string(store, section_server, cfg_key_cert_name, "", max_string_size);
    result = key_value_to_json(param_keys::param_cert_name, value);
  }
  else if (parameter_key == param_keys::param_cert_company)
  {
    std::string value = get_config_value_string(store, section_server, cfg_key_cert_company, "", max_string_size);
    result = key_value_to_json(param_keys::param_cert_company, value);
  }
  else if (parameter_key == param_keys::param_cert_city)
  {
    std::string value = get_config_value_string(store, section_server, cfg_key_cert_city, "", max_string_size);
    result = key_value_to_json(param_keys::param_cert_city, value);
  }
  else if (parameter_key == param_keys::param_cert_state)
  {
    std::string value = get_config_value_string(store, section_server, cfg_key_cert_state, "", max_string_size);
    result = key_value_to_json(param_keys::param_cert_state, value);
  }
  else if (parameter_key == param_keys::param_cert_country)
  {
    std::string value = get_config_value_string(store, section_server, cfg_key_cert_country, "", max_string_size);
    result = key_value_to_json(param_keys::param_cert_country, value);
  }

  return result;
}

std::string get_all_config_value_json(config_store * store)
{
  json j = json::object();

  for (const auto & key : param_keys::all_keys)
  {
    std::string single_result = get_config_value_json(store, key);
    j.update(json::parse(single_result));
  }

  return j.dump();
}

bool set_config_value(config_store * store, const std::string & parameter_key, const std::string & value)
{
  bool param_found = true;
  if (parameter_key == param_keys::param_enabled)
  {
    set_config_value_bool(store, section_server, cfg_key_enabled, (value == "true"), bool_de_activated);
  }
  else if (parameter_key == param_keys::param_ctrl_config_name)
  {
    set_config_value_string(store, section_cds_provider, cfg_key_ctrl_config_name, value);
  }
  else if (parameter_key == param_keys::param_log_level)
  {
    int raw_value = log_levels::log_level_from_string(value);
    set_config_value_int(store, section_stack, cfg_key_log_level, raw_value);
  }
  else if (parameter_key == param_keys::param_sec_policy)
  {
    std::string raw_value = find_reverse_map_value(value, sec_policy_values);
    if (raw_value == "")
    {
      raw_value = "POLICY_AES128SHA256RSAOAEP";
    }
    set_config_value_string(store, section_server, cfg_key_sec_policy, raw_value);
  }
  else if (parameter_key == param_keys::param_sec_mode)
  {
    std::string raw_value = find_reverse_map_value(value, sec_mode_values);
    if (raw_value == "")
    {
      raw_value = "ALL";
    }
    set_config_value_string(store, section_server, cfg_key_sec_mode, raw_value);
  }
  else if (parameter_key == param_keys::param_plain_password)
  {
    set_config_value_bool(store, section_server, cfg_key_plain_password, (value == "true"), bool_yes_no);
  }
  else if (parameter_key == param_keys::param_anon_access)
  {
    set_config_value_bool(store, section_server, cfg_key_anon_access, (value == "true"), bool_enabled_enforced);
  }
  else if (parameter_key == param_keys::param_cert_name)
  {
    set_config_value_string(store, section_server, cfg_key_cert_name, value);
  }
  else if (parameter_key == param_keys::param_cert_company)
  {
    set_config_value_string(store, section_server, cfg_key_cert_company, value);
  }
  else if (parameter_key == param_keys::param_cert_city)
  {
    set_config_value_string(store, section_server, cfg_key_cert_city, value);
  }
  else if (parameter_key == param_keys::param_cert_state)
  {
    set_config_value_string(store, section_server, cfg_key_cert_state, value);
  }
  else if (parameter_key == param_keys::param_cert_country)
  {
    set_config_value_string(store, section_server, cfg_key_cert_country, value);
  }
  else
  {
    param_found = false;
  }

  return param_found;
}

bool trigger_runtime_restart()
{
  bool success = false;
  if (system(cmd_restart_runtime.c_str()) != -1)
  {
    success = true;
  }

  return success;
}

} // namespace wago::opcua_config
