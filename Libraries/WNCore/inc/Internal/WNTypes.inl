// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_INTERNAL_TYPES_INL__
#define __WN_CORE_INTERNAL_TYPES_INL__

#ifndef __WN_CORE_TYPES_H__
    #error "WNTypes.inl should never be included directly. Please include WNTypes.h instead"
#endif

#include "WNCore/inc/WNMacros.h"

template <typename Type>
WN_FORCE_INLINE WN_BOOL WNIsValid(Type _value) {
    WN_UNUSED_ARG(_value);

    return(WN_TRUE);
}

template <typename Type>
WN_FORCE_INLINE WN_BOOL WNIsValid(Type* _value) {
    return(_value != WN_NULL);
}

template <>
WN_FORCE_INLINE WN_BOOL WNIsValid(WN_UINT8 _value) {
    return(_value != static_cast<WN_UINT8>(-1));
}

template <>
WN_FORCE_INLINE WN_BOOL WNIsValid(WN_UINT16 _value) {
    return(_value != static_cast<WN_UINT16>(-1));
}

template <>
WN_FORCE_INLINE WN_BOOL WNIsValid(WN_UINT32 _value) {
    return(_value != static_cast<WN_UINT32>(-1));
}

template <>
WN_FORCE_INLINE WN_BOOL WNIsValid(WN_UINT64 _value) {
    return(_value != static_cast<WN_UINT64>(-1));
}

template <>
WN_FORCE_INLINE WN_BOOL WNIsValid(WN_FLOAT32 _value) {
    return(_value == _value);
}

template <>
WN_FORCE_INLINE WN_BOOL WNIsValid(WN_FLOAT64 _value) {
    return(_value == _value);
}

#endif // __WN_CORE_INTERNAL_TYPES_INL__
