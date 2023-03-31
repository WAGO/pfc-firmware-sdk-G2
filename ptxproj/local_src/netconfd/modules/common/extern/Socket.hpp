// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <sys/socket.h>
#include <unistd.h>

#include <memory>
#include <system_error>

/**
 * Wrapper of socket file descriptor that complies to RAII concept.
 */
class Socket {
 public:
  Socket() = default;
  Socket(int __domain, int __type, int __protocol) {
    int s = socket(__domain, __type, __protocol);
    if (s < 0) {
      throw ::std::system_error(errno, ::std::system_category(), "failed to open socket!");
    }
    s_ = std::shared_ptr<int>(new int{s}, [](const int* fd) {
      close(*fd);
      delete fd;  // NOLINT(cppcoreguidelines-owning-memory) Use of gsl::owner<> not possible because variable is not
                  // created as gsl::owner<>
    });
  }

  Socket(const Socket& other)            = default;
  Socket(Socket&& other)                 = default;
  Socket& operator=(const Socket& other) = default;
  Socket& operator=(Socket&& other)      = default;

  ~Socket() = default;

  [[nodiscard]] int fd() const {
    return s_ ? *s_ : -1;
  }

 private:
  ::std::shared_ptr<int> s_;
};
