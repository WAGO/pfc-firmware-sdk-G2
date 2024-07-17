// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "opcua-config/opcua-config.hpp"

#include <iostream>
#include <regex>
#include <vector>

#include <getopt.h>

#define XSTR(value) STR(value)
#define STR(value) #value

#define APP_VERSION XSTR(OPCUACONFIGTOOL_VERSION)

using namespace wago::opcua_config;

namespace
{

const std::string cfg_file_path = "/etc/codesys3.d/CODESYSControl_User.cfg";

const std::string opt_string_short = "hvorg::s:";
const std::vector<option> opts_long
{
  {"help", no_argument, 0, 'h'},
  {"version", no_argument, 0, 'v'},
  {"onlyvalue", no_argument, 0, 'o'},
  {"restart", no_argument, 0, 'r'},
  {"get", optional_argument, 0, 'g'},
  {"set", required_argument, 0, 's'}
};

const auto set_regex = std::regex("(.*)=\"?([^\"]*)\"?$"); // capture key and a value after = (without "")

void print_help()
{
  const std::string help_text = R"(
* Command line interface tool to change OPC UA configuration parameters *
Usage: config_opcua [OPTION]
   or: config_opcua [OPTION]... PARAMETER

Options:
  -h, --help                  Print this help.
  -v, --version               Print the version of this config tool.

  -g, --get [PARAMETER]       Get the json formatted value(s) of either one specific parameter or all, if no parameter is given.
  -o, --onlyvalue             Removes the full json output from --get and just returns the value (only applicable when getting specific parameters).
  -s, --set PARAMETER=VALUE   Set the value of a parameter.
  -r, --restart               Restart the runtime.

Known values for PARAMETER:
)";

  std::cout << help_text;
  for (const auto & param : param_keys::all_keys)
  {
    std::cout << "  " << param << std::endl;
  }
  std::cout << std::endl;
}

bool has_optional_arg(int argc, char * argv[])
{
  return optarg == nullptr && optind < argc && argv[optind][0] != '-';
}

char * get_optional_arg(int argc, char * argv[])
{
  if (has_optional_arg(argc, argv))
  {
    return argv[optind++];
  }
  return nullptr;
}

std::string unescape_arg(const std::string & arg)
{
  std::string result = arg;

  char * raw_result = g_uri_unescape_string(arg.c_str(), nullptr);
  if (raw_result != nullptr)
  {
    result = raw_result;
    g_free(raw_result);
  }

  return result;
}

} // namespace


int main(int argc, char * argv[])
{
  int result = EXIT_SUCCESS;
  int opt_index = 0;
  int opt;

  config_store store;

  bool use_raw_get_value = false;
  bool restart_runtime = false;
  std::vector<std::string> get_params;
  std::vector<std::pair<std::string, std::string>> set_params;

  while((opt = getopt_long(argc, argv, opt_string_short.c_str(), &opts_long[0], &opt_index)) != -1 && result == EXIT_SUCCESS)
  {
    switch (opt)
    {
    case 'h':
    {
      print_help();
      break;
    }
    case 'v':
    {
      std::cout << APP_VERSION << std::endl;
      break;
    }
    case 'o':
    {
      use_raw_get_value = true;
      break;
    }
    case 'r':
    {
      restart_runtime = true;
      break;
    }
    case 'g':
    {
      char * optional_arg = get_optional_arg(argc, argv);
      if (optional_arg != nullptr)
      {
        get_params.push_back(optional_arg);
      }
      else
      {
        get_params.push_back("");
      }
      break;
    }
    case 's':
    {
      std::string raw_arg = optarg;
      std::smatch matches;
      if (std::regex_search(raw_arg, matches, set_regex) && matches.size() > 2)
      {
        set_params.push_back({matches.str(1), unescape_arg(matches.str(2))});
      }
      break;
    }
    default:
      std::cerr << "Unknown option. Use --help to get more information." << std::endl;
      result = EXIT_FAILURE;
      break;
    }
  }

  if (result == EXIT_SUCCESS && (get_params.size() > 0 || set_params.size() > 0))
  {
    store.init_key_file(cfg_file_path);
  }

  for (const auto & get_param : get_params)
  {
    if (result != EXIT_SUCCESS)
    {
      break;
    }

    if (get_param != "")
    {
      std::string value_result = get_config_value_json(&store, get_param);
      if (value_result == "")
      {
        std::cerr << "Unknown parameter for option --get." << std::endl;
        result = EXIT_FAILURE;
        continue;
      }

      if (use_raw_get_value)
      {
        value_result = get_raw_config_value_from_json(value_result, get_param);
      }

      std::cout << value_result << std::endl;
    }
    else
    {
      std::cout << get_all_config_value_json(&store) << std::endl;
    }
  }

  for (const auto & set_param : set_params)
  {
    if (result != EXIT_SUCCESS)
    {
      break;
    }

    bool param_found = set_config_value(&store, set_param.first, set_param.second);
    if (!param_found)
    {
      std::cerr << "Unknown parameter for option --set." << std::endl;
      result = EXIT_FAILURE;
    }
  }

  if (result == EXIT_SUCCESS && set_params.size() > 0)
  {
    store.save_config_to_file(cfg_file_path);
  }

  if (result == EXIT_SUCCESS && restart_runtime && !trigger_runtime_restart())
  {
    result = EXIT_FAILURE;
  }

  return result;
}


