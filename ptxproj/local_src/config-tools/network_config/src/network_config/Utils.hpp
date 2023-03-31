// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "UriEscape.hpp"

#include <string>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/optional/optional.hpp>
#include <initializer_list>
#include <array>
#include "OptionParser.hpp"
#include "OptionStrings.hpp"
#include "BaseTypes.hpp"

namespace network_config {

bool Contains(const boost::program_options::variables_map &map, const ::std::string &key);
bool Contains(const boost::program_options::variables_map &map, const Option &option);
::std::string GetValueOf(const boost::program_options::variables_map &map, const ::std::string &key);
::std::string GetValueOf(const boost::program_options::variables_map &map, const Option &option);
boost::optional<::std::vector<::std::string>> GetValuesOf(
    const boost::program_options::variables_map &map, const ::std::string &key);
::std::string GetValueOfGet(const boost::program_options::variables_map &map);
::std::string GetValueOfSet(const boost::program_options::variables_map &map);
::std::string GetValueOfAdd(const boost::program_options::variables_map &map);
::std::string GetValueOfDel(const boost::program_options::variables_map &map);
::std::string GetFormat(const boost::program_options::variables_map &map);
::std::string GetDevice(const boost::program_options::variables_map &map);
::std::string Get(const boost::program_options::variables_map &map, const Option &option);

template<typename ... Options>
::std::string JoinOptionNames(std::string separator, Options &&... options) {
  ::std::array<::std::string, sizeof...(options)> options_a { { (options.name)... } };
  return boost::algorithm::join(options_a, separator);
}

template<typename ... Args>
void OptionalAndMutuallyExclusive(const boost::program_options::variables_map &map, Args &&... args) {
  auto option_count = (map.count(args.name) + ...);

  if (option_count > 1) {
    throw boost::program_options::error(
        ::std::string("Optional arguments are mutually exclusive: " + JoinOptionNames(",", args...)));
  }
}

template<typename ... Args>
void MutuallyExclusiveAndOnlyOnce(const boost::program_options::variables_map &map, Args &&... args) {
  auto option_count = (map.count(args.name) + ... );

  if (option_count != 1) {
    throw boost::program_options::error(
        ::std::string("Either options are set that are mutually exclusive, or an option is set multiple times."));
  }
}

template<typename ... Args>
void ExpectOptionPair(const boost::program_options::variables_map &map, Option option, Args &&... suboptions) {
  if (Contains(map, option)) {
    MutuallyExclusiveAndOnlyOnce(map, suboptions...);
  }
}

}
