// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <fstream>
#include <ostream>
#include <iostream>

namespace network_config {

class Output {
 public:
  Output() = default;
  virtual ~Output() = default;
  virtual ::std::ostream& GetStream() = 0;

  Output(const Output&) = delete;
  Output& operator=(const Output&) = delete;
  Output(Output&& other) = delete;
  Output& operator=(Output&&) = delete;
};

class StderrOutput : public Output {
 public:
  ::std::ostream& GetStream() override {
    return ::std::cerr;
  }
};

class FileOutput : public Output {
 public:
  explicit FileOutput(const ::std::string& filename)
      : ostream_ { filename } {
    if (!ostream_.is_open()) {
      throw ::std::ios::failure { "Failed to open: " + filename };
    }

  }
  ~FileOutput() override {
    ostream_.close();
  }

  FileOutput(const FileOutput&) = delete;
  FileOutput& operator=(const FileOutput&) = delete;
  FileOutput(FileOutput&& other) = delete;
  FileOutput& operator=(FileOutput&&) = delete;

  ::std::ostream& GetStream() override {
    return ostream_;
  }

 private:
  ::std::ofstream ostream_;

};

class NullOutput : public Output {
 public:

  NullOutput() = default;
  ~NullOutput() override = default;

  NullOutput(const NullOutput &other) = delete;
  NullOutput(NullOutput &&other) = delete;
  NullOutput& operator=(const NullOutput&) = delete;
  NullOutput& operator=(NullOutput&&) = delete;

  ::std::ostream& GetStream() override {
    return ostream_;
  }

 private:
  ::std::ofstream ostream_;
};

} /* namespace network_config */

