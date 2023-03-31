// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Types.hpp"

#include <optional>

namespace netconf {

enum class EventLayer { EVENT_FOLDER, IP_CHANGE_FILES };

class IEventManager {
 public:
  IEventManager()                     = default;
  virtual ~IEventManager()            = default;

  IEventManager(const IEventManager&) = delete;
  IEventManager& operator=(const IEventManager&) = delete;
  IEventManager(const IEventManager&&)           = delete;
  IEventManager& operator=(const IEventManager&&) = delete;

  virtual void NotifyNetworkChanges(EventLayer event_layer) = 0;
  virtual void NotifyNetworkChanges(EventLayer event_layer, ::std::optional<Interface>) = 0;
  virtual void ProcessPendingEvents() = 0;
};

}  // namespace netconf
