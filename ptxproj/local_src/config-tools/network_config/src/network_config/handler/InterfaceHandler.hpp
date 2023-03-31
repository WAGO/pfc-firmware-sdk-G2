// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <functional>
#include <string>
#include <boost/program_options.hpp>

#include "BaseTypes.hpp"
#include "IHandler.hpp"

#include "Types.hpp"

namespace network_config {

class InterfaceHandler : public IHandler {
 public:
  explicit InterfaceHandler(const ::boost::program_options::variables_map &vm);
  ~InterfaceHandler() override = default;

  InterfaceHandler(const InterfaceHandler&) = delete;
  InterfaceHandler& operator=(const InterfaceHandler&) = delete;
  InterfaceHandler(InterfaceHandler&& other) noexcept;
  InterfaceHandler& operator=(InterfaceHandler&&) = delete;

  void Execute() override;
 private:
  void AddInterface();
  void DeleteInterface();
  netconf::Interface CreateInterface(const ::std::string &value);

  ::std::function<void()> execute_;
  const ::boost::program_options::variables_map &vm_;
};

} /* namespace network_config */
