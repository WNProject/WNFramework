////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MEMEORY_INTERNAL_X86_MEMSET_SSE_H__
#define __WN_MEMEORY_INTERNAL_X86_MEMSET_SSE_H__

#include "WNMemory/inc/Internal/WNMemSetHelper.h"

#include <xmmintrin.h>

namespace WNMemory {
    namespace __WNInternal {
        WN_FORCE_INLINE WN_VOID __WNMemSet128B(WN_UINT8*& _memory, const __m128& _value, WN_SIZE_T& _count) {
            while (_count & ~127) {
                _mm_stream_ps(reinterpret_cast<WN_FLOAT32*>(_memory), _value);
                _mm_stream_ps(reinterpret_cast<WN_FLOAT32*>(_memory + 16), _value);
                _mm_stream_ps(reinterpret_cast<WN_FLOAT32*>(_memory + 32), _value);
                _mm_stream_ps(reinterpret_cast<WN_FLOAT32*>(_memory + 48), _value);
                _mm_stream_ps(reinterpret_cast<WN_FLOAT32*>(_memory + 64), _value);
                _mm_stream_ps(reinterpret_cast<WN_FLOAT32*>(_memory + 80), _value);
                _mm_stream_ps(reinterpret_cast<WN_FLOAT32*>(_memory + 96), _value);
                _mm_stream_ps(reinterpret_cast<WN_FLOAT32*>(_memory + 112), _value);

                _memory += 128;
                _count -= 128;
            }
        }

        WN_FORCE_INLINE WN_VOID __WNMemSet16B(WN_UINT8*& _memory, const __m128& _value, WN_SIZE_T& _count) {
            while (_count & ~15) {
                _mm_stream_ps(reinterpret_cast<WN_FLOAT32*>(_memory), _value);

                _memory += 16;
                _count -= 16;
            }
        }

        WN_FORCE_INLINE WN_VOID* __WNMemSet(WN_VOID* _memory, const WN_UINT8 _value, const WN_SIZE_T _size) {
            WN_SIZE_T count = _size;
            WN_UINT8* memory = static_cast<WN_UINT8*>(_memory);

            __WNMemSetHelperSetUnaligned<16>(memory, _value, count);

            if (count) {
                const WN_UINT32 nvalue = (_value << 24) | (_value << 16) | (_value << 8) | _value;
                const __m128 value = _mm_set1_ps(*reinterpret_cast<const WN_FLOAT32*>(&nvalue));

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