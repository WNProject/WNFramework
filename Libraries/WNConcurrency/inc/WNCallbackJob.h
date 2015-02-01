// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_CALLBACK_JOB_H__
#define __WN_CONCURRENCY_CALLBACK_JOB_H__

#include "WNConcurrency/inc/WNThreadJob.h"
#include "WNContainers/inc/WNCallback.h"
#include "WNConcurrency/inc/WNResourcePointer.h"

namespace WNConcurrency {
    template <typename T>
    class WNCallbackJob0 : public WNThreadJob {
    public:
        WNCallbackJob0(const WNContainers::WNCallback0<T>& _callback);
        virtual WN_FORCE_INLINE ~WNCallbackJob0() {}

        virtual WN_VOID Execute();

        const T& GetResult() const;

    private:
        WNContainers::WNCallback0<T> mCallback;
        T mResult;
    };

    template <>
    class WNCallbackJob0<WN_VOID> : public WNThreadJob {
    public:
        WNCallbackJob0(const WNContainers::WNCallback0<WN_VOID>& _callback);
        virtual WN_FORCE_INLINE ~WNCallbackJob0() {}

        virtual WN_VOID Execute();

    private:
        WNContainers::WNCallback0<WN_VOID> mCallback;
    };

    template <typename T, typename P1>
    class WNCallbackJob1 : public WNThreadJob {
    public:
        WNCallbackJob1(const WNContainers::WNCallback1<T, P1>& _callback, const P1& _val1);
        virtual WN_FORCE_INLINE ~WNCallbackJob1() {}

        virtual WN_VOID Execute();

        const T& GetResult() const;

    private:
        WNContainers::WNCallback1<T, P1> mCallback;
        P1 mP1Val;
        T mResult;
    };

    template <typename P1>
    class WNCallbackJob1<WN_VOID, P1> : public WNThreadJob {
    public:
        WNCallbackJob1(const WNContainers::WNCallback1<WN_VOID, P1>& _callback, const P1& _val1);
        virtual WN_FORCE_INLINE ~WNCallbackJob1() {}

        virtual WN_VOID Execute();

    private:
        WNContainers::WNCallback1<WN_VOID, P1> mCallback;
        P1 mP1Val;
    };

    template <typename T, typename P1, typename P2>
    class WNCallbackJob2 : public WNThreadJob {
    public:
        WNCallbackJob2(const WNContainers::WNCallback2<T, P1, P2>& _callback, const P1& _val1, const P2& _val2);
        virtual WN_FORCE_INLINE ~WNCallbackJob2() {}

        virtual WN_VOID Execute();

        const T& GetResult() const;

    private:
        WNContainers::WNCallback2<T, P1, P2> mCallback;
        P1 mP1Val;
        P2 mP2Val;
        T mResult;
    };

    template <typename P1, typename P2>
    class WNCallbackJob2<WN_VOID, P1, P2> : public WNThreadJob {
    public:
        WNCallbackJob2(const WNContainers::WNCallback2<WN_VOID, P1, P2>& _callback, const P1& _val1, const P2& _val2);
        virtual WN_FORCE_INLINE ~WNCallbackJob2() {}

        virtual WN_VOID Execute();

    private:
        WNContainers::WNCallback2<WN_VOID, P1, P2> mCallback;
        P1 mP1Val;
        P2 mP2Val;
    };

    template <typename T, typename P1, typename P2, typename P3>
    class WNCallbackJob3 : public WNThreadJob {
    public:
        WNCallbackJob3(const WNContainers::WNCallback3<T, P1, P2, P3>& _callback, const P1& _val1, const P2& _val2, const P3& _val3);
        virtual WN_FORCE_INLINE ~WNCallbackJob3() {}

        virtual WN_VOID Execute();

        const T& GetResult() const;

    private:
        WNContainers::WNCallback3<T, P1, P2, P3> mCallback;
        P1 mP1Val;
        P2 mP2Val;
        P3 mP3Val;
        T mResult;
    };

    template <typename P1, typename P2, typename P3>
    class WNCallbackJob3<WN_VOID, P1, P2, P3> : public WNThreadJob {
    public:
        WNCallbackJob3(const WNContainers::WNCallback3<WN_VOID, P1, P2, P3>& _callback, const P1& _val1, const P2& _val2, const P3& _val3);
        virtual WN_FORCE_INLINE ~WNCallbackJob3() {}

        virtual WN_VOID Execute();

    private:
        WNContainers::WNCallback3<WN_VOID, P1, P2, P3> mCallback;
        P1 mP1Val;
        P2 mP2Val;
        P3 mP3Val;
    };

    template <typename R>
    WNResourcePointer<WNConcurrency::WNCallbackJob0<R> > WNAllocateCallbackJob(R (*_ptr)());

    template <typename R, typename C>
    WNResourcePointer<WNConcurrency::WNCallbackJob0<R> > WNAllocateCallbackJob(R (C::*_ptr)(), C* _this);

    template <typename R, typename P1>
    WNResourcePointer<WNConcurrency::WNCallbackJob1<R, P1> > WNAllocateCallbackJob(R (*_ptr)(P1), P1 _param);

    template <typename R, typename C, typename P1>
    WNResourcePointer<WNConcurrency::WNCallbackJob1<R, P1> > WNAllocateCallbackJob(R (C::*_ptr)(P1), C*_this, P1 _param);

    template <typename R, typename P1, typename P2>
    WNResourcePointer<WNConcurrency::WNCallbackJob2<R, P1, P2> > WNAllocateCallbackJob(R (*_ptr)(P1, P2), P1 _param, P2 _param2);

    template <typename R, typename C, typename P1, typename P2>
    WNResourcePointer<WNConcurrency::WNCallbackJob2<R, P1, P2> > WNAllocateCallbackJob(R (C::*_ptr)(P1, P2), C*_this, P1 _param, P2 _param2);

    template <typename R, typename P1, typename P2, typename P3>
    WNResourcePointer<WNConcurrency::WNCallbackJob3<R, P1, P2, P3> > WNAllocateCallbackJob(R (*_ptr)(P1, P2, P3), P1 _param, P2 _param2, P3 _param3);

    template <typename R, typename C, typename P1, typename P2, typename P3>
    WNResourcePointer<WNConcurrency::WNCallbackJob3<R, P1, P2, P3> > WNAllocateCallbackJob(R (C::*_ptr)(P1, P2, P3), C*_this, P1 _param, P2 _param2, P3 _param3);
}

#define CREATE_CBJOB_TYPEDEF0(name, T) typedef WNConcurrency::WNResourcePointer<WNConcurrency::WNCallbackJob0<T> > name
#define CREATE_CBJOB_TYPEDEF1(name, T, T2) typedef WNConcurrency::WNResourcePointer<WNConcurrency::WNCallbackJob1<T, T2> > name
#define CREATE_CBJOB_TYPEDEF2(name, T, T2, T3) typedef WNConcurrency::WNResourcePointer<WNConcurrency::WNCallbackJob2<T, T2, T3> > name
#define CREATE_CBJOB_TYPEDEF3(name, T, T2, T3, T4) typedef WNConcurrency::WNResourcePointer<WNConcurrency::WNCallbackJob3<T, T2, T3, T4> > name

#include "WNConcurrency/inc/Internal/WNCallbackJob.inl"

#endif // __WN_CONCURRENCY_CALLBACK_JOB_H__