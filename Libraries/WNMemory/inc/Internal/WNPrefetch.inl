// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_PREFETCH_INL__
#define __WN_CORE_INTERNAL_PREFETCH_INL__

#include "WNCore/inc/Internal/WNPrefetch.h"

namespace WNCore {
    template <typename Type>
    WN_FORCE_INLINE wn_void WNPrefetch(const Type* _address) {
        internal::__WNPrefetch<Type>::Execute(_address);
    }

    template <typename Type>
    WN_FORCE_INLINE wn_void WNPrefetch(const Type* _address, const wn_size_t _offset) {
        internal::__WNPrefetch<Type>::Execute(reinterpret_cast<const wn_byte*>(_address) + _offset);
    }
}

#endif // __WN_CORE_INTERNAL_PREFETCH_INL__