////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MEMEORY_INTERNAL_X86_MEMSET_SSE2_H__
#define __WN_MEMEORY_INTERNAL_X86_MEMSET_SSE2_H__

#include "WNMemory/inc/Internal/WNMemSetHelper.h"

#include <emmintrin.h>

namespace WNMemory {
    namespace __WNInternal {
        WN_FORCE_INLINE WN_VOID __WNMemSet128B(WN_UINT8*& _memory, const __m128i& _value, WN_SIZE_T& _count) {
            while (_count & ~127) {
                _mm_stream_si128(reinterpret_cast<__m128i*>(_memory), _value);
                _mm_stream_si128(reinterpret_cast<__m128i*>(_memory + 16), _value);
                _mm_stream_si128(reinterpret_cast<__m128i*>(_memory + 32), _value);
                _mm_stream_si128(reinterpret_cast<__m128i*>(_memory + 48), _value);
                _mm_stream_si128(reinterpret_cast<__m128i*>(_memory + 64), _value);
                _mm_stream_si128(reinterpret_cast<__m128i*>(_memory + 80), _value);
                _mm_stream_si128(reinterpret_cast<__m128i*>(_memory + 96), _value);
                _mm_stream_si128(reinterpret_cast<__m128i*>(_memory + 112), _value);

                _memory += 128;
                _count -= 128;
            }
        }

        WN_FORCE_INLINE WN_VOID __WNMemSet16B(WN_UINT8*& _memory, const __m128i& _value, WN_SIZE_T& _count) {
            while (_count & ~15) {
                _mm_stream_si128(reinterpret_cast<__m128i*>(_memory), _value);

                _memory += 16;
                _count -= 16;
            }
        }

        WN_FORCE_INLINE WN_VOID* __WNMemSet(WN_VOID* _memory, const WN_UINT8 _value, const WN_SIZE_T _size) {
            WN_SIZE_T count = _size;
            WN_UINT8* memory = static_cast<WN_UINT8*>(_memory);

            __WNMemSetHelperSetUnaligned<16>(memory, _value, count);

            if (count) {
                const __m128i value = _mm_set1_epi8(_value);

                __WNMemSet128B(memory, value, count);
                __WNMemSet16B(memory, value, count);

                _mm_sfence();

                __WNMemSetHelperSetRemaining(memory, _value, count);
            }

            return(_memory);
        }
    }
}

#endif // __WN_MEMEORY_INTERNAL_X86_MEMSET_SSE2_H__