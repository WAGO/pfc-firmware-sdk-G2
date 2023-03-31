// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>

class UriEscape {
 public:
  explicit UriEscape(::std::string escape_chars = "");
  ~UriEscape() = default;

  UriEscape(const UriEscape&) = delete;
  UriEscape& operator=(const UriEscape&) = delete;
  UriEscape(UriEscape&& other) = delete;
  UriEscape& operator=(UriEscape&&) = delete;

  ::std::string Unescape(const ::std::string &escaped_uri) const;
  ::std::string Unescape(const char *escaped_uri) const;

  ::std::string Escape(const ::std::string &escaped_uri) const;
  ::std::string Escape(const char *escaped_uri) const;

 private:
  ::std::string escape_chars_;

};
