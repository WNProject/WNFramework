// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_INTERNAL_BASIC_TRAITS_H__
#define __WN_MEMORY_INTERNAL_BASIC_TRAITS_H__

#include "WNMemory/inc/Internal/WNConfig.h"

#ifdef _WN_MEMORY_USE_EXTENSION
#include "WNMemory/inc/Internal/Extension/WNBasicTraits.h"
#endif

#if !defined _WN_MEMORY_USE_EXTENSION || defined _WN_MEMORY_NO_EXTENSION
#include "WNMemory/inc/Internal/Generic/WNBasicTraits.h"
#endif

namespace wn {
namespace memory {
namespace internal {

#if defined _WN_MEMORY_USE_EXTENSION && !defined _WN_MEMORY_NO_EXTENSION
typedef basic_traits_extension basic_traits;
#else
typedef basic_traits_generic basic_traits;
#endif

}  // namespace internal
}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_INTERNAL_BASIC_TRAITS_H__