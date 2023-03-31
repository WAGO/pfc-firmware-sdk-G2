// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include "Status.hpp"
namespace netconf {

template<class DataType>
class IPersistence {
 public:
  IPersistence() = default;
  virtual ~IPersistence() = default;

  IPersistence(const IPersistence &other) = delete;
  IPersistence& operator=(const IPersistence &other) = delete;
  IPersistence(IPersistence &&other) = delete;
  IPersistence& operator=(IPersistence &&other) = delete;

  virtual Status Write(const DataType& d) = 0;
  virtual Status Read(DataType& d) = 0;
};

}  // namespace netconf
