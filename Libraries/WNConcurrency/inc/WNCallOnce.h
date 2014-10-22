// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_CALL_ONCE_H__
#define __WN_CONCURRENCY_CALL_ONCE_H__

#include "WNCore/inc/WNTypes.h"

namespace WNConcurrency {
    class WNOnceFlag {
    public:
        WNOnceFlag();

        WN_VOID Reset();

    private:
        friend WN_VOID WNCallOnce(WNOnceFlag&, WN_VOID (*)());

        template <typename Parameter>
        friend WN_VOID WNCallOnce(WNOnceFlag&, WN_VOID (*)(Parameter), Parameter);

        template <typename Parameter>
        friend WN_VOID WNCallOnce(WNOnceFlag&, WN_VOID (*)(Parameter&), Parameter&);

    private:
        WNOnceFlag(const WNOnceFlag&);
        WNOnceFlag& operator = (const WNOnceFlag&);

    private:
        WN_ATOM_T mFlag;
    };

    WN_VOID WNCallOnce(WNOnceFlag& _flag, WN_VOID (*_function)());

    template <typename Parameter>
    WN_VOID WNCallOnce(WNOnceFlag& _flag, WN_VOID (*_function)(Parameter), Parameter _parameter);

    template <typename Parameter>
    WN_VOID WNCallOnce(WNOnceFlag& _flag, WN_VOID (*_function)(Parameter&), Parameter& _parameter);
}

#include "WNConcurrency/inc/Internal/WNCallOnce.inl"

#endif // __WN_CONCURRENCY_CALL_ONCE_H__
