//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include "File.hpp"

#include <algorithm>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

namespace wago::util {

void WriteFile(const ::std::string &path, const ::std::string &data) {
  std::ofstream os(path + ".tmp");
  os << data << ::std::flush;
  os.close();
  std::filesystem::rename(path + ".tmp", path);

}
 
void RemoveSnmpUser(const std::string &path) {
    std::ifstream inputFile(path); 
    std::ofstream outputFile(path + ".tmp"); 
 
    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return;
    }
 
    std::string line;
 
    while (std::getline(inputFile, line)) { 
        if (line.substr(0, 7) != "usmUser") { 
            outputFile << line << std::endl;
        }
    }
 
    inputFile.close(); 
    outputFile.close(); 
 
    std::filesystem::rename(path + ".tmp", path);
}

::std::string ReadFile(const ::std::string &path) {
  std::ifstream t(path);
  return ::std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
}

}
