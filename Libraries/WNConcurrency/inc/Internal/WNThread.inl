////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_THREAD_INL__
#define __WN_CONCURRENCY_THREAD_INL__

#ifndef __WN_CONCURRENCY_THREAD_H__
    #error "WNThread.inl should never be included directly. Please include WNThread.h instead"
#endif

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNMacros.h"
#include "WNMemory/inc/WNAllocation.h"

#ifdef _WN_WINDOWS
    #include <windows.h>
#elif defined _WN_POSIX
    #include "WNConcurrency/inc/WNSemaphore.h"

    #include <pthread.h>
    #include <time.h>
#endif

#ifdef _WN_LINUX
    #include <sys/sysinfo.h>
#endif

namespace WNConcurrency {
    template <typename Return>
    WN_INLINE typename WNThreadReturnCode::Type WNThread<Return>::Initialize(Return (*_function)(), const WN_UINT32 _stackSize) {
        return(SetupStatic(_function, _stackSize));
    }

    template <typename Return>
    template <typename Class>
    WN_INLINE typename WNThreadReturnCode::Type WNThread<Return>::Initialize(Class* _object, Return (Class::*_function)(), const WN_UINT32 _stackSize) {
        return(SetupMember<Class>(_object, _function, _stackSize));
    }

    template <typename Return>
    template <typename Parameter>
    WN_INLINE typename WNThreadReturnCode::Type WNThread<Return>::Initialize(Return (*_function)(Parameter), Parameter _parameter, const WN_UINT32 _stackSize) {
        return(SetupStatic<Parameter>(_function, _parameter, _stackSize));
    }

    template <typename Return>
    template <typename Parameter>
    WN_INLINE typename WNThreadReturnCode::Type WNThread<Return>::Initialize(Return (*_function)(Parameter&), Parameter& _parameter, const WN_UINT32 _stackSize) {
        return(SetupStatic<Parameter&>(_function, _parameter, _stackSize));
    }

    template <typename Return>
    template <typename Class, typename Parameter>
    WN_INLINE typename WNThreadReturnCode::Type WNThread<Return>::Initialize(Class* _object, Return (Class::*_function)(Parameter), Parameter _parameter, const WN_UINT32 _stackSize) {
        return(SetupMember<Class, Parameter>(_object, _function, _parameter, _stackSize));
    }

    template <typename Return>
    template <typename Class, typename Parameter>
    WN_INLINE typename WNThreadReturnCode::Type WNThread<Return>::Initialize(Class* _object, Return (Class::*_function)(Parameter&), Parameter& _parameter, const WN_UINT32 _stackSize) {
        return(SetupMember<Class, Parameter&>(_object, _function, _parameter, _stackSize));
    }

    template <typename Return>
    WN_FORCE_INLINE WNThreadNativeHandle WNThread<Return>::GetNativeHandle() const {
        #ifdef _WN_WINDOWS
            return(static_cast<WNThreadNativeHandle>(this->mData->mHandle));
        #elif defined _WN_POSIX
            if (IsValid()) {
                return(static_cast<WNThreadNativeHandle>(this->mData->mThread));
            } else {
                return(WN_THREAD_INVALID_NATIVE_HANDLE);
            }
        #endif
    }

    template <typename Return>
    WN_FORCE_INLINE WN_BOOL WNThread<Return>::IsValid() const {
        if (Base::GetState() != WNThreadState::eWNInvalid) {
            #ifdef _WN_WINDOWS
                return(this->mData->mHandle != NULL && this->mData->mHandle != INVALID_HANDLE_VALUE);
            #elif defined _WN_POSIX
                return(this->mData->mValid);
            #endif
        }

        return(WN_FALSE);
    }

    template <typename Return>
    WN_FORCE_INLINE WN_BOOL WNThread<Return>::IsReady() const {
        return(Base::GetState() == WNThreadState::eWNReady);
    }

    template <typename Return>
    WN_FORCE_INLINE WN_BOOL WNThread<Return>::IsRunning() const {
        return(Base::GetState() == WNThreadState::eWNRunning);
    }

    template <typename Return>
    WN_FORCE_INLINE WN_BOOL WNThread<Return>::IsComplete() const {
        return(Base::GetState() == WNThreadState::eWNComplete);
    }

    template <typename Return>
    WN_FORCE_INLINE WN_BOOL WNThread<Return>::WaitForCompletion() const {
        if (IsValid()) {
            if (!IsComplete()) {
                #ifdef _WN_WINDOWS
                    if (WaitForSingleObject(this->mData->mHandle, INFINITE) != WAIT_OBJECT_0) {
                        return(WN_FALSE);
                    }
                #elif defined _WN_POSIX
                    this->mData->mSemaphore.Wait();
                    this->mData->mSemaphore.Post();
                #endif
            }
        } else {
            return(WN_FALSE);
        }

        return(WN_TRUE);
    }

    template <typename Return>
    WN_VOID WNThread<Return>::Detach() {
        Base::CleanupData();
    }

    template <typename Return>
    WN_INLINE WNThreadReturnCode::Type WNThread<Return>::SetupExecution(__WNInternal::__WNThreadNativeStartRoutine _function, __WNInternal::__WNThreadNativeParameter _data, const WN_UINT32 _stackSize) {
        this->mData->AddRef();

        #ifdef _WN_WINDOWS
            this->mData->mHandle = CreateThread(NULL, _stackSize, _function, _data, 0, reinterpret_cast<LPDWORD>(&(this->mData->mID)));

            if (this->mData->mHandle == NULL) {
                this->mData->RemoveRef();

                this->mData->mHandle = INVALID_HANDLE_VALUE;

                return(WNThreadReturnCode::eWNCreationFailed);
            }
        #elif defined _WN_POSIX
            pthread_attr_t attributes;

            if (pthread_attr_init(&attributes) == 0) {
                if (_stackSize != 0) {
                    WN_DEBUG_ASSERT(pthread_attr_setstacksize(&attributes, static_cast<size_t>(_stackSize)) == 0);
                }

                const WN_UINT32 result = pthread_create(&(this->mData->mThread), &attributes, _function, _data);

                pthread_attr_destroy(&attributes);

                if (result != 0) {
                    this->mData->RemoveRef();

                    return(WNThreadReturnCode::eWNCreationFailed);
                }

                this->mData->mValid = WN_TRUE;
            } else {
                return(WNThreadReturnCode::eWNError);
            }
        #endif

        WNAtomicSwap(&(this->mData->mState), WNThreadState::eWNReady);

        return(WNThreadReturnCode::eWNOK);
    }

    template <typename Return>
    WN_FORCE_INLINE WNThreadReturnCode::Type WNThread<Return>::SetupStatic(Return (*_function)(), const WN_UINT32 _stackSize) {
        Base::SetupData();

        __WNInternal::__WNThreadStartDataStatic0<Return>* threadStartData = WN_NEW __WNInternal::__WNThreadStartDataStatic0<Return>(_function, this->mData);
        __WNInternal::__WNThreadNativeStartRoutine function = reinterpret_cast<__WNInternal::__WNThreadNativeStartRoutine>(&(Base::StartStatic0::Execute));
        __WNInternal::__WNThreadNativeParameter data = static_cast<__WNInternal::__WNThreadNativeParameter>(threadStartData);

        WNThreadReturnCode::Type result;

        if ((result = SetupExecution(function, data, _stackSize)) != WNThreadReturnCode::eWNOK) {
            WN_DELETE(threadStartData);

            Base::CleanupData();
        }

        return(result);
    }

    template <typename Return>
    template <typename Parameter>
    WN_FORCE_INLINE WNThreadReturnCode::Type WNThread<Return>::SetupStatic(Return (*_function)(Parameter), Parameter _parameter, const WN_UINT32 _stackSize) {
        Base::SetupData();

        __WNInternal::__WNThreadStartDataStatic1<Return, Parameter>* threadStartData = WN_NEW __WNInternal::__WNThreadStartDataStatic1<Return, Parameter>(_function, _parameter, this->mData);
        __WNInternal::__WNThreadNativeStartRoutine function = reinterpret_cast<__WNInternal::__WNThreadNativeStartRoutine>(&(Base::template StartStatic1<Parameter>::Execute));
        __WNInternal::__WNThreadNativeParameter data = static_cast<__WNInternal::__WNThreadNativeParameter>(threadStartData);

        WNThreadReturnCode::Type result;

        if ((result = SetupExecution(function, data, _stackSize)) != WNThreadReturnCode::eWNOK) {
            WN_DELETE(threadStartData);

            Base::CleanupData();
        }

        return(result);
    }

    template <typename Return>
    template <typename Class>
    WN_FORCE_INLINE WNThreadReturnCode::Type WNThread<Return>::SetupMember(Class* _object, Return (Class::*_function)(), const WN_UINT32 _stackSize) {
        Base::SetupData();

        __WNInternal::__WNThreadStartDataMember0<Return, Class>* threadStartData = WN_NEW __WNInternal::__WNThreadStartDataMember0<Return, Class>(_object, _function, this->mData);
        __WNInternal::__WNThreadNativeStartRoutine function = reinterpret_cast<__WNInternal::__WNThreadNativeStartRoutine>(&(Base::template StartMember0<Class>::Execute));
        __WNInternal::__WNThreadNativeParameter data = static_cast<__WNInternal::__WNThreadNativeParameter>(threadStartData);

        WNThreadReturnCode::Type result;

        if ((result = SetupExecution(function, data, _stackSize)) != WNThreadReturnCode::eWNOK) {
            WN_DELETE(threadStartData);

            Base::CleanupData();
        }

        return(result);
    }

    template <typename Return>
    template <typename Class, typename Parameter>
    WN_FORCE_INLINE WNThreadReturnCode::Type WNThread<Return>::SetupMember(Class* _object, Return (Class::*_function)(Parameter), Parameter _parameter, const WN_UINT32 _stackSize) {
        Base::SetupData();

        __WNInternal::__WNThreadStartDataMember1<Return, Class, Parameter>* threadStartData = WN_NEW __WNInternal::__WNThreadStartDataMember1<Return, Class, Parameter>(_object, _function, _parameter, this->mData);
        __WNInternal::__WNThreadNativeStartRoutine function = reinterpret_cast<__WNInternal::__WNThreadNativeStartRoutine>(&(Base::template StartMember1<Class, Parameter>::Execute));
        __WNInternal::__WNThreadNativeParameter data = static_cast<__WNInternal::__WNThreadNativeParameter>(threadStartData);

        WNThreadReturnCode::Type result;

        if ((result = SetupExecution(function, data, _stackSize)) != WNThreadReturnCode::eWNOK) {
            WN_DELETE(threadStartData);

            Base::CleanupData();
        }

        return(result);
    }

    WN_FORCE_INLINE WN_VOID WNThreadYield() {
        #if defined _WN_WINDOWS || defined _WN_LINUX || defined _WN_ANDROID
            static WN_BOOL checkMultiThreaded = WN_FALSE;
            static WN_BOOL multiThreaded = WN_FALSE;

            if (!checkMultiThreaded) {
                #ifdef _WN_WINDOWS
                    SYSTEM_INFO sysInfo = {0};

                    GetSystemInfo(&sysInfo);

                    multiThreaded = (sysInfo.dwNumberOfProcessors > 1);
                #elif defined _WN_LINUX
                    multiThreaded = (sysconf(_SC_NPROCESSORS_ONLN) > 1);
                #endif
            }

            if (multiThreaded) {
                WN_YIELD_PROCESSOR();
            } else {
                #ifdef _WN_WINDOWS
                    SwitchToThread();
                #elif defined _WN_ANDROID
                    sched_yield();
                #elif defined _WN_LINUX
                    pthread_yield();
                #endif
            }
        #elif defined _WN_POSIX
            pthread_yield();
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNThreadSleep(const WN_UINT32 _milliseconds) {
        #ifdef _WN_WINDOWS
            Sleep(_milliseconds);
        #elif defined _WN_POSIX
            struct timespec timeRequested = {0};

            timeRequested.tv_sec = _milliseconds / 1000;
            timeRequested.tv_nsec = (_milliseconds % 1000) * 1000000L;

            while (nanosleep(&timeRequested, &timeRequested) == -1) {
                continue;
            }
        #endif
    }
}

#endif // __WN_CONCURRENCY_THREAD_INL__
