// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <functional>
#include <string>
#include <utility>

namespace network_config {

template<typename DataType>
class DataField {
 public:
  using ValueGetter = ::std::function<::std::string(const DataType&)>;
  using ValueSetter = ::std::function<void(DataType&,const ::std::string&)>;

  explicit DataField(ValueGetter getter, ValueSetter setter = []([[maybe_unused]] DataType& data_type,[[maybe_unused]] const ::std::string& value) noexcept {
  }) noexcept: getter_ { std::move(getter) },
        setter_ { std::move(setter) } {
  }
  virtual ~DataField() = default;

  DataField(const DataField&) = default;
  DataField& operator=(const DataField&) = delete;
  DataField(const DataField&&) = delete;
  DataField& operator=(const DataField&&) = delete;

  ::std::string Get(const DataType &data) {
    return getter_(data);
  }

  ::std::string Set(DataType &data, ::std::string value) {
    return setter_(data, value);
  }

 private:
  ValueGetter getter_;
  ValueSetter setter_;
};

} /* namespace network_config */

