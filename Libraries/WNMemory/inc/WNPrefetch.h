// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_PREFETCH_H__
#define __WN_MEMORY_PREFETCH_H__

#include "WNMemory/inc/Internal/WNPrefetch.h"

namespace wn {
    namespace memory {
        template <typename _Type>
        WN_FORCE_INLINE wn_void prefetch(const _Type* _ptr) {
            internal::prefetch::execute(_ptr);
        }

        template <typename _Type>
        WN_FORCE_INLINE wn_void prefetch(const _Type* _ptr, const wn_size_t _offset) {
            prefetch(_ptr + _offset);
        }

        template <>
        WN_FORCE_INLINE wn_void prefetch(const wn_void* _ptr, const wn_size_t _offset) {
            prefetch(static_cast<const wn_byte*>(_ptr), _offset);
        }
    }
}

#endif // __WN_MEMORY_PREFETCH_H__