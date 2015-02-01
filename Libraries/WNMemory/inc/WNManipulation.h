// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_MANIPULATION_H__
#define __WN_MEMORY_MANIPULATION_H__

#include "WNCore/inc/WNTypes.h"

namespace WNMemory {
    wn_void* WNMemClr(wn_void* _memory, const wn_size_t _size);
    wn_void* WNMemSet(wn_void* _memory, const wn_uint8 _value, const wn_size_t _size);
    wn_void* WNMemCpy(wn_void* _destination, const wn_void* _source, const wn_size_t _number);
    wn_uint32 WNMemCmp(const wn_void* _memory1, const wn_void* _memory2, const wn_size_t _number);
    wn_void* WNMemMove(wn_void* _destination, const wn_void* _source, const wn_size_t _number);

    template <typename type>
    type* WNMemClrT(type* _memory, const wn_size_t _amount = 1);

    // MemSet
    template <typename type>
    type* WNMemSetT(type* _memory, const wn_uint8 _value, const wn_size_t _amount = 1);

    template <>
    wn_void* WNMemSetT(wn_void* _memory, wn_uint8 _value, const wn_size_t _amount);

    // MemCpy
    template <typename type>
    type* WNMemCpyT(type* _destination, const type* _source, const wn_size_t _number = 1);

    template <typename type>
    wn_uint32 WNMemCmpT(const type* _memory1, const type* _memory2, const wn_size_t _number = 1);

    template <typename type>
    type* WNMemMoveT(type* _destination, const type* _source, const wn_size_t _number = 1);
}

#include "WNMemory/inc/Internal/WNManipulation.inl"

#endif // __WN_MEMORY_MANIPULATION_H__