// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_RESOURCE_BASE_H__
#define __WN_CONCURRENCY_RESOURCE_BASE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNConcurrency/inc/WNAtomic.h"

namespace WNConcurrency {
    class WNResourceBase {
    public:
        WNResourceBase();

        WN_VOID AddRef();
        WN_BOOL RemoveRef();
        WN_ATOM_T GetRefs();

    private:
        WN_ATOM_T mRefCount;
    };
}

#include "WNConcurrency/inc/Internal/WNResourceBase.inl"

#endif // __WN_CONCURRENCY_RESOURCE_BASE_H__