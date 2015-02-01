// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_INTERNAL_ATOMIC_INL__
#define __WN_CONCURRENCY_INTERNAL_ATOMIC_INL__

#ifndef __WN_CONCURRENCY_ATOMIC_H__
    #error "WNAtomic.inl should never be included directly. Please include WNAtomic.h instead"
#endif

#include "WNCore/inc/WNAssert.h"

#ifdef _WN_MSVC
    #if _WN_MSVC_MAJOR < 16
        #pragma warning (push)
        #pragma warning (disable: 4985)
    #endif

    #include <intrin.h>

    #if _WN_MSVC_MAJOR < 16
        #pragma warning (pop)
    #endif
#endif

namespace WNConcurrency {
    WN_FORCE_INLINE WN_ATOM_T WNAtomicCompareSwap(volatile WN_ATOM_T* _destination, WN_ATOM_T _swap, WN_ATOM_T _compare) {
        WN_DEBUG_ASSERT(_destination != WN_NULL);

        #ifdef _WN_MSVC
            #ifdef _WN_32_BIT
                return(static_cast<WN_ATOM_T>(_InterlockedCompareExchange(_destination, _swap, _compare)));
            #elif defined _WN_64_BIT
                return(static_cast<WN_ATOM_T>(_InterlockedCompareExchange64(_destination, _swap, _compare)));
            #endif
        #elif defined _WN_GCC || defined _WN_CLANG
            return(__sync_val_compare_and_swap(_destination, _compare, _swap));
        #endif
    }

    WN_FORCE_INLINE WN_VOID* WNAtomicCompareSwapPointer(WN_VOID* volatile* _destination, WN_VOID* _swap, WN_VOID* _compare) {
        WN_DEBUG_ASSERT(_destination != WN_NULL);
        WN_DEBUG_ASSERT(_swap != WN_NULL);
        WN_DEBUG_ASSERT(_compare != WN_NULL);

        #ifdef _WN_MSVC
            #ifdef _WN_32_BIT
                return(reinterpret_cast<WN_VOID*>(_InterlockedCompareExchange(reinterpret_cast<volatile WN_ATOM_T*>(_destination),
                                                                              reinterpret_cast<WN_ATOM_T>(_swap),
                                                                              reinterpret_cast<WN_ATOM_T>(_compare))));
            #elif defined _WN_64_BIT
                return(_InterlockedCompareExchangePointer(_destination, _swap, _compare));
            #endif
        #elif defined _WN_GCC || defined _WN_CLANG
            return(__sync_val_compare_and_swap(_destination, _compare, _swap));
        #endif
    }

    WN_FORCE_INLINE WN_BOOL WNAtomicDoubleCompareSwap(volatile WN_ATOM_T* _destination, WN_ATOM_T* _swap, WN_ATOM_T* _compare) {
        WN_DEBUG_ASSERT(_destination != WN_NULL);
        WN_DEBUG_ASSERT(_swap != WN_NULL);
        WN_DEBUG_ASSERT(_compare != WN_NULL);

        #ifdef _WN_MSVC
            #ifdef _WN_32_BIT
                const WN_INT64 originalCompare = *(WN_INT64*)_compare;
                *reinterpret_cast<WN_INT64*>(_compare) = _InterlockedCompareExchange64(reinterpret_cast<volatile WN_INT64*>(_destination),
                                                                                       *reinterpret_cast<WN_INT64*>(_swap),
                                                                                       *reinterpret_cast<WN_INT64*>(_compare));

                return(*reinterpret_cast<WN_INT64*>(_compare) == originalCompare);
            #elif defined _WN_64_BIT
                return(_InterlockedCompareExchange128(_destination, *(_swap + 1), *_swap, _compare) == 1);
            #endif
        #elif defined _WN_GCC || defined _WN_CLANG
            WN_BYTE result;

            #ifdef _WN_X86
                #ifdef _WN_32_BIT
                    __asm__ __volatile__ (
                        "xchg %%esi, %%ebx;"    // Swap ESI and EBX
                        "lock;"                 // Make cmpxchg8b atomic
                        "cmpxchg8b %0;"         // On success cmpxchg8b sets ZF
                        "setz %3;"              // If ZF set, set cas_result to 1
                        "xchg %%ebx, %%esi;"    // Re-swap ESI and EBX
                        // Output
                        : "+m" (*(volatile WN_ATOM_T (*)[2]) _destination), "+a" (*_compare), "+d" (*(_compare + 1)), "=q" (result)
                        // Input
                        : "S" (*_swap), "c" (*(_swap + 1))
                        // Memory barrier
                        : "cc", "memory"
                    );
                #elif defined _WN_64_BIT
                    __asm__ __volatile__ (
                        "xchg %%rsi, %%rbx;"    // Swap RSI and RBX
                        "lock;"                 // Make cmpxchg16b atomic
                        "cmpxchg16b %0;"        // On success cmpxchg16b sets ZF
                        "setz %3;"              // If ZF set, set cas_result to 1
                        "xchg %%rbx, %%rsi;"    // Re-swap RSI and RBX
                        // Output
                        : "+m" (*(volatile WN_ATOM_T (*)[2]) _destination), "+a" (*_compare), "+d" (*(_compare + 1)), "=q" (result)
                        // Input
                        : "S" (*_swap), "c" (*(_swap + 1))
                        // Memory barrier
                        : "cc", "memory"
                    );
                #endif
            #endif

            return(result == 1);
        #endif
    }

    WN_FORCE_INLINE WN_BOOL WNAtomicDoubleCompareSwapPointer(WN_VOID* volatile* _destination, WN_VOID* _swap, WN_VOID* _compare) {
        return(WNAtomicDoubleCompareSwap(reinterpret_cast<volatile WN_ATOM_T*>(_destination),
                                         static_cast<WN_ATOM_T*>(_swap),
                                         static_cast<WN_ATOM_T*>(_compare)));
    }

    WN_FORCE_INLINE WN_VOID* WNAtomicSwapPointer(WN_VOID* volatile* _target, WN_VOID* _value) {
        WN_DEBUG_ASSERT(_target != WN_NULL);

        #ifdef _WN_MSVC
            #ifdef _WN_32_BIT
                return(reinterpret_cast<WN_VOID*>(_InterlockedExchange(reinterpret_cast<volatile WN_ATOM_T*>(_target),
                                                                       reinterpret_cast<WN_ATOM_T>(_value))));
            #elif defined _WN_64_BIT
                return(_InterlockedExchangePointer(_target, _value));
            #endif
        #elif defined _WN_GCC || defined _WN_CLANG
            return(__sync_lock_test_and_set(_target, _value));
        #endif
    }

    WN_FORCE_INLINE WN_ATOM_T WNAtomicSwap(volatile WN_ATOM_T* _target, WN_ATOM_T _value) {
        WN_DEBUG_ASSERT(_target != WN_NULL);

        #ifdef _WN_MSVC
            #ifdef _WN_32_BIT
                return(static_cast<WN_ATOM_T>(_InterlockedExchange(_target, _value)));
            #elif defined _WN_64_BIT
                return(static_cast<WN_ATOM_T>(_InterlockedExchange64(_target, _value)));
            #endif
        #elif defined _WN_GCC || defined _WN_CLANG
            return(__sync_lock_test_and_set(_target, _value));
        #endif
    }

    WN_FORCE_INLINE WN_ATOM_T WNAtomicIncrement(volatile WN_ATOM_T* _value) {
        WN_DEBUG_ASSERT(_value != WN_NULL);

        #ifdef _WN_MSVC
            #ifdef _WN_32_BIT
                return(static_cast<WN_ATOM_T>(_InterlockedIncrement(_value)));
            #elif defined _WN_64_BIT
                return(static_cast<WN_ATOM_T>(_InterlockedIncrement64(_value)));
            #endif
        #elif defined _WN_GCC || defined _WN_CLANG
            return(static_cast<WN_ATOM_T>(__sync_add_and_fetch(_value, 1)));
        #endif
    }

    WN_FORCE_INLINE WN_ATOM_T WNAtomicDecrement(volatile WN_ATOM_T* _value) {
        WN_DEBUG_ASSERT(_value != WN_NULL);

        #ifdef _WN_MSVC
            #ifdef _WN_32_BIT
                return(static_cast<WN_ATOM_T>(_InterlockedDecrement(_value)));
            #elif defined _WN_64_BIT
                return(static_cast<WN_ATOM_T>(_InterlockedDecrement64(_value)));
            #endif
        #elif defined _WN_GCC || defined _WN_CLANG
            return(static_cast<WN_ATOM_T>(__sync_sub_and_fetch(_value, 1)));
        #endif
    }

    WN_FORCE_INLINE WN_ATOM_T WNAtomicAnd(volatile WN_ATOM_T* _value, WN_ATOM_T _mask) {
        WN_DEBUG_ASSERT(_value != WN_NULL);

        #ifdef _WN_MSVC
            #ifdef _WN_32_BIT
                return(static_cast<WN_ATOM_T>(_InterlockedAnd(_value, _mask)));
            #elif defined _WN_64_BIT
                return(static_cast<WN_ATOM_T>(_InterlockedAnd64(_value, _mask)));
            #endif
        #elif defined _WN_GCC || defined _WN_CLANG
            return(static_cast<WN_ATOM_T>(__sync_and_and_fetch(_value, _mask)));
        #endif
    }

    WN_FORCE_INLINE WN_ATOM_T WNAtomicOr(volatile WN_ATOM_T* _value, WN_ATOM_T _mask) {
        WN_DEBUG_ASSERT(_value != WN_NULL);

        #ifdef _WN_MSVC
            #ifdef _WN_32_BIT
                return(static_cast<WN_ATOM_T>(_InterlockedOr(_value, _mask)));
            #elif defined _WN_64_BIT
                return(static_cast<WN_ATOM_T>(_InterlockedOr64(_value, _mask)));
            #endif
        #elif defined _WN_GCC || defined _WN_CLANG
            return(static_cast<WN_ATOM_T>(__sync_or_and_fetch(_value, _mask)));
        #endif
    }

    WN_FORCE_INLINE WN_ATOM_T WNAtomicXOr(volatile WN_ATOM_T* _value, WN_ATOM_T _mask) {
        WN_DEBUG_ASSERT(_value != WN_NULL);

        #ifdef _WN_MSVC
            #ifdef _WN_32_BIT
                return(static_cast<WN_ATOM_T>(_InterlockedXor(_value, _mask)));
            #elif defined _WN_64_BIT
                return(static_cast<WN_ATOM_T>(_InterlockedXor64(_value, _mask)));
            #endif
        #elif defined _WN_GCC || defined _WN_CLANG
            return(static_cast<WN_ATOM_T>(__sync_xor_and_fetch(_value, _mask)));
        #endif
    }
}

#endif // __WN_CONCURRENCY_INTERNAL_ATOMIC_INL__
