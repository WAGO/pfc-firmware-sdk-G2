// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     SwitchConfigLegacy.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#pragma once

#include <string>

namespace netconf {

class SwitchConfigLegacy {
 public:
  explicit SwitchConfigLegacy(::std::string filename);
  ~SwitchConfigLegacy() = default;

  SwitchConfigLegacy(const SwitchConfigLegacy &other) = delete;
  SwitchConfigLegacy& operator=(const SwitchConfigLegacy &other) = delete;
  SwitchConfigLegacy(SwitchConfigLegacy &&other) = delete;
  SwitchConfigLegacy& operator=(SwitchConfigLegacy &&other) = delete;

  bool GetFastAgeing() const;

 private:
  ::std::string filename_;
};

}  // namespace netconf

//---- End of header file ------------------------------------------------------

