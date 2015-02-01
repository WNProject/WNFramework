// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_SPIN_LOCK_H__
#define __WN_CONCURRENCY_SPIN_LOCK_H__

#include "WNCore/inc/WNTypes.h"
#include "WNConcurrency/inc/WNConfig.h"

#ifdef __WN_PTHREAD_SPIN_LOCKS
    #include <pthread.h>
#endif

namespace WNConcurrency {
    class WNSpinLock {
    public:
        WNSpinLock();
        ~WNSpinLock();

        WN_VOID Lock();
        WN_BOOL TryLock();
        WN_VOID Unlock();

    private:
        WNSpinLock(const WNSpinLock&);
        WNSpinLock& operator = (const WNSpinLock&);

    private:
        #ifdef __WN_PTHREAD_SPIN_LOCKS
            pthread_spinlock_t mSpinLock;
        #else
            WN_ATOM_T mSpinFlag;
        #endif
    };
}

#include "WNConcurrency/inc/Internal/WNSpinLock.inl"

#endif // __WN_CONCURRENCY_SPIN_LOCK_H__