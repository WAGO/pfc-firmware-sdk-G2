// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/program_options.hpp>
#include <string>

#include "IHandler.hpp"

namespace network_config {

class DCHPClientIDHandler : public IHandler {
 public:
  explicit DCHPClientIDHandler(const ::boost::program_options::variables_map &vm);
  ~DCHPClientIDHandler() override = default;

  DCHPClientIDHandler(const DCHPClientIDHandler&)            = delete;
  DCHPClientIDHandler& operator=(const DCHPClientIDHandler&) = delete;
  DCHPClientIDHandler(DCHPClientIDHandler&&) = delete;
  DCHPClientIDHandler& operator=(DCHPClientIDHandler&&) = delete;

  void Execute() override;

 private:
  ::std::function<void()> execute_;
};

} /* namespace network_config */
