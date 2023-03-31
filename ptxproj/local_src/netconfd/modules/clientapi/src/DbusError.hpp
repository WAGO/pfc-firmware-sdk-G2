// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <dbus/dbus.h>

namespace netconf {

class DbusError {
 public:
  DbusError();
  ~DbusError();

  DbusError(const DbusError &other) = delete;
  DbusError(DbusError &&other) = delete;
  DbusError& operator=(const DbusError &other) = delete;
  DbusError& operator=(DbusError &&other) = delete;

  bool IsSet() const noexcept;
  const char* GetName() const noexcept;
  const char* GetMessage() const noexcept;

  DBusError* operator&() noexcept{ //NOLINT(google-runtime-operator) it is a wrapper class and we get the underlying data in a simple way
    return &err;
  }

 private:
  DBusError err = {};
};

}  // namespace netconf
