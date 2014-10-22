// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_PREFETCH_H__
#define __WN_CORE_PREFETCH_H__

#include "WNCore/inc/WNTypes.h"

namespace WNCore {
    template <typename Type>
    WN_VOID WNPrefetch(const Type* _address);

    template <typename Type>
    WN_VOID WNPrefetch(const Type* _address, const WN_SIZE_T _offset);
}

#include "WNCore/inc/Internal/WNPrefetch.inl"

#endif // __WN_CORE_PREFETCH_H__