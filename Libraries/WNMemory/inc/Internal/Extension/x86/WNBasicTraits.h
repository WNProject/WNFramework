// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_INTERNAL_EXTENSION_X86_BASIC_TRAITS_H__
#define __WN_MEMORY_INTERNAL_EXTENSION_X86_BASIC_TRAITS_H__

#ifdef _WN_X86_SSE
  #include "WNMemory/inc/Internal/Extension/x86/SSE/WNBasicTraits.h"
#elif defined _WN_X86_3DNOW
  #include "WNMemory/inc/Internal/Extension/x86/3DNow/WNBasicTraits.h"
#endif

namespace wn {
namespace memory {
namespace internal {

#ifdef _WN_X86_SSE
  typedef basic_traits_sse basic_traits_x86;
#elif defined _WN_X86_3DNOW
  typedef basic_traits_3dnow basic_traits_x86;
#endif

} // namespace internal
} // namespace memory
} // namespace wn

#endif // __WN_MEMORY_INTERNAL_EXTENSION_X86_BASIC_TRAITS_H__