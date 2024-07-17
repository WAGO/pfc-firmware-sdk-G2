// Copyright (c) 2023 WAGO GmbH & Co. KG
// SPDX-License-Identifier: MPL-2.0

#include "program.hpp"

#include <fcntl.h>
#include <glib.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace configdnsmasq {

program::program(program &&other) noexcept {
  this->operator=(::std::move(other));
}

program &program::operator=(program &&other) noexcept {
  result = other.result;

  other.result = -1;

  return *this;
}

program program::execute(std::string &&cmd_line) {
  return execute(cmd_line);
}

program program::execute(const std::string &cmd_line) {
  gchar *stdout;
  gchar *stderr;
  GError *g_error;
  program p;

  if (g_spawn_command_line_sync(cmd_line.c_str(), &stdout, &stderr, &p.result, &g_error) == 0) {
    g_error_free(g_error);
  } else {
    p.stdout = stdout;
    p.stderr = stderr;
    g_free(stdout);
    g_free(stderr);
  }

  return p;
}

::std::string program::get_stdout() const {
  return stdout;
}

::std::string program::get_stderr() const {
  return stderr;
}

int program::get_result() const {
  return result;
}

}  // namespace configdnsmasq
