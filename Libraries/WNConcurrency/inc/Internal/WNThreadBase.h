// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_INTERNAL_THREAD_BASE_H__
#define __WN_CONCURRENCY_INTERNAL_THREAD_BASE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNConcurrency/inc/WNResourceBase.h"

#ifdef _WN_WINDOWS
    #include <windows.h>
#elif defined _WN_POSIX
    #include "WNConcurrency/inc/WNSemaphore.h"

    #include <pthread.h>
#endif

namespace WNConcurrency {
    namespace WNThreadState {
        enum Type {
            eWNInvalid = 0,
            eWNReady = 1,
            eWNRunning = 2,
            eWNComplete = 3
        };
    }

    namespace __WNInternal {
        #ifdef _WN_WINDOWS
            typedef LPVOID __WNThreadNativeParameter;
            typedef DWORD __WNThreadNativeReturn;
            typedef LPTHREAD_START_ROUTINE __WNThreadNativeStartRoutine;
        #elif defined _WN_POSIX
            typedef void* __WNThreadNativeParameter;
            typedef void* __WNThreadNativeReturn;
            typedef void* WN_OSCALL_BEGIN (*__WNThreadNativeStartRoutine)(void*) WN_OSCALL_END;
        #endif

        class __WNThreadDataCommon : public WNConcurrency::WNResourceBase {
        public:
            __WNThreadDataCommon();
            virtual ~__WNThreadDataCommon();

        public:
            WN_UINT32 mID;

            #ifdef _WN_WINDOWS
                HANDLE mHandle;
            #elif defined _WN_POSIX
                pthread_t mThread;
                WNSemaphore mSemaphore;
                volatile WN_BOOL mValid;
            #endif

            WN_ATOM_T mState;
        };

        template <typename Return>
        class __WNThreadData : public __WNThreadDataCommon {
        public:
            __WNThreadData();
            ~__WNThreadData();

        public:
            Return* mReturn;
        };

        template <>
        class __WNThreadData<WN_VOID> : public __WNThreadDataCommon {};

        template <typename Return>
        class __WNThreadStartDataStatic0 {
        public:
            __WNThreadStartDataStatic0(Return (*_function)(), __WNThreadData<Return>* _data);

        private:
            __WNThreadStartDataStatic0(__WNThreadStartDataStatic0&);
            __WNThreadStartDataStatic0& operator = (__WNThreadStartDataStatic0&);

        public:
            Return (*mFunction)();
            __WNThreadData<Return>* mData;
        };

        template <typename Return, typename Parameter>
        class __WNThreadStartDataStatic1 {
        public:
            __WNThreadStartDataStatic1(Return (*_function)(Parameter), Parameter _parameter, __WNThreadData<Return>* _data);

        private:
            __WNThreadStartDataStatic1(__WNThreadStartDataStatic1&);
            __WNThreadStartDataStatic1& operator = (__WNThreadStartDataStatic1&);

        public:
            Parameter mParameter;
            Return (*mFunction)(Parameter);
            __WNThreadData<Return>* mData;
        };

        template <typename Return, typename Class>
        class __WNThreadStartDataMember0 {
        public:
            __WNThreadStartDataMember0(Class* _object, Return (Class::*_function)(), __WNThreadData<Return>* _data);

        public:
            __WNThreadStartDataMember0(__WNThreadStartDataMember0&);
            __WNThreadStartDataMember0& operator = (__WNThreadStartDataMember0&);

        public:
            Class* mObject;
            Return (Class::*mFunction)();
            __WNThreadData<Return>* mData;
        };

        template <typename Return, typename Class, typename Parameter>
        class __WNThreadStartDataMember1 {
        public:
            __WNThreadStartDataMember1(Class* _object, Return (Class::*_function)(Parameter), Parameter _parameter, __WNThreadData<Return>* _data);

        private:
            __WNThreadStartDataMember1(__WNThreadStartDataMember1&);
            __WNThreadStartDataMember1& operator = (__WNThreadStartDataMember1&);

        public:
            Class* mObject;
            Parameter mParameter;
            Return (Class::*mFunction)(Parameter);
            __WNThreadData<Return>* mData;
        };

        template <typename Return>
        class __WNThreadBaseCommon {
        public:
            WNThreadState::Type GetState() const;

        protected:
            __WNThreadBaseCommon();
            virtual ~__WNThreadBaseCommon();

            WN_VOID SetupData();
            WN_VOID CleanupData();

        protected:
            __WNThreadData<Return>* mData;
        };

        template <typename Return>
        class __WNThreadBase : public __WNThreadBaseCommon<Return> {
        public:
            WN_BOOL GetReturn(Return* _return) const;

        protected:
            class StartStatic0 {
            public:
                static __WNThreadNativeReturn WN_OSCALL_BEGIN Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END;

            private:
                StartStatic0();
            };

            template <typename Parameter>
            class StartStatic1 {
            public:
                static __WNThreadNativeReturn WN_OSCALL_BEGIN Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END;

            private:
                StartStatic1();
            };

            template <typename Class>
            class StartMember0 {
            public:
                static __WNThreadNativeReturn WN_OSCALL_BEGIN Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END;

            private:
                StartMember0();
            };

            template <typename Class, typename Parameter>
            class StartMember1 {
            public:
                static __WNThreadNativeReturn WN_OSCALL_BEGIN Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END;

            private:
                StartMember1();
            };

        protected:
            __WNThreadBase() {}

        private:
            typedef __WNThreadBaseCommon<Return> Base;
        };

        template <>
        class __WNThreadBase<WN_VOID> : public __WNThreadBaseCommon<WN_VOID> {
        protected:
            class StartStatic0 {
            public:
                static __WNThreadNativeReturn WN_OSCALL_BEGIN Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END;

            private:
                StartStatic0();
            };

            template <typename Parameter>
            class StartStatic1 {
            public:
                static __WNThreadNativeReturn WN_OSCALL_BEGIN Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END;

            private:
                StartStatic1();
            };

            template <typename Class>
            class StartMember0 {
            public:
                static __WNThreadNativeReturn WN_OSCALL_BEGIN Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END;

            private:
                StartMember0();
            };

            template <typename Class, typename Parameter>
            class StartMember1 {
            public:
                static __WNThreadNativeReturn WN_OSCALL_BEGIN Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END;

            private:
                StartMember1();
            };

        protected:
            __WNThreadBase() {}

        private:
            typedef __WNThreadBaseCommon<WN_VOID> Base;
        };
    }
}

#include "WNConcurrency/inc/Internal/WNThreadBase.inl"

#endif // __WN_CONCURRENCY_INTERNAL_THREAD_BASE_H__