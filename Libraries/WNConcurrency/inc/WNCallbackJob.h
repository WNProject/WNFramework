// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_CALLBACK_JOB_H__
#define __WN_CONCURRENCY_CALLBACK_JOB_H__

#include "WNConcurrency/inc/WNThreadTask.h"
#include "WNContainers/inc/WNFunction.h"
#include "WNMemory/inc/WNIntrusivePtr.h"

namespace wn {
    template <typename T>
    class WNCallbackJob0 : public thread_task {
    public:
        WNCallbackJob0(const containers::function<T()>& _callback);
        virtual WN_FORCE_INLINE ~WNCallbackJob0() {}

        virtual wn_void run();

        const T& GetResult() const;

    private:
        containers::function<T()> mCallback;
        T mResult;
    };

    template <>
    class WNCallbackJob0<wn_void> : public thread_task {
    public:
        WNCallbackJob0(const containers::function<wn_void()>& _callback);
        virtual WN_FORCE_INLINE ~WNCallbackJob0() {}

        virtual wn_void run();

    private:
        containers::function<wn_void()> mCallback;
    };

    template <typename T, typename P1>
    class WNCallbackJob1 : public thread_task {
    public:
        WNCallbackJob1(const containers::function<T(P1)>& _callback, const P1& _val1);
        virtual WN_FORCE_INLINE ~WNCallbackJob1() {}

        virtual wn_void run();

        const T& GetResult() const;

    private:
        containers::function<T(P1)> mCallback;
        P1 mP1Val;
        T mResult;
    };

    template <typename P1>
    class WNCallbackJob1<wn_void, P1> : public thread_task {
    public:
        WNCallbackJob1(const containers::function<wn_void(P1)>& _callback, const P1& _val1);
        virtual WN_FORCE_INLINE ~WNCallbackJob1() {}

        virtual wn_void run();

    private:
        containers::function<wn_void(P1)> mCallback;
        P1 mP1Val;
    };

    template <typename T, typename P1, typename P2>
    class WNCallbackJob2 : public thread_task {
    public:
        WNCallbackJob2(const containers::function<T(P1, P2)>& _callback, const P1& _val1, const P2& _val2);
        virtual WN_FORCE_INLINE ~WNCallbackJob2() {}

        virtual wn_void run();

        const T& GetResult() const;

    private:
        containers::function<T(P1, P2)> mCallback;
        P1 mP1Val;
        P2 mP2Val;
        T mResult;
    };

    template <typename P1, typename P2>
    class WNCallbackJob2<wn_void, P1, P2> : public thread_task {
    public:
        WNCallbackJob2(const containers::function<wn_void(P1, P2)>& _callback, const P1& _val1, const P2& _val2);
        virtual WN_FORCE_INLINE ~WNCallbackJob2() {}

        virtual wn_void run();

    private:
        containers::function<wn_void(P1, P2)> mCallback;
        P1 mP1Val;
        P2 mP2Val;
    };

    template <typename T, typename P1, typename P2, typename P3>
    class WNCallbackJob3 : public thread_task {
    public:
        WNCallbackJob3(const containers::function<T(P1, P2, P3)>& _callback, const P1& _val1, const P2& _val2, const P3& _val3);
        virtual WN_FORCE_INLINE ~WNCallbackJob3() {}

        virtual wn_void run();

        const T& GetResult() const;

    private:
        containers::function<T(P1, P2, P3)> mCallback;
        P1 mP1Val;
        P2 mP2Val;
        P3 mP3Val;
        T mResult;
    };

    template <typename P1, typename P2, typename P3>
    class WNCallbackJob3<wn_void, P1, P2, P3> : public thread_task {
    public:
        WNCallbackJob3(const containers::function<wn_void(P1, P2, P3)>& _callback, const P1& _val1, const P2& _val2, const P3& _val3);
        virtual WN_FORCE_INLINE ~WNCallbackJob3() {}

        virtual wn_void run();

    private:
        containers::function<wn_void(P1, P2, P3)> mCallback;
        P1 mP1Val;
        P2 mP2Val;
        P3 mP3Val;
    };

    template <typename R>
    memory::intrusive_ptr<WNCallbackJob0<R> > WNAllocateCallbackJob(R (*_ptr)());

    template <typename R, typename C>
    memory::intrusive_ptr<WNCallbackJob0<R> > WNAllocateCallbackJob(R(C::*_ptr)(), C* _this);

    template <typename R, typename P1>
    memory::intrusive_ptr<WNCallbackJob1<R, P1> > WNAllocateCallbackJob(R(*_ptr)(P1), P1 _param);

    template <typename R, typename C, typename P1>
    memory::intrusive_ptr<WNCallbackJob1<R, P1> > WNAllocateCallbackJob(R(C::*_ptr)(P1), C*_this, P1 _param);

    template <typename R, typename P1, typename P2>
    memory::intrusive_ptr<WNCallbackJob2<R, P1, P2> > WNAllocateCallbackJob(R(*_ptr)(P1, P2), P1 _param, P2 _param2);

    template <typename R, typename C, typename P1, typename P2>
    memory::intrusive_ptr<WNCallbackJob2<R, P1, P2> > WNAllocateCallbackJob(R(C::*_ptr)(P1, P2), C*_this, P1 _param, P2 _param2);

    template <typename R, typename P1, typename P2, typename P3>
    memory::intrusive_ptr<WNCallbackJob3<R, P1, P2, P3> > WNAllocateCallbackJob(R(*_ptr)(P1, P2, P3), P1 _param, P2 _param2, P3 _param3);

    template <typename R, typename C, typename P1, typename P2, typename P3>
    memory::intrusive_ptr<WNCallbackJob3<R, P1, P2, P3> > WNAllocateCallbackJob(R(C::*_ptr)(P1, P2, P3), C*_this, P1 _param, P2 _param2, P3 _param3);
}

#define CREATE_CBJOB_TYPEDEF0(name, T) typedef wn::memory::intrusive_ptr<wn::WNCallbackJob0<T> > name
#define CREATE_CBJOB_TYPEDEF1(name, T, T2) typedef wn::memory::intrusive_ptr<wn::WNCallbackJob1<T, T2> > name
#define CREATE_CBJOB_TYPEDEF2(name, T, T2, T3) typedef wn::memory::intrusive_ptr<wn::WNCallbackJob2<T, T2, T3> > name
#define CREATE_CBJOB_TYPEDEF3(name, T, T2, T3, T4) typedef wn::memory::intrusive_ptr<wn::WNCallbackJob3<T, T2, T3, T4> > name

#include "WNConcurrency/inc/Internal/WNCallbackJob.inl"

#endif // __WN_CONCURRENCY_CALLBACK_JOB_H__