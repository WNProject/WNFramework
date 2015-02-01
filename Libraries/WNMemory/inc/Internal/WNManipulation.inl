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
    WN_FORCE_INLINE wn_void* WNMemClr(wn_void* _memory, const wn_size_t _size) {
        WN_DEBUG_ASSERT(_memory != wn_nullptr);
        WN_DEBUG_ASSERT(_size != 0);

        return(WNMemSet(_memory, 0, _size));
    }

    WN_FORCE_INLINE wn_void* WNMemSet(wn_void* _memory, const wn_uint8 _value, const wn_size_t _size) {
        WN_DEBUG_ASSERT(_memory != wn_nullptr);
        WN_DEBUG_ASSERT(_size != 0);

        return(internal::__WNMemSet(_memory, _value, _size));
    }

    WN_FORCE_INLINE wn_void* WNMemCpy(wn_void* _destination, const wn_void* _source, const wn_size_t _number) {
        WN_DEBUG_ASSERT(_destination != wn_nullptr);
        WN_DEBUG_ASSERT(_source != wn_nullptr);
        WN_DEBUG_ASSERT(_destination != _source);

        return(memcpy(_destination, _source, _number));
    }

    WN_FORCE_INLINE wn_uint32 WNMemCmp(const wn_void* _memory1, const wn_void* _memory2, const wn_size_t _number) {
        WN_DEBUG_ASSERT(_memory1 != wn_nullptr);
        WN_DEBUG_ASSERT(_memory2 != wn_nullptr);
        WN_DEBUG_ASSERT(_number != 0);
        WN_DEBUG_ASSERT(_memory1 != _memory2);

        return(memcmp(_memory1, _memory2, _number));
    }

    WN_FORCE_INLINE wn_void* WNMemMove(wn_void* _destination, const wn_void* _source, const wn_size_t _number) {
        WN_DEBUG_ASSERT(_destination != wn_nullptr);
        WN_DEBUG_ASSERT(_source != wn_nullptr);
        WN_DEBUG_ASSERT(_number != 0);
        WN_DEBUG_ASSERT(_destination != _source);

        return(memmove(_destination, _source, _number));
    }

    template <typename type>
    WN_FORCE_INLINE type* WNMemClrT(type* _memory, const wn_size_t _amount) {
        return(static_cast<type*>(WNMemClr(static_cast<wn_void*>(_memory), _amount * sizeof(type))));
    }

    template <typename type>
    WN_FORCE_INLINE type* WNMemSetT(type* _memory, wn_uint8 _value, const wn_size_t _amount) {
        return(static_cast<type*>(WNMemSet(static_cast<wn_void*>(_memory), _value, _amount * sizeof(type))));
    }

    template <>
    WN_FORCE_INLINE wn_void* WNMemSetT(wn_void* _memory, wn_uint8 _value, const wn_size_t _amount) {
        return(WNMemSet(_memory, _value, _amount));    
    }

    template <typename type>
    WN_FORCE_INLINE type* WNMemCpyT(type* _destination, const type* _source, const wn_size_t _number) {
        return(static_cast<type*>(WNMemCpy(static_cast<wn_void*>(_destination), static_cast<const wn_void*>(_source), _number * sizeof(type))));
    }

    template <typename type>
    WN_FORCE_INLINE wn_uint32 WNMemCmpT(const type* _memory1, const type* _memory2, const wn_size_t _number) {
        return(WNMemCmp(static_cast<wn_void*>(_memory1), static_cast<const wn_void*>(_memory2), _number * sizeof(type)));
    }

    template <typename type>
    WN_FORCE_INLINE type* WNMemMoveT(type* _destination, const type* _source, const wn_size_t _number) {
        return(static_cast<type*>(WNMemMove(static_cast<wn_void*>(_destination), static_cast<const wn_void*>(_source), _number * sizeof(type))));
    }
}

#endif // __WN_MEMORY_INTERNAL_MANIPULATION_INL__
