// SPDX-License-Identifier: GPL-2.0-or-later

#include "HandlerFactory.hpp"
#include <cstdio>
#include <exception>
#include <cstdarg>
#include <cstdio>
#include <execinfo.h>
#include <csignal>

#include "OptionParser.hpp"
#include "BridgeConfig.hpp"
#include "MessagePrinter.hpp"
#include "NetconfStatus.hpp"
#include "OutputFactory.hpp"

namespace po = boost::program_options;

using network_config::OptionParser;
using network_config::MessagePrinter;
using network_config::OutputFactory;

enum error_codes {
  OK,
  SYNTAX_ERROR,
  OPERATION_ERROR
};

template <typename... Params>
static void Print(bool beQuiet, const ::std::string& format, Params... params) {
  if (beQuiet) {
    return;
  }

  printf(format.c_str(), params...);
}

[[noreturn]]
static void segfaul_handler(int sig) {
  void *array[10];
  int size;

  // get void*'s for all entries on the stack
  size = backtrace(&array[0], 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(&array[0], size, STDERR_FILENO);
  exit(1);
}

static void install_segfault_handler() {
  signal(SIGSEGV, segfaul_handler);
}

int main(int argc, const char *argv[]) {
  OptionParser parser;
  MessagePrinter error_printer;
  int status = OK;

  install_segfault_handler();

  try {
    parser.Parse(argc, argv);
    error_printer.SetDestination(OutputFactory::Create(parser.GetVariableMap()));

    auto needHelp = parser.IsSet("help");


    if (needHelp) {
      parser.PrintHelp();
      return status;
    }

    auto handler = network_config::HandlerFactory::CreateHandler(parser);

    if (handler) {
      handler->Execute();
    }
  } catch (po::error &e) {
    status = SYNTAX_ERROR;
    error_printer.Print(
        "Your call was syntactically incorrect:" + ::std::string{e.what()} +
        "\nMaybe pay attention to the order of your options and parameters");
  } catch (::network_config::NetconfStatus &e) {
    status = OPERATION_ERROR;
    error_printer.Print(e.Get());
  } catch (std::exception& e) {
    status = OPERATION_ERROR;
    error_printer.Print("The operation failed: " + ::std::string{e.what()});
  }

  return status;
}
