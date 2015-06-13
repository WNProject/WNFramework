// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_UNIQUE_PTR_H__
#define __WN_MEMORY_UNIQUE_PTR_H__

#include "WNMemory/inc/WNBasic.h"

namespace wn {
namespace memory {

template <typename T, typename Deleter = default_destroyer<T>>
using unique_ptr = std::unique_ptr<T, Deleter>;

template <typename T, typename... Args>
WN_FORCE_INLINE unique_ptr<T> make_unique(Args&&... args) {
  return(unique_ptr<T>(construct<T>(std::forward<Args>(args)...)));
}

} // namespace memory
} // namespace wn

#endif // __WN_MEMORY_UNIQUE_PTR_H__