// SPDX-License-Identifier: GPL-2.0-or-later
#include "Utils.hpp"

namespace network_config {

bool Contains(const boost::program_options::variables_map &map, const ::std::string &key) {
  return map.count(key) > 0;
}

bool Contains(const boost::program_options::variables_map &map, const Option &option) {
  return map.count(option.name) > 0;
}

::std::string GetValueOf(const boost::program_options::variables_map &map, const ::std::string &key) {
  auto value = map[key].as<::std::string>();
  UriEscape uri_escape;
  value = uri_escape.Unescape(value);
  boost::algorithm::trim_if(value, boost::algorithm::is_any_of("'"));
  return value;
}

::std::string GetValueOf(const boost::program_options::variables_map &map, const Option &option) {
  return GetValueOf(map, option.name);
}

boost::optional<::std::vector<::std::string>> GetValuesOf(
    const boost::program_options::variables_map &map, const ::std::string &key) {
  if (!Contains(map, key)) {
    return boost::optional<::std::vector<::std::string>> { };
  }
  auto values = map[key].as<::std::vector<::std::string>>();
  UriEscape uri_escape;
  std::transform(values.begin(), values.end(), values.begin(), [&](auto &value) {
    boost::algorithm::trim_if(value, boost::algorithm::is_any_of("'"));
    return uri_escape.Unescape(value);
  });

  return boost::optional<::std::vector<::std::string>> { values };
}

::std::string GetValueOfGet(const boost::program_options::variables_map &map) {
  return GetValueOf(map, "get");
}

::std::string GetValueOfSet(const boost::program_options::variables_map &map) {
  return GetValueOf(map, "set");
}

::std::string GetValueOfAdd(const boost::program_options::variables_map &map) {
  return GetValueOf(map, "add");
}

::std::string GetValueOfDel(const boost::program_options::variables_map &map) {
  return GetValueOf(map, "del");
}

::std::string GetFormat(const boost::program_options::variables_map &map) {
  auto value = map["format"].as<FormatOption>().value;
  boost::algorithm::trim_if(value, boost::algorithm::is_any_of("'"));
  return value;
}

::std::string GetDevice(const boost::program_options::variables_map &map) {
  return netconf::Interface::NameFromLabel(GetValueOf(map, "device"));
}

::std::string Get(const boost::program_options::variables_map &map, const Option &option) {
  return GetValueOf(map, option.name);
}



}
