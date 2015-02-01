// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_X86_PREFETCH_SSE_H__
#define __WN_CORE_INTERNAL_X86_PREFETCH_SSE_H__

#include <xmmintrin.h>

namespace WNCore {
    namespace internal {
        template <typename Type>
        class __WNPrefetch<Type, typename std::enable_if<(!((sizeof(Type) == 4 || sizeof(Type) == 8) && std::is_floating_point<Type>::value))>::type> {
        public:
            static WN_FORCE_INLINE wn_void Execute(const Type* _address) {
                _mm_prefetch(reinterpret_cast<const char*>(_address), _MM_HINT_T0);
            }

        private:
            WN_FORCE_INLINE __WNPrefetch() {}
        };

        template <typename Type>
        class __WNPrefetch<Type, typename std::enable_if<((sizeof(Type) == 4 || sizeof(Type) == 8) && std::is_floating_point<Type>::value)>::type> {
        public:
            static WN_FORCE_INLINE wn_void Execute(const Type* _address) {
                _mm_prefetch(reinterpret_cast<const char*>(_address), _MM_HINT_T1);
            }

        private:
            WN_FORCE_INLINE __WNPrefetch() {}
        };
    }
}

#endif // __WN_CORE_INTERNAL_X86_PREFETCH_SSE_H__