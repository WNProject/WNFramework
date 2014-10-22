// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ALLOCATOR_H__
#define __WN_ALLOCATOR_H__
#include "WNCore/inc/WNTypes.h"
#include "WNMath/inc/WNBasic.h"
namespace WNContainers {
    struct WNAllocationPair {
        void* m_pLocation;
        WN_SIZE_T count;
    };
    
    class WNAllocator {
    public:
        virtual ~WNAllocator() {}
        virtual WNAllocationPair Allocate(WN_SIZE_T _size, WN_SIZE_T _count) = 0;
        virtual WNAllocationPair Reallocate(void* _ptr, WN_SIZE_T _number, WN_SIZE_T _count, WN_SIZE_T _oldSize = 0) = 0;
        virtual WNAllocationPair AllocateForResize(WN_SIZE_T _size, WN_SIZE_T _count, WN_SIZE_T _oldSize) = 0;
        virtual void Free(void*) = 0;
    private:
    };

    template<WN_SIZE_T PERC_EXPAND, WN_SIZE_T MIN_ALLOC_SIZE = 1>
    class WNDefaultExpandingAllocator: public WNAllocator {
    public:
        
        virtual WNAllocationPair AllocateForResize(WN_SIZE_T _size, WN_SIZE_T _count, WN_SIZE_T _oldSize) {
            _count = WNMath::WNMax(_count, MIN_ALLOC_SIZE);
            WN_SIZE_T allocatedNum = WNMath::WNMax(static_cast<WN_SIZE_T>(_oldSize * (1 + (PERC_EXPAND / 100.0f))), _count);
            WNAllocationPair p = {malloc(_size * allocatedNum), allocatedNum};
            return(p);
        }
        virtual WNAllocationPair Allocate(WN_SIZE_T _size, WN_SIZE_T _count) {
            _count = WNMath::WNMax(_count, MIN_ALLOC_SIZE);
            WNAllocationPair p = {malloc(_size * _count), _count};
            return(p);
        }
        virtual WNAllocationPair Reallocate(void* _ptr, WN_SIZE_T _size, WN_SIZE_T _count, WN_SIZE_T _oldSize = 0) {
            _count = WNMath::WNMax(_count, MIN_ALLOC_SIZE);
            WN_SIZE_T allocatedNum = WNMath::WNMax(static_cast<WN_SIZE_T>(_oldSize * (1 + (PERC_EXPAND / 100.0f))), _count);
            WNAllocationPair p = {realloc(_ptr, _size * allocatedNum), _count};
            return(p);
        }
        virtual WN_VOID Free(WN_VOID* _ptr) {
            free(_ptr);
        }
    };
    typedef WNDefaultExpandingAllocator<50> WNDefaultAllocator;
}

#endif//__WN_ALLOCATOR_H__
