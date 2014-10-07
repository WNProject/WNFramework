////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MEMEORY_INTERNAL_X86_MEMSET_AVX_H__
#define __WN_MEMEORY_INTERNAL_X86_MEMSET_AVX_H__

#include "WNMemory/inc/Internal/WNMemSetHelper.h"
#include "WNMemory/inc/Internal/x86/WNUtilities_YMM.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4752)
#endif

#include <immintrin.h>

namespace WNMemory {
    namespace __WNInternal {
        WN_FORCE_INLINE WN_VOID __WNMemSet256B(WN_UINT8*& _memory, const __m256i& _value, WN_SIZE_T& _count) {
            while (_count & ~255) {
                _mm256_stream_si256(reinterpret_cast<__m256i*>(_memory), _value);
                _mm256_stream_si256(reinterpret_cast<__m256i*>(_memory + 32), _value);
                _mm256_stream_si256(reinterpret_cast<__m256i*>(_memory + 64), _value);
                _mm256_stream_si256(reinterpret_cast<__m256i*>(_memory + 96), _value);
                _mm256_stream_si256(reinterpret_cast<__m256i*>(_memory + 128), _value);
                _mm256_stream_si256(reinterpret_cast<__m256i*>(_memory + 160), _value);
                _mm256_stream_si256(reinterpret_cast<__m256i*>(_memory + 192), _value);
                _mm256_stream_si256(reinterpret_cast<__m256i*>(_memory + 224),  _value);

                _memory += 256;
                _count -= 256;
            }
        }

        WN_FORCE_INLINE WN_VOID __WNMemSet32B(WN_UINT8*& _memory, const __m256i& _value, WN_SIZE_T& _count) {
            while (_count & ~31) {
                _mm256_stream_si256(reinterpret_cast<__m256i*>(_memory), _value);

                _memory += 32;
                _count -= 32;
            }
        }

        WN_FORCE_INLINE WN_VOID* __WNMemSet(WN_VOID* _memory, const WN_UINT8 _value, const WN_SIZE_T _size) {
            WN_SIZE_T count = _size;
            WN_UINT8* memory = static_cast<WN_UINT8*>(_memory);

            __WNMemSetHelperSetUnaligned<16>(memory, _value, count);

            if (count) {
                const __m128i xmmValue = _mm_set1_epi8(_value);

                if ((reinterpret_cast<WN_SIZE_T>(memory) & 15) && !(reinterpret_cast<WN_SIZE_T>(memory) & 31)) {
                    _mm_stream_si128(reinterpret_cast<__m128i*>(memory), xmmValue);

                    memory += 16;
                    count -= 16;
                }

                const __m256i ymmValue = _mm256_insertf128_si256(_mm256_castsi128_si256(xmmValue), xmmValue, 1);

                __WNMemSet256B(memory, ymmValue, count);
                __WNMemSet32B(memory, ymmValue, count);

                __WN_YMM_ZEROUPPER();

                if (count & ~15) {
                    _mm_stream_si128(reinterpret_cast<__m128i*>(memory), xmmValue);

                    memory += 16;
                    count -= 16;
                }

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


#endif // __WN_MEMEORY_INTERNAL_X86_MEMSET_AVX_H__