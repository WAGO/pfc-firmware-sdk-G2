// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "BaseTypes.hpp"
#include "IBridgeChangeEvent.hpp"

namespace netconf {

class Redundancy : public IBridgeChangeEvent{

public:
  Redundancy() = default;
  ~Redundancy() override = default;

  Redundancy(const Redundancy&) = delete;
  Redundancy& operator=(const Redundancy&) = delete;
  Redundancy(const Redundancy&&) = delete;
  Redundancy& operator=(const Redundancy&&) = delete;

  void OnBridgeRemove(const Interface& bridge) const override;
  void OnBridgeAddOrPortChange(const Interface& bridge, const Interfaces& ports) const override;

};

}