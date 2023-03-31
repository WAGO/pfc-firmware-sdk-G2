// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <ostream>
#include <fstream>
#include <memory>
#include <utility>
#include <iostream>
#include <Status.hpp>
#include "Output.hpp"

namespace network_config {

class MessagePrinter {
 public:
  MessagePrinter();
  ~MessagePrinter() = default;

  MessagePrinter(const MessagePrinter&) = delete;
  MessagePrinter& operator=(const MessagePrinter&) = delete;
  MessagePrinter(MessagePrinter&& other) = delete;
  MessagePrinter& operator=(MessagePrinter&&) = delete;

  void SetDestination(::std::unique_ptr<Output> output);
  void Print(const ::netconf::Status &status);
  void Print(const ::std::string &text);

 private:
  ::std::unique_ptr<Output> output_;

};

} /* namespace network_config */

