// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "snmp_config_handler.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "SnmpConfig.hpp"

namespace wago::snmp_config {
namespace {

using namespace ::std::string_literals;

using snmp_config_lib::VersionV1V2c;
using ::wago::snmp_config_lib::Access;
using ::wago::snmp_config_lib::AuthenticationType;
using ::wago::snmp_config_lib::Community;
using ::wago::snmp_config_lib::Privacy;
using ::wago::snmp_config_lib::SecurityLevel;
using ::wago::snmp_config_lib::SnmpConfig;
using ::wago::snmp_config_lib::TrapReceiverV1V2c;
using ::wago::snmp_config_lib::TrapReceiverV3;
using ::wago::snmp_config_lib::User;

enum class edit_operation { add_op, delete_op };

constexpr auto enabled  = "enabled";
constexpr auto disabled = "disabled";

constexpr auto basic_set_snmp          = "set-snmp";
constexpr auto basic_device_name       = "device-name";
constexpr auto basic_description       = "description";
constexpr auto basic_physical_location = "physical-location";
constexpr auto basic_contact           = "contact";
constexpr auto basic_object_id         = "objectID";
constexpr auto basic_state_value       = "v1-v2c-state";

constexpr auto v1v2c_community_edit                = "v1-v2c-community-edit";
constexpr auto v1v2c_community_name                = "v1-v2c-community-name";
constexpr auto v1v2c_community_access              = "v1-v2c-community-access";
constexpr auto v1v2c_community_required_params_add = {
    v1v2c_community_name,
    v1v2c_community_access,
};

constexpr auto v1v2c_community_required_params_delete = {
    v1v2c_community_name,
};

constexpr auto v1v2c_trap_receiver_edit            = "v1-v2c-trap-receiver-edit";
constexpr auto v1v2c_trap_receiver_community_name  = "v1-v2c-trap-receiver-community-name";
constexpr auto v1v2c_trap_receiver_address         = "v1-v2c-trap-receiver-address";
constexpr auto v1v2c_trap_receiver_version         = "v1-v2c-trap-receiver-version";
constexpr auto v1v2c_trap_receiver_required_params = {
    v1v2c_trap_receiver_community_name,
    v1v2c_trap_receiver_address,
    v1v2c_trap_receiver_version,
};

constexpr auto v3_edit                  = "v3-edit";
constexpr auto v3_user_edit             = "v3-user-edit";
constexpr auto v3_trap_receiver_edit    = "v3-trap-receiver-edit";
constexpr auto v3_auth_name             = "v3-auth-name";
constexpr auto v3_auth_type             = "v3-auth-type";
constexpr auto v3_auth_key              = "v3-auth-key";
constexpr auto v3_privacy_type          = "v3-privacy";
constexpr auto v3_privacy_key           = "v3-privacy-key";
constexpr auto v3_user_access           = "v3-user-access";
constexpr auto v3_notification_reveiver = "v3-notification-receiver";

constexpr auto v3_trap_receiver_required_params_add = {v3_auth_name,    v3_auth_key,    v3_auth_type,
                                                       v3_privacy_type, v3_privacy_key, v3_notification_reveiver};

constexpr auto v3_trap_receiver_required_params_delete = {
    v3_auth_name,
    v3_notification_reveiver,
};

constexpr auto v3_user_required_params_add = {
    v3_auth_name, v3_auth_key, v3_auth_type, v3_privacy_type, v3_privacy_key,
};

constexpr auto v3_user_required_params_delete = {
    v3_auth_name,
};

constexpr auto all_parameters = {
    basic_set_snmp,
    basic_device_name,
    basic_description,
    basic_physical_location,
    basic_contact,
    basic_object_id,
    basic_state_value,
    v1v2c_community_edit,
    v1v2c_community_name,
    v1v2c_community_access,
    v1v2c_trap_receiver_edit,
    v1v2c_trap_receiver_community_name,
    v1v2c_trap_receiver_address,
    v1v2c_trap_receiver_version,
    v3_edit,
    v3_user_edit,
    v3_trap_receiver_edit,
    v3_auth_name,
    v3_auth_type,
    v3_auth_key,
    v3_privacy_type,
    v3_privacy_key,
    v3_user_access,
    v3_notification_reveiver,
};

template <typename T>
using setter_func_map = ::std::map<::std::string, ::std::function<void(T&, ::std::string const&)>>;

void to_lower(::std::string& value) {
  std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return std::tolower(c); });
}

template <typename T>
T parse_enum(::std::map<::std::string, T> const& map, ::std::string value) {
  to_lower(value);
  if (map.count(value) > 0) {
    return map.at(value);
  }

  throw snmp_config_exception(snmp_config_error::invalid_parameter, "invalid conversion: "s + value);
}

Access parse_access(::std::string const& value) {
  static const ::std::map<::std::string, Access> access_map = {
      {"read-only", Access::ReadOnly},
      {"read-write", Access::ReadWrite},
  };
  return parse_enum(access_map, value);
}

AuthenticationType parse_authentication(::std::string const& value) {
  static const ::std::map<::std::string, AuthenticationType> auth_map = {
      {"none", AuthenticationType::None},     {"md5", AuthenticationType::MD5},
      {"sha", AuthenticationType::SHA},       {"sha224", AuthenticationType::SHA224},
      {"sha256", AuthenticationType::SHA256}, {"sha384", AuthenticationType::SHA384},
      {"sha512", AuthenticationType::SHA512},
  };

  return parse_enum(auth_map, value);
}

Privacy parse_privacy(::std::string const& value) {
  static const ::std::map<::std::string, Privacy> privacy_map = {
      {"none", Privacy::None},     {"des", Privacy::DES},         {"aes", Privacy::AES},
      {"aes128", Privacy::AES128}, {"aes192", Privacy::AES192},   {"aes192c", Privacy::AES192C},
      {"aes256", Privacy::AES256}, {"aes256c", Privacy::AES256C},

  };

  return parse_enum(privacy_map, value);
}

VersionV1V2c parse_version(::std::string const& value) {
  static const ::std::map<::std::string, VersionV1V2c> version_map{
      {"v1", VersionV1V2c::V1},
      {"v2c", VersionV1V2c::V2c},
  };
  return parse_enum(version_map, value);
}

::std::optional<edit_operation> parse_operation(::std::optional<::std::string> const& value) {
  static const ::std::map<::std::string, edit_operation> operation_map{
      {"add", edit_operation::add_op},
      {"delete", edit_operation::delete_op},
  };
  return value ? ::std::make_optional(parse_enum(operation_map, value.value())) : ::std::nullopt;
}

template <typename T>
void check_that_all_parameters_are_given(parameter_map const& given, T const& required) {
  ::std::vector<::std::string> missing_parameters;
  missing_parameters = {
      ::std::find_if_not(required.begin(), required.end(), [&](auto const& p) { return given.count(p) > 0; }),
      required.end()};

  if (not missing_parameters.empty()) {
    throw snmp_config_exception(snmp_config_error::missing_parameter,
                                "missing parameter: " + missing_parameters.front());
  }
}

template <typename T>
void check_that_all_parameters_are_given(parameter_map const& given, T const& required_add, T const& required_delete,
                                         edit_operation edit_operation) {
  ::std::vector<::std::string> missing_parameters;
  if (edit_operation == edit_operation::add_op) {
    check_that_all_parameters_are_given(given, required_add);
  } else if (edit_operation == edit_operation::delete_op) {
    check_that_all_parameters_are_given(given, required_delete);
  }
}

template <typename T>
void update_security_level(T& user_or_trap) {
  const AuthenticationType auth = user_or_trap.authentication_type_;
  const Privacy priv            = user_or_trap.privacy_;

  if (auth == AuthenticationType::None) {
    user_or_trap.security_level_ = SecurityLevel::NoAuthNoPriv;
    if (priv != Privacy::None) {
      throw snmp_config_exception(snmp_config_error::missing_parameter, "privacy without authentication is invalid");
    }
  } else {
    if (priv == Privacy::None) {
      user_or_trap.security_level_ = SecurityLevel::AuthNoPriv;
    } else {
      user_or_trap.security_level_ = SecurityLevel::AuthPriv;
    }
  }
}

template <typename T>
void read_parameters_to(parameter_map const& parameters, setter_func_map<T> const& setter, T& to) {
  for (auto& p : parameters) {
    if (setter.count(p.first) > 0) {
      setter.at(p.first)(to, p.second);
    }
  }
}

template <typename T>
T create_from_parameters(parameter_map const& parameters, setter_func_map<T> const& setter) {
  T t{};
  read_parameters_to(parameters, setter, t);
  return t;
}

template <typename T>
T read_v3_user_or_trap(parameter_map const& parameters) {
  static const setter_func_map<T> set = {
      {v3_auth_name, [](T& e, ::std::string const& val) { e.name_ = val; }},
      {v3_auth_type, [](T& e, ::std::string const& val) { e.authentication_type_ = parse_authentication(val); }},
      {v3_auth_key, [](T& e, ::std::string const& val) { e.authentication_key_ = val; }},
      {v3_privacy_type, [](T& e, ::std::string const& val) { e.privacy_ = parse_privacy(val); }},
      {v3_privacy_key, [](T& e, std::string const& val) { e.privacy_key_ = val; }},
  };

  auto t = create_from_parameters(parameters, set);
  update_security_level(t);
  return t;
}

TrapReceiverV1V2c read_v1v2c_trap_receiver(parameter_map const& parameters) {
  static const setter_func_map<TrapReceiverV1V2c> setter_funcs = {
      {v1v2c_trap_receiver_address, [](TrapReceiverV1V2c& tr, ::std::string const& val) { tr.host_ = val; }},
      {v1v2c_trap_receiver_community_name,
       [](TrapReceiverV1V2c& tr, ::std::string const& val) { tr.community_ = val; }},
      {v1v2c_trap_receiver_version,
       [](TrapReceiverV1V2c& tr, ::std::string const& val) { tr.version_ = parse_version(val); }},
  };

  return create_from_parameters(parameters, setter_funcs);
}

Community read_v1v2c_community(parameter_map const& parameters) {
  static const setter_func_map<Community> setter_funcs = {
      {v1v2c_community_name, [](Community& c, ::std::string const& val) { c.name_ = val; }},
      {v1v2c_community_access, [](Community& c, ::std::string const& val) { c.access_ = parse_access(val); }}};

  return create_from_parameters(parameters, setter_funcs);
}

::std::optional<::std::string> get_parameter_value(parameter_map const& parameters, ::std::string const& key) {
  return (parameters.count(key) > 0) ? ::std::make_optional(parameters.at(key)) : ::std::nullopt;
}

::std::optional<::std::string> get_v3_trap_receiver_host(parameter_map const& parameters) {
  return get_parameter_value(parameters, v3_notification_reveiver);
}

::std::optional<edit_operation> get_v3_edit_operation(parameter_map const& parameters) {
  auto value = get_parameter_value(parameters, v3_edit);
  return parse_operation(value);
}

::std::optional<edit_operation> get_v3_user_edit_operation(parameter_map const& parameters) {
  auto value = get_parameter_value(parameters, v3_user_edit);
  return parse_operation(value);
}

::std::optional<edit_operation> get_v3_trap_receiver_edit_operation(parameter_map const& parameters) {
  auto value = get_parameter_value(parameters, v3_trap_receiver_edit);
  return parse_operation(value);
}

::std::optional<edit_operation> get_v1v2c_trap_edit_operation(parameter_map const& parameters) {
  auto value = get_parameter_value(parameters, v1v2c_trap_receiver_edit);
  return parse_operation(value);
}

::std::optional<edit_operation> get_v1v2c_community_edit_operation(parameter_map const& parameters) {
  auto value = get_parameter_value(parameters, v1v2c_community_edit);
  return parse_operation(value);
}

void set_snmp_enable(wago::snmp_config_lib::SnmpConfig& config, ::std::string const& value) {
  if (value == enabled) {
    config.snmp_enable_ = true;
  }
  if (value == disabled) {
    config.snmp_enable_ = false;
  }
}

void set_snmp_community_legacy(::std::vector<Community>& communities, ::std::string const& value) {
  if (value.empty()) {
    communities.erase(std::next(communities.begin()));
  } else {
    auto community = Community{value, Access::ReadWrite};
    if (communities.empty()) {
      communities.emplace_back(community);
    } else {
      communities.front() = community;
    }
  }
}

template <typename T, typename DeletePredicate>
void add_or_delete(::std::vector<T>& container, edit_operation edit_operation, T& entry,
                   DeletePredicate delete_predicate) {
  if (edit_operation == edit_operation::add_op) {
    container.emplace_back(entry);
  } else if (edit_operation == edit_operation::delete_op) {
    auto it = std::remove_if(container.begin(), container.end(), delete_predicate);
    auto r = std::distance(it, container.end());
    container.erase(it, container.end());
    if (r == 0) {
      throw snmp_config_exception(snmp_config_error::invalid_parameter, "entry (community/trap/user) does not exist");
    }
  }
}

void edit_v3_trap(parameter_map const& parameters, edit_operation edit_operation,
                  wago::snmp_config_lib::SnmpConfig& config) {
  check_that_all_parameters_are_given(parameters, v3_trap_receiver_required_params_add,
                                      v3_trap_receiver_required_params_delete, edit_operation);

  auto trap_receiver  = read_v3_user_or_trap<TrapReceiverV3>(parameters);
  trap_receiver.host_ = get_v3_trap_receiver_host(parameters).value();
  add_or_delete(config.trap_receivers_V3_, edit_operation, trap_receiver,
                [&](auto const& t) { return t.host_ == trap_receiver.host_ && t.name_ == trap_receiver.name_; });
}

void edit_v3_user(parameter_map const& parameters, edit_operation edit_operation,
                  wago::snmp_config_lib::SnmpConfig& config) {
  check_that_all_parameters_are_given(parameters, v3_user_required_params_add, v3_user_required_params_delete,
                                      edit_operation);

  auto user = read_v3_user_or_trap<User>(parameters);
  // user-access defaults to rw
  auto access_value = get_parameter_value(parameters, v3_user_access).value_or("read-write");
  user.access_      = parse_access(access_value);
  add_or_delete(config.user_, edit_operation, user, [&](auto const& t) { return t.name_ == user.name_; });
}

}  // namespace

void check_that_parameters_are_known(parameter_map const& parameters) {
  static const auto known_parameters = ::std::set<::std::string>{all_parameters.begin(), all_parameters.end()};
  for (auto& parameter : parameters) {
    if (known_parameters.count(parameter.first) == 0) {
      throw snmp_config_exception(snmp_config_error::invalid_parameter, "invalid command line: " + parameter.second);
    }
  }
}

void edit_basic_parameter(parameter_map const& parameters, SnmpConfig& config) {
  const setter_func_map<SnmpConfig> setter_funcs = {
      {basic_set_snmp, [](SnmpConfig& c, ::std::string const& v) { set_snmp_enable(c, v); }},
      {basic_device_name, [](SnmpConfig& c, ::std::string const& val) { c.sys_name_ = val; }},
      {basic_description, [](SnmpConfig& c, ::std::string const& val) { c.sys_descr_ = val; }},
      {basic_physical_location, [](SnmpConfig& c, ::std::string const& val) { c.sys_location_ = val; }},
      {basic_contact, [](SnmpConfig& c, ::std::string const& val) { c.sys_contact_ = val; }},
      {basic_object_id, [](SnmpConfig& c, ::std::string const& val) { c.sys_objectID_ = val; }},
      {basic_state_value,
       [](SnmpConfig& c, ::std::string const& val) {
         if (val == disabled) {
           set_snmp_community_legacy(c.communities_, "");
         }
       }},
      {v1v2c_community_name,
       [&](SnmpConfig& c, ::std::string const& val) {
         // only execute legacy set community when community-edit is not given
         if (not get_v1v2c_community_edit_operation(parameters)) {
           set_snmp_community_legacy(c.communities_, val);
         }
       }},
  };

  read_parameters_to(parameters, setter_funcs, config);
}

void edit_v1v2c_trap_receiver(parameter_map const& parameters, wago::snmp_config_lib::SnmpConfig& config) {
  auto edit = get_v1v2c_trap_edit_operation(parameters);
  if (edit) {
    check_that_all_parameters_are_given(parameters, v1v2c_trap_receiver_required_params,
                                        v1v2c_trap_receiver_required_params, edit.value());

    auto trap_receiver = read_v1v2c_trap_receiver(parameters);
    add_or_delete(config.trap_receivers_V1V2c_, edit.value(), trap_receiver,
                  [&](auto const& t) { return t == trap_receiver; });
  }
}

void edit_v1v2c_community(parameter_map const& parameters, wago::snmp_config_lib::SnmpConfig& config) {
  auto edit = get_v1v2c_community_edit_operation(parameters);
  if (edit) {
    check_that_all_parameters_are_given(parameters, v1v2c_community_required_params_add,
                                        v1v2c_community_required_params_delete, edit.value());

    auto community = read_v1v2c_community(parameters);
    add_or_delete(config.communities_, edit.value(), community,
                  [&](auto const& c) { return c.name_ == community.name_; });
  }
}

void edit_v3_user_or_trap(parameter_map const& parameters, wago::snmp_config_lib::SnmpConfig& config) {
  ::std::optional<edit_operation> edit;
  if ((edit = get_v3_edit_operation(parameters))) {
    auto trap_reveiver_host = get_v3_trap_receiver_host(parameters);
    if (trap_reveiver_host && !trap_reveiver_host.value().empty()) {
      edit_v3_trap(parameters, edit.value(), config);
    } else {
      edit_v3_user(parameters, edit.value(), config);
    }
  } else if ((edit = get_v3_user_edit_operation(parameters))) {
    edit_v3_user(parameters, edit.value(), config);
  } else if ((edit = get_v3_trap_receiver_edit_operation(parameters))) {
    edit_v3_trap(parameters, edit.value(), config);
  }
}

}  // namespace wago::snmp_config
