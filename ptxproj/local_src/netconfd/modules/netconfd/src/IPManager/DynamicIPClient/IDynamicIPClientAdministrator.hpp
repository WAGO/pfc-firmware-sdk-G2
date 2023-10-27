// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <map>
#include <optional>
#include "BaseTypes.hpp"
#include "IDynamicIPClient.hpp"
#include "DynamicIPType.hpp"
#include "Status.hpp"

namespace netconf {


class IDynamicIPClientAdministrator{
 public:
  IDynamicIPClientAdministrator() = default;
  virtual ~IDynamicIPClientAdministrator() = default;

  IDynamicIPClientAdministrator(const IDynamicIPClientAdministrator&) = delete;
  IDynamicIPClientAdministrator& operator=(const IDynamicIPClientAdministrator&) = delete;
  IDynamicIPClientAdministrator(IDynamicIPClientAdministrator&&) = delete;
  IDynamicIPClientAdministrator& operator=(IDynamicIPClientAdministrator&&) = delete;

  virtual IDynamicIPClientPtr AddClient(DynamicIPType type, const Interface &interface, const ::std::string &hostname, const ::std::string& client_id)  = 0;
  virtual void DeleteClient(const Interface &interface)  = 0;
  [[nodiscard]] virtual IDynamicIPClientPtr GetClient(const Interface &interface) const  = 0;

  virtual void RestartAllClients(const ::std::string& hostname) = 0;
};

} /* namespace netconf */
