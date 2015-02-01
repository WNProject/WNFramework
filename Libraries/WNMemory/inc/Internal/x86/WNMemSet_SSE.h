// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMEORY_INTERNAL_X86_MEMSET_SSE_H__
#define __WN_MEMEORY_INTERNAL_X86_MEMSET_SSE_H__

#include "WNMemory/inc/Internal/WNMemSetHelper.h"

#include <xmmintrin.h>

namespace WNMemory {
    namespace internal {
        WN_FORCE_INLINE wn_void __WNMemSet128B(wn_uint8*& _memory, const __m128& _value, wn_size_t& _count) {
            while (_count & ~127) {
                _mm_stream_ps(reinterpret_cast<wn_float32*>(_memory), _value);
                _mm_stream_ps(reinterpret_cast<wn_float32*>(_memory + 16), _value);
                _mm_stream_ps(reinterpret_cast<wn_float32*>(_memory + 32), _value);
                _mm_stream_ps(reinterpret_cast<wn_float32*>(_memory + 48), _value);
                _mm_stream_ps(reinterpret_cast<wn_float32*>(_memory + 64), _value);
                _mm_stream_ps(reinterpret_cast<wn_float32*>(_memory + 80), _value);
                _mm_stream_ps(reinterpret_cast<wn_float32*>(_memory + 96), _value);
                _mm_stream_ps(reinterpret_cast<wn_float32*>(_memory + 112), _value);

                _memory += 128;
                _count -= 128;
            }
        }

        WN_FORCE_INLINE wn_void __WNMemSet16B(wn_uint8*& _memory, const __m128& _value, wn_size_t& _count) {
            while (_count & ~15) {
                _mm_stream_ps(reinterpret_cast<wn_float32*>(_memory), _value);

                _memory += 16;
                _count -= 16;
            }
        }

        WN_FORCE_INLINE wn_void* __WNMemSet(wn_void* _memory, const wn_uint8 _value, const wn_size_t _size) {
            wn_size_t count = _size;
            wn_uint8* memory = static_cast<wn_uint8*>(_memory);

            __WNMemSetHelperSetUnaligned<16>(memory, _value, count);

            if (count) {
                const wn_uint32 nvalue = (_value << 24) | (_value << 16) | (_value << 8) | _value;
                const __m128 value = _mm_set1_ps(*reinterpret_cast<const wn_float32*>(&nvalue));

                __WNMemSet128B(memory, value, count);
                __WNMemSet16B(memory, value, count);

                _mm_sfence();

                __WNMemSetHelperSetRemaining(memory, _value, count);
            }

            return(_memory);
        }
    }
}

#endif // __WN_MEMEORY_INTERNAL_X86_MEMSET_SSE_H__