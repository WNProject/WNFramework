////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_CONFIG_H__
#define __WN_MATH_INTERNAL_X86_CONFIG_H__

#ifdef __WN_MMX_AVAILABLE
    #include <mmintrin.h>

    #ifndef __WN_MATH_MM_AVAILABLE
        #define __WN_MATH_MM_AVAILABLE
    #endif
#endif

#ifdef __WN_3DNOW_AVAILABLE
    #include <mm3dnow.h>

    #ifndef __WN_MATH_MM_AVAILABLE
        #define __WN_MATH_MM_AVAILABLE
    #endif
#endif

#ifdef __WN_SSE_AVAILABLE
    #include <xmmintrin.h>

    #ifndef __WN_MATH_XMM_AVAILABLE
        #define __WN_MATH_XMM_AVAILABLE
    #endif
#endif

#ifdef __WN_SSE2_AVAILABLE
    #include <emmintrin.h>

    // This is to fix a bug in the SSE2 header for MSVC
    #ifdef _WN_MSVC
        #ifdef _WN_64_BIT
            #include <intrin.h>

            #pragma intrinsic(_mm_set1_epi64x)
        #elif defined _WN_32_BIT
            WN_FORCE_INLINE __m128i _mm_set1_epi64x(__int64 i) {
                __m128i n;

                n.m128i_i64[0] = i;
                n.m128i_i64[1] = i;

                return(n);
            }
        #endif
    #endif

    #ifndef __WN_MATH_XMM_AVAILABLE
        #define __WN_MATH_XMM_AVAILABLE
    #endif
#endif

#ifdef __WN_SSE3_AVAILABLE
    #include <pmmintrin.h>

    #ifndef __WN_MATH_XMM_AVAILABLE
        #define __WN_MATH_XMM_AVAILABLE
    #endif
#endif

#ifdef __WN_SSE4_1_AVAILABLE
    #include <smmintrin.h>

    #ifndef __WN_MATH_XMM_AVAILABLE
        #define __WN_MATH_XMM_AVAILABLE
    #endif
#endif

#ifdef __WN_SSE4_2_AVAILABLE
    #include <tmmintrin.h>

    #ifndef __WN_MATH_XMM_AVAILABLE
        #define __WN_MATH_XMM_AVAILABLE
    #endif
#endif

#if defined __WN_AVX_AVAILABLE || defined __WN_AVX2_AVAILABLE
    #include <immintrin.h>

    #ifdef _WN_MSVC
        #pragma warning(disable: 4752)
    #endif

    #ifndef __WN_MATH_YMM_AVAILABLE
        #define __WN_MATH_YMM_AVAILABLE
    #endif
#endif

#ifdef __WN_MATH_MM_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNConstants_MM.h"
    #include "WNMath/inc/Internal/x86/WNUtilities_MM.h"
#endif

#ifdef __WN_MATH_XMM_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNConstants_XMM.h"
    #include "WNMath/inc/Internal/x86/WNUtilities_XMM.h"
#endif

#ifdef __WN_MATH_YMM_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNConstants_YMM.h"
    #include "WNMath/inc/Internal/x86/WNUtilities_YMM.h"
#endif

#if !defined __WN_MATH_MM_AVAILABLE && !defined __WN_MATH_XMM_AVAILABLE && !defined __WN_MATH_YMM_AVAILABLE
    #undef __WN_MATH_EXTENSIONS_ENABLED
#endif

#endif // __WN_MATH_INTERNAL_X86_CONFIG_H__