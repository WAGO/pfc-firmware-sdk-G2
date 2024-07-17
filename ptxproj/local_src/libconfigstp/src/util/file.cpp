// Copyright (c) 2024 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "file.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>

namespace wago::stp::lib {

status write(::std::stringstream &ss, ::std::string &&path) {
  ::std::string path_tmp = path + ".tmp";

  umask(0022);

  std::ofstream of_stream(path_tmp, std::ios::binary);
  if (!of_stream.good()) {
    return status{status_code::SYSTEM_CALL_ERROR, "Failed to open file " + path};
  }
  of_stream << ss.rdbuf();
  of_stream.flush();
  of_stream.close();

  sync();
  ::std::rename(path_tmp.c_str(), path.c_str());
  sync();

  return status{};
}

status read(std::stringstream &ss, ::std::string &&path) {
  status s{};
  ss.str(::std::string{});
  if (std::filesystem::exists(path)) {
    std::ifstream if_stream(path);
    if (if_stream.is_open()) {
      ss << if_stream.rdbuf();
      if_stream.close();
    } else {
      s = status{status_code::SYSTEM_CALL_ERROR, "Cannot read persistence file " + path};
    }
  } 
  return s;
}

}  // namespace wago::stp::lib