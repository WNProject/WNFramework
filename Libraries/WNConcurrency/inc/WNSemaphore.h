// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_SEMAPHORE_H__
#define __WN_CONCURRENCY_SEMAPHORE_H__

#include "WNCore/inc/WNTypes.h"

#ifdef _WN_WINDOWS
    #include <windows.h>
#elif defined _WN_POSIX
    #include <semaphore.h>
#endif

namespace WNConcurrency {
    class WNSemaphore {
    public:
        WNSemaphore(const WN_UINT32 _count = 0);
        ~WNSemaphore();

        WN_VOID Wait();
        WN_BOOL TryWait();
        WN_VOID Post(const WN_UINT32 _count = 1);

        WN_UINT32 GetCount() const;

    private:
        WNSemaphore(const WNSemaphore&);
        WNSemaphore& operator = (const WNSemaphore&);

    private:
        #ifdef _WN_WINDOWS
            HANDLE mHandle;
        #elif defined _WN_POSIX
            sem_t mSemaphore;
        #endif
    };
}

#include "WNConcurrency/inc/Internal/WNSemaphore.inl"

#endif // __WN_CONCURRENCY_SEMAPHORE_H__
