// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_INTERNAL_LOCK_GUARD_INL__
#define __WN_CONCURRENCY_INTERNAL_LOCK_GUARD_INL__

#ifndef __WN_CONCURRENCY_LOCK_GUARD_H__
    #error "WNLockGuard.inl should never be included directly. Please include WNLockGuard.h instead"
#endif

namespace WNConcurrency {
    template <typename Lock>
    WN_FORCE_INLINE WNLockGuard<Lock>::WNLockGuard(Lock& _mutex, const WN_BOOL _adoptLock) :
        mLock(_mutex) {
        if (!_adoptLock) {
            mLock.Lock();
        }
    }

    template <typename Lock>
    WN_FORCE_INLINE WNLockGuard<Lock>::~WNLockGuard() {
        mLock.Unlock();
    }
}

#endif // __WN_CONCURRENCY_INTERNAL_LOCK_GUARD_INL__