// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_STRING_H__
#define __WN_CONTAINERS_STRING_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNSTLCompatibleAllocator.h"
#include "WNMemory/inc/WNStringUtility.h"

#include <string>

namespace wn {
namespace containers {

typedef memory::passthrough_stl_allocator<char, memory::basic_allocator>
    string_default_allocator;

typedef std::basic_string<char, std::char_traits<char>,
    string_default_allocator>
    string;

}  // namespace containers
}  // namespace wn

namespace std {

template <>
struct hash<wn::containers::string> {
  size_t operator()(const wn::containers::string& string) const {
    return wn::memory::strhash(string.c_str());
  }
};

template <>
struct equal_to<wn::containers::string> {
  size_t operator()(const wn::containers::string& string,
      const wn::containers::string& _other) const {
    if (string.size() != _other.size()) {
      return false;
    }

    if (string.empty()) {
      return true;
    }

    return (wn::memory::strcmp(string.c_str(), _other.c_str()) == 0);
  }
};

}  // namespace std

#endif  // __WN_CONTAINERS_STRING_H__
