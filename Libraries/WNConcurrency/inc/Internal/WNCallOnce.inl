// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_INTERNAL_CALL_ONCE_INL__
#define __WN_CONCURRENCY_INTERNAL_CALL_ONCE_INL__

#ifndef __WN_CONCURRENCY_CALL_ONCE_H__
    #error "WNCallOnce.inl should never be included directly. Please include WNCallOnce.h instead"
#endif

#include "WNCore/inc/WNAssert.h"
#include "WNConcurrency/inc/WNAtomic.h"
#include "WNConcurrency/inc/WNThread.h"

#define __WN_ONCE_FLAG_NOT_RUN 0
#define __WN_ONCE_FLAG_RUNNING 1
#define __WN_ONCE_FLAG_COMPLETE 2

#define __WN_CALL_ONCE_ERR_MSG "Error: Invalid state detected when using call once routine."

namespace WNConcurrency {
    namespace __WNInternal {
        namespace __WNCallOnceState {
            enum Type {
                eWNNotRun = 0,
                eWNRunning = 1,
                eWNComplete = 2
            };
        }

        WN_FORCE_INLINE WN_BOOL __WNCallOnceHelper(WN_ATOM_T* _flag) {
            for (;;) {
                const WN_ATOM_T previous = WNAtomicCompareSwap(_flag, __WNCallOnceState::eWNRunning, __WNCallOnceState::eWNNotRun);

                if (previous == __WN_ONCE_FLAG_COMPLETE) {
                    return(WN_TRUE);
                } else if (previous == __WNCallOnceState::eWNNotRun) {
                    break;
                } else {
                    WN_RELEASE_ASSERT_DESC(previous == __WNCallOnceState::eWNRunning, __WN_CALL_ONCE_ERR_MSG);

                    WNThreadYield();
                }
            }

            return(WN_FALSE);
        }
    }

    WN_FORCE_INLINE WNOnceFlag::WNOnceFlag() :
        mFlag(__WNInternal::__WNCallOnceState::eWNNotRun) {
    }

    WN_FORCE_INLINE WN_VOID WNOnceFlag::Reset() {
        WNAtomicSwap(&mFlag, __WNInternal::__WNCallOnceState::eWNNotRun);
    }

    WN_FORCE_INLINE WN_VOID WNCallOnce(WNOnceFlag& _flag, WN_VOID (*_function)()) {
        if (__WNInternal::__WNCallOnceHelper(&_flag.mFlag)) {
            return;
        }

        _function();

        WNAtomicSwap(&_flag.mFlag, __WNInternal::__WNCallOnceState::eWNComplete);
    }

    template <typename Parameter>
    WN_FORCE_INLINE WN_VOID WNCallOnce(WNOnceFlag& _flag, WN_VOID (*_function)(Parameter), Parameter _parameter) {
        if (__WNInternal::__WNCallOnceHelper(&_flag.mFlag)) {
            return;
        }

        _function(_parameter);

        WNAtomicSwap(&_flag.mFlag, __WNInternal::__WNCallOnceState::eWNComplete);
    }

    template <typename Parameter>
    WN_FORCE_INLINE WN_VOID WNCallOnce(WNOnceFlag& _flag, WN_VOID (*_function)(Parameter&), Parameter& _parameter) {
        if (__WNInternal::__WNCallOnceHelper(&_flag.mFlag)) {
            return;
        }

        _function(_parameter);

        WNAtomicSwap(&_flag.mFlag, __WNInternal::__WNCallOnceState::eWNComplete);
    }
}

#endif // __WN_CONCURRENCY_INTERNAL_CALL_ONCE_INL__