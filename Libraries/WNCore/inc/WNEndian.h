// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_ENDIAN_H__
#define __WN_CORE_ENDIAN_H__

#include "WNCore/inc/WNTypes.h"

namespace WNCore {
    template <typename Type>
    Type& WNToBigEndian(Type& _value);

    template <typename Type>
    Type& WNFromBigEndian(Type& _value);

    template <typename Type>
    Type& WNToLittleEndian(Type& _value);

    template <typename Type>
    Type& WNFromLittleEndian(Type& _value);
}

#include "WNCore/inc/Internal/WNEndian.inl"

#endif // __WN_CORE_ENDIAN_H__
