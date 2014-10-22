// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_INTERNAL_MANIPULATION_INL__
#define __WN_MEMORY_INTERNAL_MANIPULATION_INL__

#ifndef __WN_MEMORY_MANIPULATION_H__
    #error "WNManipulation.inl should never be included directly. Please include WNManipulation.h instead"
#endif

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNAssert.h"
#include "WNMemory/inc/Internal/WNMemSet.h"

#include <string.h>

namespace WNMemory {
    WN_FORCE_INLINE WN_VOID* WNMemClr(WN_VOID* _memory, const WN_SIZE_T _size) {
        WN_DEBUG_ASSERT(_memory != WN_NULL);
        WN_DEBUG_ASSERT(_size != 0);

        return(WNMemSet(_memory, 0, _size));
    }

    WN_FORCE_INLINE WN_VOID* WNMemSet(WN_VOID* _memory, const WN_UINT8 _value, const WN_SIZE_T _size) {
        WN_DEBUG_ASSERT(_memory != WN_NULL);
        WN_DEBUG_ASSERT(_size != 0);

        return(__WNInternal::__WNMemSet(_memory, _value, _size));
    }

    WN_FORCE_INLINE WN_VOID* WNMemCpy(WN_VOID* _destination, const WN_VOID* _source, const WN_SIZE_T _number) {
        WN_DEBUG_ASSERT(_destination != WN_NULL);
        WN_DEBUG_ASSERT(_source != WN_NULL);
        WN_DEBUG_ASSERT(_destination != _source);

        return(memcpy(_destination, _source, _number));
    }

    WN_FORCE_INLINE WN_UINT32 WNMemCmp(const WN_VOID* _memory1, const WN_VOID* _memory2, const WN_SIZE_T _number) {
        WN_DEBUG_ASSERT(_memory1 != WN_NULL);
        WN_DEBUG_ASSERT(_memory2 != WN_NULL);
        WN_DEBUG_ASSERT(_number != 0);
        WN_DEBUG_ASSERT(_memory1 != _memory2);

        return(memcmp(_memory1, _memory2, _number));
    }

    WN_FORCE_INLINE WN_VOID* WNMemMove(WN_VOID* _destination, const WN_VOID* _source, const WN_SIZE_T _number) {
        WN_DEBUG_ASSERT(_destination != WN_NULL);
        WN_DEBUG_ASSERT(_source != WN_NULL);
        WN_DEBUG_ASSERT(_number != 0);
        WN_DEBUG_ASSERT(_destination != _source);

        return(memmove(_destination, _source, _number));
    }

    template <typename Type>
    WN_FORCE_INLINE Type* WNMemClrT(Type* _memory, const WN_SIZE_T _amount) {
        return(static_cast<Type*>(WNMemClr(static_cast<WN_VOID*>(_memory), _amount * sizeof(Type))));
    }

    template <typename Type>
    WN_FORCE_INLINE Type* WNMemSetT(Type* _memory, WN_UINT8 _value, const WN_SIZE_T _amount) {
        return(static_cast<Type*>(WNMemSet(static_cast<WN_VOID*>(_memory), _value, _amount * sizeof(Type))));
    }

    template <>
    WN_FORCE_INLINE WN_VOID* WNMemSetT(WN_VOID* _memory, WN_UINT8 _value, const WN_SIZE_T _amount) {
        return(WNMemSet(_memory, _value, _amount));    
    }

    template <typename Type>
    WN_FORCE_INLINE Type* WNMemCpyT(Type* _destination, const Type* _source, const WN_SIZE_T _number) {
        return(static_cast<Type*>(WNMemCpy(static_cast<WN_VOID*>(_destination), static_cast<const WN_VOID*>(_source), _number * sizeof(Type))));
    }

    template <typename Type>
    WN_FORCE_INLINE WN_UINT32 WNMemCmpT(const Type* _memory1, const Type* _memory2, const WN_SIZE_T _number) {
        return(WNMemCmp(static_cast<WN_VOID*>(_memory1), static_cast<const WN_VOID*>(_memory2), _number * sizeof(Type)));
    }

    template <typename Type>
    WN_FORCE_INLINE Type* WNMemMoveT(Type* _destination, const Type* _source, const WN_SIZE_T _number) {
        return(static_cast<Type*>(WNMemMove(static_cast<WN_VOID*>(_destination), static_cast<const WN_VOID*>(_source), _number * sizeof(Type))));
    }
}

#endif // __WN_MEMORY_INTERNAL_MANIPULATION_INL__
