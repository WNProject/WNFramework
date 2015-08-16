// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_STRING_H__
#define __WN_CONTAINERS_STRING_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNSTLCompatibleAllocator.h"

#include <string>

namespace wn {
namespace containers {

typedef memory::passthrough_stl_allocator<wn_char, memory::default_allocator>
    wn_string_default_allocator;
typedef memory::passthrough_stl_allocator<
    wn_char, memory::default_test_allocator> wn_string_test_allocator;

typedef std::basic_string<wn_char, std::char_traits<wn_char>,
                          wn_string_default_allocator> string;
typedef std::basic_string<
    wn_char, std::char_traits<wn_char>, wn_string_test_allocator> test_string;
}  // namespace containers
}  // namespace wn

#endif  // __WN_CONTAINERS_STRING_H__