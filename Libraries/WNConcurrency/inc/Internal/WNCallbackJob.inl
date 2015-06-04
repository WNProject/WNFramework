// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_INTERNAL_CALLBACK_JOB_INL__
#define __WN_CONCURRENCY_INTERNAL_CALLBACK_JOB_INL__

#ifndef __WN_CONCURRENCY_CALLBACK_JOB_H__
    #error "WNCallbackJob.inl should never be included directly. Please include WNCallbackJob.h instead"
#endif

namespace wn {
    template <typename T>
    WN_FORCE_INLINE WNCallbackJob0<T>::WNCallbackJob0(const containers::function<T()>& _callback) :
        thread_task(),
        mCallback(_callback) {
    }

    template <typename T>
    WN_FORCE_INLINE wn_void WNCallbackJob0<T>::run() {
        mResult = mCallback();
    }

    template <typename T>
    WN_FORCE_INLINE const T& WNCallbackJob0<T>::GetResult() const {
        return(mResult);
    }

    WN_FORCE_INLINE WNCallbackJob0<wn_void>::WNCallbackJob0(const containers::function<wn_void()>& _callback) :
        thread_task(),
        mCallback(_callback) {
    }

    WN_FORCE_INLINE wn_void WNCallbackJob0<wn_void>::run() {
        mCallback();
    }

    template <typename T, typename P1>
    WN_FORCE_INLINE WNCallbackJob1<T, P1>::WNCallbackJob1(const containers::function<T(P1)>& _callback, const P1& _val1) :
        thread_task(),
        mCallback(_callback),
        mP1Val(_val1) {
    }

    template <typename T, typename P1>
    WN_FORCE_INLINE wn_void WNCallbackJob1<T, P1>::run() {
        mResult = mCallback(mP1Val);
    }

    template <typename T, typename P1>
    WN_FORCE_INLINE const T& WNCallbackJob1<T, P1>::GetResult() const {
        return(mResult);
    }

    template <typename P1>
    WN_FORCE_INLINE WNCallbackJob1<wn_void, P1>::WNCallbackJob1(const containers::function<wn_void(P1)>& _callback, const P1& _val1) :
        thread_task(),
        mCallback(_callback),
        mP1Val(_val1) {
    }

    template <typename P1>
    WN_FORCE_INLINE wn_void WNCallbackJob1<wn_void, P1>::run() {
        mCallback(mP1Val);
    }

    template <typename T, typename P1, typename P2>
    WN_FORCE_INLINE WNCallbackJob2<T, P1, P2>::WNCallbackJob2(const containers::function<T(P1, P2)>& _callback, const P1& _val1, const P2& _val2) :
        thread_task(),
        mCallback(_callback),
        mP1Val(_val1),
        mP2Val(_val2) {
    }

    template <typename T, typename P1, typename P2>
    WN_FORCE_INLINE wn_void WNCallbackJob2<T, P1, P2>::run() {
        mResult = mCallback(mP1Val, mP2Val);
    }

    template <typename T, typename P1, typename P2>
    WN_FORCE_INLINE const T& WNCallbackJob2<T, P1, P2>::GetResult() const {
        return(mResult);
    }

    template <typename P1, typename P2>
    WN_FORCE_INLINE WNCallbackJob2<wn_void, P1, P2>::WNCallbackJob2(const containers::function<wn_void(P1, P2)>& _callback, const P1& _val1, const P2& _val2) :
        thread_task(),
        mCallback(_callback),
        mP1Val(_val1),
        mP2Val(_val2) {
    }

    template <typename P1, typename P2>
    WN_FORCE_INLINE wn_void WNCallbackJob2<wn_void, P1, P2>::run() {
        mCallback(mP1Val, mP2Val);
    }

    template <typename T, typename P1, typename P2, typename P3>
    WN_FORCE_INLINE WNCallbackJob3<T, P1, P2, P3>::WNCallbackJob3(const containers::function<T(P1, P2, P3)>& _callback, const P1& _val1, const P2& _val2, const P3& _val3) :
        thread_task(),
        mCallback(_callback),
        mP1Val(_val1),
        mP2Val(_val2),
        mP3Val(_val3) {
    }

    template <typename T, typename P1, typename P2, typename P3>
    WN_FORCE_INLINE wn_void WNCallbackJob3<T, P1, P2, P3>::run() {
        mResult = mCallback(mP1Val, mP2Val, mP3Val);
    }

    template <typename T, typename P1, typename P2, typename P3>
    WN_FORCE_INLINE const T& WNCallbackJob3<T, P1, P2, P3>::GetResult() const {
        return(mResult);
    }

    template <typename P1, typename P2, typename P3>
    WN_FORCE_INLINE WNCallbackJob3<wn_void, P1, P2, P3>::WNCallbackJob3(const containers::function<wn_void(P1, P2, P3)>& _callback, const P1& _val1, const P2& _val2, const P3& _val3) :
        thread_task(),
        mCallback(_callback),
        mP1Val(_val1),
        mP2Val(_val2),
        mP3Val(_val3) {
    }

    template <typename P1, typename P2, typename P3>
    WN_FORCE_INLINE wn_void WNCallbackJob3<wn_void, P1, P2, P3>::run() {
        mCallback(mP1Val, mP2Val, mP3Val);
    }

    template <typename R>
    WN_FORCE_INLINE memory::intrusive_ptr<WNCallbackJob0<R>> WNAllocateCallbackJob(R (*_ptr)()) {
        return(memory::make_intrusive<WNCallbackJob0<R>>(containers::function<R()>(_ptr)));
    }

    template <typename R, typename C>
    WN_FORCE_INLINE memory::intrusive_ptr<WNCallbackJob0<R>> WNAllocateCallbackJob(R(C::*_ptr)(), C* _this) {
        return(memory::make_intrusive<WNCallbackJob0<R>>(containers::function<R()>(_ptr, _this)));
    }

    template <typename R, typename P1>
    WN_FORCE_INLINE memory::intrusive_ptr<WNCallbackJob1<R, P1>> WNAllocateCallbackJob(R(*_ptr)(P1), P1 _param) {
        return(memory::make_intrusive<WNCallbackJob1<R, P1>>(containers::function<R(P1)>(_ptr), _param));
    }

    template <typename R, typename C, typename P1>
    WN_FORCE_INLINE memory::intrusive_ptr<WNCallbackJob1<R, P1>> WNAllocateCallbackJob(R(C::*_ptr)(P1), C*_this, P1 _param) {
        return(memory::make_intrusive<WNCallbackJob1<R, P1>>(containers::function<R(P1)>(_ptr, _this), _param));
    }

    template <typename R, typename P1, typename P2>
    WN_FORCE_INLINE memory::intrusive_ptr<WNCallbackJob2<R, P1, P2> > WNAllocateCallbackJob(R(*_ptr)(P1, P2), P1 _param, P2 _param2) {
        return(memory::make_intrusive<WNCallbackJob2<R, P1, P2> >(containers::function<R(P1, P2)>(_ptr), _param, _param2));
    }

    template <typename R, typename C, typename P1, typename P2>
    WN_FORCE_INLINE memory::intrusive_ptr<WNCallbackJob2<R, P1, P2> > WNAllocateCallbackJob(R(C::*_ptr)(P1, P2), C*_this, P1 _param, P2 _param2) {
        return(memory::make_intrusive<WNCallbackJob2<R, P1, P2> >(containers::function<R(P1, P2)>(_ptr, _this), _param, _param2));
    }

    template <typename R, typename P1, typename P2, typename P3>
    WN_FORCE_INLINE memory::intrusive_ptr<WNCallbackJob3<R, P1, P2, P3> > WNAllocateCallbackJob(R(*_ptr)(P1, P2, P3), P1 _param, P2 _param2, P3 _param3) {
        return(memory::make_intrusive<WNCallbackJob3<R, P1, P2, P3> >(containers::function<R(P1, P2, P3)>(_ptr), _param, _param2, _param3));
    }

    template <typename R, typename C, typename P1, typename P2, typename P3>
    WN_FORCE_INLINE memory::intrusive_ptr<WNCallbackJob3<R, P1, P2, P3> > WNAllocateCallbackJob(R(C::*_ptr)(P1, P2, P3), C*_this, P1 _param, P2 _param2, P3 _param3) {
        return(memory::make_intrusive<WNCallbackJob3<R, P1, P2, P3> >(containers::function<R(P1, P2, P3)>(_ptr, _this), _param, _param2, _param3));
    }
}

#endif // __WN_CONCURRENCY_INTERNAL_CALLBACK_JOB_INL__
