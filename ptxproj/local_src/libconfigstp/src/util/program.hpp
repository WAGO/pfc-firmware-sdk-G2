// Copyright (c) 2022 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <unistd.h>

namespace wago::stp::lib {

class program {
 public:
  program() = default;
  ~program() = default;
  program(const program&) = delete;
  program(program &&other) noexcept;
  program& operator=(const program&) = delete;
  program& operator=(program &&other) noexcept;

  ::std::string get_stdout() const;
  ::std::string get_stderr() const;
  int get_result() const;

  static program execute(::std::string &&cmd_line);
  static program execute(::std::string &cmd_line);

 private:
  int result = -1;
  ::std::string stdout;
  ::std::string stderr;
};

}  // namespace wago::std::lib
