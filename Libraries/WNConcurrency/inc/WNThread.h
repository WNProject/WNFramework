// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_THREAD_H__
#define __WN_CONCURRENCY_THREAD_H__

#include "WNConcurrency/inc/Internal/WNThreadBase.h"

#ifdef _WN_WINDOWS
    #define WN_THREAD_INVALID_NATIVE_HANDLE INVALID_HANDLE_VALUE
#elif defined _WN_POSIX
    #define WN_THREAD_INVALID_NATIVE_HANDLE static_cast<pthread_t>(-1)
#endif

namespace WNConcurrency {
    #ifdef _WN_WINDOWS
        typedef HANDLE WNThreadNativeHandle;
    #elif defined _WN_POSIX
        typedef pthread_t WNThreadNativeHandle;
    #endif

    namespace WNThreadReturnCode {
        enum Type {
            #include "WNCore/inc/Internal/WNErrors.inc"
            eWNCreationFailed,
        };
    }

    template <typename Return>
    class WNThread : public __WNInternal::__WNThreadBase<Return> {
    public:
        WN_FORCE_INLINE WNThread() {}

        WNThreadReturnCode::Type Initialize(Return (*_function)(), const WN_UINT32 _stackSize = 0);

        template <typename Parameter>
        WNThreadReturnCode::Type Initialize(Return (*_function)(Parameter), Parameter _parameter, const WN_UINT32 _stackSize = 0);

        template <typename Parameter>
        WNThreadReturnCode::Type Initialize(Return (*_function)(Parameter&), Parameter& _parameter, const WN_UINT32 _stackSize = 0);

        template <typename Class>
        WNThreadReturnCode::Type Initialize(Class* _object, Return (Class::*_function)(), const WN_UINT32 _stackSize = 0);

        template <typename Class, typename Parameter>
        WNThreadReturnCode::Type Initialize(Class* _object, Return (Class::*_function)(Parameter), Parameter _parameter, const WN_UINT32 _stackSize = 0);

        template <typename Class, typename Parameter>
        WNThreadReturnCode::Type Initialize(Class* _object, Return (Class::*_function)(Parameter&), Parameter& _parameter, const WN_UINT32 _stackSize = 0);

        WNThreadNativeHandle GetNativeHandle() const;

        WN_BOOL IsValid() const;
        WN_BOOL IsReady() const;
        WN_BOOL IsRunning() const;
        WN_BOOL IsComplete() const;

        WN_BOOL WaitForCompletion() const;

        WN_VOID Detach();

    private:
        typedef class __WNInternal::__WNThreadBase<Return> Base;

    private:
        WNThread(const WNThread&);
        WNThread& operator = (const WNThread&);

        WNThreadReturnCode::Type SetupStatic(Return (*_function)(), const WN_UINT32 _stackSize);

        template <typename Parameter>
        WNThreadReturnCode::Type SetupStatic(Return (*_function)(Parameter), Parameter _parameter, const WN_UINT32 _stackSize);

        template <typename Class>
        WNThreadReturnCode::Type SetupMember(Class* _object, Return (Class::*_function)(), const WN_UINT32 _stackSize);

        template <typename Class, typename Parameter>
        WNThreadReturnCode::Type SetupMember(Class* _object, Return (Class::*_function)(Parameter), Parameter _parameter, const WN_UINT32 _stackSize);

        WNThreadReturnCode::Type SetupExecution(__WNInternal::__WNThreadNativeStartRoutine _function, __WNInternal::__WNThreadNativeParameter _data, const WN_UINT32 _stackSize);
    };

    WN_VOID WNThreadYield();
    WN_VOID WNThreadSleep(const WN_UINT32 _milliseconds);
}

#include "WNConcurrency/inc/Internal/WNThread.inl"

#endif // __WN_CONCURRENCY_THREAD_H__