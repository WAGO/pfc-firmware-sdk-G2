// SPDX-License-Identifier: GPL-2.0-or-later

#include "OutputFactory.hpp"
#include "OptionStrings.hpp"
#include "Utils.hpp"

namespace network_config {
  using namespace std::string_literals;

::std::unique_ptr<Output> OutputFactory::Create(const po::variables_map &vm) {
  const auto &optstr = GetOptions();
  if (Contains(vm, optstr.quiet)) {
    return ::std::make_unique<NullOutput>();
    // FIXME: remove handling of error_msg_dst when the wbm no longer uses it
  } else if (Contains(vm, optstr.error_msg_dst)
            || Contains(vm, optstr.write_last_error)) {
    return ::std::make_unique<FileOutput>("/tmp/last_error.txt"s);
  } else {
    return ::std::make_unique<StderrOutput>();
  }
}

}  // namespace network_config
