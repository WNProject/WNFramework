// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_ALLOCATOR_H__
#define __WN_MEMORY_ALLOCATOR_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMemory/inc/WNAllocation.h"

namespace wn {
    struct WNAllocationPair {
        void* m_pLocation;
        wn_size_t count;
    };
    
    struct WNAllocator {
        virtual ~WNAllocator() {}
        virtual WNAllocationPair Allocate(wn_size_t _size, wn_size_t _count) = 0;
        virtual WNAllocationPair Reallocate(void* _ptr, wn_size_t _number, wn_size_t _count, wn_size_t _oldSize = 0) = 0;
        virtual WNAllocationPair AllocateForResize(wn_size_t _size, wn_size_t _count, wn_size_t _oldSize) = 0;
        virtual void Free(void*) = 0;
    };

    template<wn_size_t PERC_EXPAND, wn_size_t MIN_ALLOC_SIZE = 1>
    struct default_expanding_allocator : WNAllocator {
        virtual WNAllocationPair AllocateForResize(wn_size_t _size, wn_size_t _count, wn_size_t _oldSize) {
            _count = wn::max(_count, MIN_ALLOC_SIZE);
            wn_size_t allocatedNum = wn::max(static_cast<wn_size_t>(_oldSize * (1 + (PERC_EXPAND / 100.0f))), _count);
            WNAllocationPair p = {malloc(_size * allocatedNum), allocatedNum};
            return(p);
        }

        virtual WNAllocationPair Allocate(wn_size_t _size, wn_size_t _count) {
            _count = wn::max(_count, MIN_ALLOC_SIZE);
            WNAllocationPair p = {malloc(_size * _count), _count};
            return(p);
        }

        virtual WNAllocationPair Reallocate(void* _ptr, wn_size_t _size, wn_size_t _count, wn_size_t _oldSize = 0) {
            _count = wn::max(_count, MIN_ALLOC_SIZE);
            wn_size_t allocatedNum = wn::max(static_cast<wn_size_t>(_oldSize * (1 + (PERC_EXPAND / 100.0f))), _count);
            WNAllocationPair p = {realloc(_ptr, _size * allocatedNum), _count};
            return(p);
        }

        virtual wn_void Free(wn_void* _ptr) {
            free(_ptr);
        }
    };

    typedef default_expanding_allocator<50> default_allocator;
}

#endif // __WN_MEMORY_ALLOCATOR_H__
