// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_INTERNAL_EXTENSION_BASIC_TRAITS_H__
#define __WN_MEMORY_INTERNAL_EXTENSION_BASIC_TRAITS_H__

#ifdef _WN_X86
  #include "WNMemory/inc/Internal/Extension/x86/WNBasicTraits.h"
#else
  #define _WN_MEMORY_NO_EXTENSION
#endif

namespace wn {
namespace memory {
namespace internal {

#ifdef _WN_X86
  typedef basic_traits_x86 basic_traits_extension;
#endif

} // namespace internal
} // namespace memory
} // namespace wn


#endif // __WN_MEMORY_INTERNAL_EXTENSION_BASIC_TRAITS_H__