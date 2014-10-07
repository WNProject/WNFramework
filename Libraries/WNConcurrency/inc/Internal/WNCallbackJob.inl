////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_INTERNAL_CALLBACK_JOB_INL__
#define __WN_CONCURRENCY_INTERNAL_CALLBACK_JOB_INL__

#ifndef __WN_CONCURRENCY_CALLBACK_JOB_H__
    #error "WNCallbackJob.inl should never be included directly. Please include WNCallbackJob.h instead"
#endif

namespace WNConcurrency {
    template <typename T>
    WN_FORCE_INLINE WNCallbackJob0<T>::WNCallbackJob0(const WNContainers::WNCallback0<T>& _callback) :
        WNThreadJob(),
        mCallback(_callback) {
    }

    template <typename T>
    WN_FORCE_INLINE WN_VOID WNCallbackJob0<T>::Execute() {
        mResult = mCallback.Execute();
        MarkCompleted();
    }

    template <typename T>
    WN_FORCE_INLINE const T& WNCallbackJob0<T>::GetResult() const {
        return(mResult);
    }

    WN_FORCE_INLINE WNCallbackJob0<WN_VOID>::WNCallbackJob0(const WNContainers::WNCallback0<WN_VOID>& _callback) :
        WNThreadJob(),
        mCallback(_callback) {
    }

    WN_FORCE_INLINE WN_VOID WNCallbackJob0<WN_VOID>::Execute() {
        mCallback.Execute();
        MarkCompleted();
    }

    template <typename T, typename P1>
    WN_FORCE_INLINE WNCallbackJob1<T, P1>::WNCallbackJob1(const WNContainers::WNCallback1<T, P1>& _callback, const P1& _val1) :
        WNThreadJob(),
        mCallback(_callback),
        mP1Val(_val1) {
    }

    template <typename T, typename P1>
    WN_FORCE_INLINE WN_VOID WNCallbackJob1<T, P1>::Execute() {
        mResult = mCallback.Execute(mP1Val);
        MarkCompleted();
    }

    template <typename T, typename P1>
    WN_FORCE_INLINE const T& WNCallbackJob1<T, P1>::GetResult() const {
        return(mResult);
    }

    template <typename P1>
    WN_FORCE_INLINE WNCallbackJob1<WN_VOID, P1>::WNCallbackJob1(const WNContainers::WNCallback1<WN_VOID, P1>& _callback, const P1& _val1) :
        WNThreadJob(),
        mCallback(_callback),
        mP1Val(_val1) {
    }

    template <typename P1>
    WN_FORCE_INLINE WN_VOID WNCallbackJob1<WN_VOID, P1>::Execute() {
        mCallback.Execute(mP1Val);
        MarkCompleted();
    }

    template <typename T, typename P1, typename P2>
    WN_FORCE_INLINE WNCallbackJob2<T, P1, P2>::WNCallbackJob2(const WNContainers::WNCallback2<T, P1, P2>& _callback, const P1& _val1, const P2& _val2) :
        WNThreadJob(),
        mCallback(_callback),
        mP1Val(_val1),
        mP2Val(_val2) {
    }

    template <typename T, typename P1, typename P2>
    WN_FORCE_INLINE WN_VOID WNCallbackJob2<T, P1, P2>::Execute() {
        mResult = mCallback.Execute(mP1Val, mP2Val);
        MarkCompleted();
    }

    template <typename T, typename P1, typename P2>
    WN_FORCE_INLINE const T& WNCallbackJob2<T, P1, P2>::GetResult() const {
        return(mResult);
    }

    template <typename P1, typename P2>
    WN_FORCE_INLINE WNCallbackJob2<WN_VOID, P1, P2>::WNCallbackJob2(const WNContainers::WNCallback2<WN_VOID, P1, P2>& _callback, const P1& _val1, const P2& _val2) :
        WNThreadJob(),
        mCallback(_callback),
        mP1Val(_val1),
        mP2Val(_val2) {
    }

    template <typename P1, typename P2>
    WN_FORCE_INLINE WN_VOID WNCallbackJob2<WN_VOID, P1, P2>::Execute() {
        mCallback.Execute(mP1Val, mP2Val);
        MarkCompleted();
    }

    template <typename T, typename P1, typename P2, typename P3>
    WN_FORCE_INLINE WNCallbackJob3<T, P1, P2, P3>::WNCallbackJob3(const WNContainers::WNCallback3<T, P1, P2, P3>& _callback, const P1& _val1, const P2& _val2, const P3& _val3):
        WNThreadJob(),
        mCallback(_callback),
        mP1Val(_val1),
        mP2Val(_val2),
        mP3Val(_val3) {
    }

    template <typename T, typename P1, typename P2, typename P3>
    WN_FORCE_INLINE WN_VOID WNCallbackJob3<T, P1, P2, P3>::Execute() {
        mResult = mCallback.Execute(mP1Val, mP2Val);
        MarkCompleted();
    }

    template <typename T, typename P1, typename P2, typename P3>
    WN_FORCE_INLINE const T& WNCallbackJob3<T, P1, P2, P3>::GetResult() const {
        return(mResult);
    }

    template <typename P1, typename P2, typename P3>
    WN_FORCE_INLINE WNCallbackJob3<WN_VOID, P1, P2, P3>::WNCallbackJob3(const WNContainers::WNCallback3<WN_VOID, P1, P2, P3>& _callback, const P1& _val1, const P2& _val2, const P3& _val3):
        WNThreadJob(),
        mCallback(_callback),
        mP1Val(_val1),
        mP2Val(_val2),
        mP3Val(_val3) {
    }

    template <typename P1, typename P2, typename P3>
    WN_FORCE_INLINE WN_VOID WNCallbackJob3<WN_VOID, P1, P2, P3>::Execute() {
        mCallback.Execute(mP1Val, mP2Val, mP3Val);
        MarkCompleted();
    }

    template <typename R>
    WN_FORCE_INLINE WNResourcePointer<WNConcurrency::WNCallbackJob0<R> > WNAllocateCallbackJob(R (*_ptr)()) {
        return(WNConcurrency::WNAllocateResource<WNConcurrency::WNCallbackJob0<R> >(WNContainers::WNCallback0<R>(_ptr)));
    }

    template <typename R, typename C>
    WN_FORCE_INLINE WNResourcePointer<WNConcurrency::WNCallbackJob0<R> > WNAllocateCallbackJob(R (C::*_ptr)(), C* _this) {
        return(WNConcurrency::WNAllocateResource<WNConcurrency::WNCallbackJob0<R> >(WNContainers::WNCallback0<R>(_this, _ptr)));
    }

    template <typename R, typename P1>
    WN_FORCE_INLINE WNResourcePointer<WNConcurrency::WNCallbackJob1<R, P1> > WNAllocateCallbackJob(R (*_ptr)(P1), P1 _param) {
        return(WNConcurrency::WNAllocateResource<WNConcurrency::WNCallbackJob1<R, P1> >(WNContainers::WNCallback1<R, P1>(_ptr), _param));
    }

    template <typename R, typename C, typename P1>
    WN_FORCE_INLINE WNResourcePointer<WNConcurrency::WNCallbackJob1<R, P1> > WNAllocateCallbackJob(R (C::*_ptr)(P1), C*_this, P1 _param) {
        return(WNConcurrency::WNAllocateResource<WNConcurrency::WNCallbackJob1<R, P1> >(WNContainers::WNCallback1<R, P1>(_this, _ptr), _param));
    }

    template <typename R, typename P1, typename P2>
    WN_FORCE_INLINE WNResourcePointer<WNConcurrency::WNCallbackJob2<R, P1, P2> > WNAllocateCallbackJob(R (*_ptr)(P1, P2), P1 _param, P2 _param2) {
        return(WNConcurrency::WNAllocateResource<WNConcurrency::WNCallbackJob2<R, P1, P2> >(WNContainers::WNCallback2<R, P1, P2>(_ptr), _param, _param2));
    }

    template <typename R, typename C, typename P1, typename P2>
    WN_FORCE_INLINE WNResourcePointer<WNConcurrency::WNCallbackJob2<R, P1, P2> > WNAllocateCallbackJob(R (C::*_ptr)(P1, P2), C*_this, P1 _param, P2 _param2) {
        return(WNConcurrency::WNAllocateResource<WNConcurrency::WNCallbackJob2<R, P1, P2> >(WNContainers::WNCallback2<R, P1, P2>(_this, _ptr), _param, _param2));
    }

    template <typename R, typename P1, typename P2, typename P3>
    WN_FORCE_INLINE WNResourcePointer<WNConcurrency::WNCallbackJob3<R, P1, P2, P3> > WNAllocateCallbackJob(R (*_ptr)(P1, P2, P3), P1 _param, P2 _param2, P3 _param3) {
        return(WNConcurrency::WNAllocateResource<WNConcurrency::WNCallbackJob3<R, P1, P2, P3> >(WNContainers::WNCallback3<R, P1, P2, P3>(_ptr), _param, _param2, _param3));
    }

    template <typename R, typename C, typename P1, typename P2, typename P3>
    WN_FORCE_INLINE WNResourcePointer<WNConcurrency::WNCallbackJob3<R, P1, P2, P3> > WNAllocateCallbackJob(R (C::*_ptr)(P1, P2, P3), C*_this, P1 _param, P2 _param2, P3 _param3) {
        return(WNConcurrency::WNAllocateResource<WNConcurrency::WNCallbackJob3<R, P1, P2, P3> >(WNContainers::WNCallback3<R, P1, P2, P3>(_this, _ptr), _param, _param2, _param3));
    }
}

#endif // __WN_CONCURRENCY_INTERNAL_CALLBACK_JOB_INL__
