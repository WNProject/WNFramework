// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_INTERNAL_ALLOCATION_INL__
#define __WN_MEMORY_INTERNAL_ALLOCATION_INL__

#ifndef __WN_MEMORY_ALLOCATION_H__
    #error "WNAllocation.inl should never be included directly. Please include WNAllocation.h instead"
#endif

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNAssert.h"

#ifndef _WN_WINDOWS
    #include "WNMemory/inc/WNManipulation.h"

    #include <stdlib.h>
#endif

namespace WNMemory {
    WN_FORCE_INLINE WN_VOID* WNMalloc(const WN_SIZE_T _size) {
        WN_DEBUG_ASSERT_DESC(_size != 0, "Attempting to allocate 0 bytes.  Please allocate something.");

        return(malloc(_size));
    }

    WN_FORCE_INLINE WN_VOID* WNCalloc(const WN_SIZE_T _count, const WN_SIZE_T _size) {
        WN_DEBUG_ASSERT_DESC(_count != 0, "Attempting to allocate 0 elements.  Please allocate something.");
        WN_DEBUG_ASSERT_DESC(_size != 0, "Attempting to allocate 0 bytes.  Please allocate something.");

        return(calloc(_count, _size));
    }

    WN_FORCE_INLINE WN_VOID* WNRealloc(WN_VOID* _memory, const WN_SIZE_T _size) {
        WN_DEBUG_ASSERT_DESC(_memory != WN_NULL, "The memory location is NULL.  Please use a valid memory location.");
        WN_DEBUG_ASSERT_DESC(_size != 0, "Attempting to allocate 0 bytes.  Please allocate something.");

        return(realloc(_memory, _size));
    }

    WN_FORCE_INLINE WN_VOID WNFree(WN_VOID* _memory) {
        WN_DEBUG_ASSERT_DESC(_memory != WN_NULL, "The memory location is NULL.  Please use a valid memory location.");

        free(_memory);
    }

    template <typename Type>
    WN_FORCE_INLINE Type* WNMallocT(const WN_SIZE_T _amount) {
        return(static_cast<Type*>(WNMalloc(_amount * sizeof(Type))));
    }

    template <typename Type>
    WN_FORCE_INLINE Type* WNCallocT(const WN_SIZE_T _count) {
        return(static_cast<Type*>(WNCalloc(sizeof(Type), _count)));
    }

    template <typename Type>
    WN_FORCE_INLINE Type* WNReallocT(Type* _memory, const WN_SIZE_T _amount) {
        return(static_cast<Type*>(WNRealloc(static_cast<WN_VOID*>(_memory), _amount * sizeof(Type))));
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNFreeT(Type* _memory) {
        WNFree(static_cast<WN_VOID*>(_memory));
    }

    WN_FORCE_INLINE WN_VOID* WNAlignedMalloc(const WN_SIZE_T _size, const WN_SIZE_T _alignment) {
        WN_DEBUG_ASSERT_DESC(_size != 0, "Attempting to allocate 0 bytes.  Please allocate something.");
        WN_DEBUG_ASSERT_DESC((_alignment != 0) && ((_alignment & (~_alignment + 1)) == _alignment),
                             "Alignment is not a power of 2.  Please use a power of 2 as the alignment.");

        WN_VOID* memory;

        #ifdef _WN_WINDOWS
            memory = _aligned_malloc(_size, _alignment);
        #elif defined _WN_POSIX && !defined _WN_ANDROID
            if (posix_memalign(&memory, _alignment, _size) != 0) {
                memory = WN_NULL;
            }
        #elif defined _WN_ANDROID
            memory = memalign(_alignment, _size);
        #else
            if (_alignment != 0 && (_alignment & (~_alignment + 1)) == _alignment) {
                WN_VOID* tempMemory = WNMalloc((_size + _alignment - 1) + sizeof(WN_VOID*));

                if (tempMemory == WN_NULL) {
                    return(WN_NULL);
                }

                memory = reinterpret_cast<WN_VOID*>(((reinterpret_cast<WN_SIZE_T>(tempMemory) + sizeof(WN_VOID*) + _alignment - 1) & ~(_alignment - 1)));

                *(reinterpret_cast<WN_VOID**>(memory) - 1) = tempMemory;
            } else {
                return(WN_NULL);
            }
        #endif

        return(memory);
    }

    WN_FORCE_INLINE WN_VOID* WNAlignedCalloc(const WN_SIZE_T _count, const WN_SIZE_T _size, const WN_SIZE_T _alignment) {
        WN_DEBUG_ASSERT_DESC(_count != 0, "Attempting to allocate 0 elements.  Please allocate something.");
        WN_DEBUG_ASSERT_DESC(_size != 0, "Attempting to allocate 0 bytes.  Please allocate something.");
        WN_DEBUG_ASSERT_DESC((_alignment != 0) && ((_alignment & (~_alignment + 1)) == _alignment),
                             "Alignment is not a power of 2.  Please use a power of 2 as the alignment.");

        return(WNAlignedMalloc(_count * _size, _alignment));
    }

    WN_FORCE_INLINE WN_VOID* WNAlignedRealloc(WN_VOID* _memory, const WN_SIZE_T _size, const WN_SIZE_T _alignment) {
        WN_DEBUG_ASSERT_DESC(_memory != WN_NULL, "The memory location is NULL.  Please use a valid memory location.");
        WN_DEBUG_ASSERT_DESC(_size != 0, "Attempting to allocate 0 bytes.  Please allocate something.");
        WN_DEBUG_ASSERT_DESC((_alignment != 0) && ((_alignment & (~_alignment + 1)) == _alignment),
                             "Alignment is not a power of 2.  Please use a power of 2 as the alignment.");

        WN_VOID* memory;

        #ifdef _WN_WINDOWS
            memory = _aligned_realloc(_memory, _size, _alignment);
        #else
            if (!((_alignment != 0) && ((_alignment & (~_alignment + 1)) == _alignment))) {
                return(WN_NULL);
            }

            if (_memory == WN_NULL) {
                return(WN_NULL);
            }

            memory = WNAlignedMalloc(_size, _alignment);

            WNMemCpy(memory, _memory, _size);
            WNAlignedFree(_memory);
        #endif

        return(memory);
    }

    WN_FORCE_INLINE WN_VOID WNAlignedFree(WN_VOID* _memory) {
        WN_DEBUG_ASSERT_DESC(_memory != WN_NULL, "The memory location is NULL.  Please use a valid memory location.");

        #ifdef _WN_WINDOWS
            _aligned_free(_memory);
        #elif defined _WN_POSIX
            free(_memory);
        #else
            WNFree(*(reinterpret_cast<WN_VOID**>(_memory) - 1));
        #endif
    }

    template <typename Type>
    WN_FORCE_INLINE Type* WNAlignedMallocT(const WN_SIZE_T _amount, const WN_SIZE_T _alignment) {
        return(static_cast<Type*>(WNAlignedMalloc(_amount * sizeof(Type), _alignment)));
    }

    template <typename Type>
    WN_FORCE_INLINE Type* WNAlignedCallocT(const WN_SIZE_T _count, const WN_SIZE_T _amount, const WN_SIZE_T _alignment) {
        return(static_cast<Type*>(WNAlignedCalloc(_count, _amount * sizeof(Type), _alignment)));
    }

    template <typename Type>
    WN_FORCE_INLINE Type* WNAlignedReallocT(Type* _memory, const WN_SIZE_T _amount, const WN_SIZE_T _alignment) {
        return(static_cast<Type*>(WNAlignedRealloc(static_cast<WN_VOID*>(_memory), _amount * sizeof(Type), _alignment)));
    }

    template <typename Type>
    WN_FORCE_INLINE WN_VOID WNAlignedFreeT(Type* _memory) {
        WNAlignedFree(static_cast<WN_VOID*>(_memory));
    }
}

#endif // __WN_MEMORY_INTERNAL_ALLOCATION_INL__
