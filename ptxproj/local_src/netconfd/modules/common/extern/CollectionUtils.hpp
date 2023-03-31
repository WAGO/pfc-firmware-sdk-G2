// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "BaseTypes.hpp"

namespace netconf {

template<typename T1, typename T2>
bool IsIncluded(const T1 &element, const T2 &vector) {
  auto it = ::std::find(vector.begin(), vector.end(), element);
  return (it != vector.end());
}

template<typename T1, typename T2>
bool IsNotIncluded(const T1 &element, const T2 &vector) {
  auto it = ::std::find(vector.begin(), vector.end(), element);
  return !(it != vector.end());
}

template<typename T1, typename T2>
void RemoveByValue(const T1 &element, T2 &vector) {
  vector.erase(remove(vector.begin(), vector.end(), element), vector.end());
}

template<typename T>
void ReduceVectorByValues(T &vector_to_be_reduced, const T &values) {
  vector_to_be_reduced.erase(
      remove_if(begin(vector_to_be_reduced), end(vector_to_be_reduced), [&](auto i) {
        return find(begin(values), end(values), i) != end(values);
      }), end(vector_to_be_reduced));
}

template<typename T>
T Concatenate(const T &first, const T &secound) {
  T concatenated = first;
  concatenated.insert(concatenated.end(), secound.begin(), secound.end());
  return concatenated;
}

}
