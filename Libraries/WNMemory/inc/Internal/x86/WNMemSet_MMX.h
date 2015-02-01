// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMEORY_INTERNAL_X86_MEMSET_MMX_H__
#define __WN_MEMEORY_INTERNAL_X86_MEMSET_MMX_H__

#include "WNMemory/inc/Internal/WNMemSetHelper.h"

#include <xmmintrin.h>

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4799)
#endif

namespace WNMemory {
    namespace internal {
        WN_FORCE_INLINE wn_void __WNMemSet64B(wn_uint8*& _memory, const __m64& _value, wn_size_t& _count) {
            while (_count & ~63) {
                _mm_stream_pi(reinterpret_cast<__m64*>(_memory), _value);
                _mm_stream_pi(reinterpret_cast<__m64*>(_memory + 8), _value);
                _mm_stream_pi(reinterpret_cast<__m64*>(_memory + 16), _value);
                _mm_stream_pi(reinterpret_cast<__m64*>(_memory + 24), _value);
                _mm_stream_pi(reinterpret_cast<__m64*>(_memory + 32), _value);
                _mm_stream_pi(reinterpret_cast<__m64*>(_memory + 40), _value);
                _mm_stream_pi(reinterpret_cast<__m64*>(_memory + 48), _value);
                _mm_stream_pi(reinterpret_cast<__m64*>(_memory + 56), _value);

                _memory += 64;
                _count -= 64;
            }
        }

        WN_FORCE_INLINE wn_void __WNMemSet8B(wn_uint8*& _memory, const __m64& _value, wn_size_t& _count) {
            while (_count & ~7) {
                _mm_stream_pi(reinterpret_cast<__m64*>(_memory), _value);

                _memory += 8;
                _count -= 8;
            }
        }

        WN_FORCE_INLINE wn_void* __WNMemSet(wn_void* _memory, const wn_uint8 _value, const wn_size_t _size) {
            wn_size_t count = _size;
            wn_uint8* memory = static_cast<wn_uint8*>(_memory);

            __WNMemSetHelperSetUnaligned<8>(memory, _value, count);

            if (count) {
                const __m64 value = _mm_set1_pi8(_value);

                __WNMemSet64B(memory, value, count);
                __WNMemSet8B(memory, value, count);

                _mm_empty();
                _mm_sfence();

                __WNMemSetHelperSetRemaining(memory, _value, count);
            }

            return(_memory);
        }
    }
}

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif


#endif // __WN_MEMEORY_INTERNAL_X86_MEMSET_MMX_H__