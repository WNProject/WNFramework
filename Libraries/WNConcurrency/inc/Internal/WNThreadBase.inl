////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_INTERNAL_THREAD_BASE_INL__
#define __WN_CONCURRENCY_INTERNAL_THREAD_BASE_INL__

#ifndef __WN_CONCURRENCY_THREAD_H__
    #error "WNThreadBase.inl should never be included directly. Please include WNThread.h instead"
#endif

#include "WNCore/inc/WNMacros.h"
#include "WNCore/inc/WNAssert.h"
#include "WNMemory/inc/WNAllocation.h"

#ifdef _WN_WINDOWS
    #include <windows.h>

    #define __WN_THREAD_NATIVE_DEFAULT_RETURN_VALUE 0
#elif defined _WN_POSIX
    #define __WN_THREAD_NATIVE_DEFAULT_RETURN_VALUE NULL
#endif

#ifdef _WN_LINUX
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/syscall.h>
#endif

namespace WNConcurrency {
    namespace __WNInternal {
        WN_FORCE_INLINE __WNThreadDataCommon::__WNThreadDataCommon() :
            #ifdef _WN_WINDOWS
                mHandle(INVALID_HANDLE_VALUE),
            #elif defined _WN_POSIX
                mValid(WN_FALSE),
            #endif
            mID(static_cast<WN_UINT32>(-1)),
            mState(WNThreadState::eWNInvalid) {
        }

        WN_FORCE_INLINE __WNThreadDataCommon::~__WNThreadDataCommon() {
            #ifdef _WN_WINDOWS
                if (mHandle != INVALID_HANDLE_VALUE && mHandle != NULL) {
                    WN_RELEASE_ASSERT(CloseHandle(mHandle));
                }
            #elif defined _WN_POSIX
                if (mValid) {
                    WN_RELEASE_ASSERT(pthread_detach(mThread) == 0);
                }
            #endif
        }

        template <typename Return>
        WN_FORCE_INLINE __WNThreadData<Return>::__WNThreadData() :
            mReturn(WN_NULL) {
        }

        template <typename Return>
        WN_FORCE_INLINE __WNThreadData<Return>::~__WNThreadData() {
            if (mReturn != WN_NULL) {
                WNMemory::WNFreeT(mReturn);
            }
        }

        template <typename Return>
        WN_FORCE_INLINE __WNThreadStartDataStatic0<Return>::__WNThreadStartDataStatic0(Return (*_function)(), __WNThreadData<Return>* _data) :
            mFunction(_function),
            mData(_data) {
        }

        template <typename Return, typename Parameter>
        WN_FORCE_INLINE __WNThreadStartDataStatic1<Return, Parameter>::__WNThreadStartDataStatic1(Return (*_function)(Parameter), Parameter _parameter, __WNThreadData<Return>* _data) :
            mParameter(_parameter),
            mFunction(_function),
            mData(_data) {
        }

        template <typename Return, typename Class>
        WN_FORCE_INLINE __WNThreadStartDataMember0<Return, Class>::__WNThreadStartDataMember0(Class* _object, Return (Class::*_function)(), __WNThreadData<Return>* _data) :
            mObject(_object),
            mFunction(_function),
            mData(_data) {
        }

        template <typename Return, typename Class, typename Parameter>
        WN_FORCE_INLINE __WNThreadStartDataMember1<Return, Class, Parameter>::__WNThreadStartDataMember1(Class* _object, Return (Class::*_function)(Parameter), Parameter _parameter, __WNThreadData<Return>* _data) :
            mObject(_object),
            mParameter(_parameter),
            mFunction(_function),
            mData(_data) {
        }

        template <typename Return>
        WN_FORCE_INLINE __WNThreadBaseCommon<Return>::__WNThreadBaseCommon() :
            mData(WN_NULL) {
        }

        template <typename Return>
        WN_FORCE_INLINE __WNThreadBaseCommon<Return>::~__WNThreadBaseCommon() {
            CleanupData();
        }

        template <typename Return>
        WN_FORCE_INLINE WNThreadState::Type __WNThreadBaseCommon<Return>::GetState() const {
            if (mData != WN_NULL) {
                return(static_cast<WNThreadState::Type>(mData->mState));
            }

            return(WNThreadState::eWNInvalid);
        }

        template <typename Return>
        WN_FORCE_INLINE WN_VOID __WNThreadBaseCommon<Return>::SetupData() {
            CleanupData();

            mData = WN_NEW __WNThreadData<Return>();

            WN_RELEASE_ASSERT(mData != WN_NULL);

            mData->AddRef();
        }

        template <typename Return>
        WN_FORCE_INLINE WN_VOID __WNThreadBaseCommon<Return>::CleanupData() {
            if (mData != WN_NULL) {
                if (mData->RemoveRef()) {
                    WN_DELETE(mData);
                }

                mData = WN_NULL;
            }
        }

        template <typename Return>
        WN_FORCE_INLINE WN_BOOL __WNThreadBase<Return>::GetReturn(Return* _return) const {
            if (this->mData != WN_NULL) {
                if (this->mData->mReturn != WN_NULL && this->mData->mState == WNThreadState::eWNComplete) {
                    *_return = *static_cast<Return*>(this->mData->mReturn);

                    return(WN_TRUE);
                }
            }

            return(WN_FALSE);
        }

        template <typename Return>
        WN_INLINE __WNThreadNativeReturn WN_OSCALL_BEGIN __WNThreadBase<Return>::StartStatic0::Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END {
            __WNThreadStartDataStatic0<Return>* threadStartData = reinterpret_cast<__WNThreadStartDataStatic0<Return>*>(_threadStartData);
            __WNThreadData<Return>* threadData = threadStartData->mData;

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNRunning);

            #ifdef _WN_LINUX
                threadData->mID = static_cast<WN_UINT32>(syscall(SYS_gettid));
            #endif

            threadData->mReturn = WNMemory::WNMallocT<Return>();

            WN_RELEASE_ASSERT(threadData->mReturn != WN_NULL);

            *(threadData->mReturn) = threadStartData->mFunction();

            WN_DELETE(threadStartData);

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNComplete);

            #ifdef _WN_POSIX
                threadData->mSemaphore.Post();
            #endif

            if (threadData->RemoveRef()) {
                WN_DELETE(threadData);
            }

            return(__WN_THREAD_NATIVE_DEFAULT_RETURN_VALUE);
        }

        template <typename Return>
        template <typename Parameter>
        WN_INLINE __WNThreadNativeReturn WN_OSCALL_BEGIN __WNThreadBase<Return>::StartStatic1<Parameter>::Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END {
            __WNThreadStartDataStatic1<Return, Parameter>* threadStartData = reinterpret_cast<__WNThreadStartDataStatic1<Return, Parameter>*>(_threadStartData);
            __WNThreadData<Return>* threadData = threadStartData->mData;

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNRunning);

            #ifdef _WN_LINUX
                threadData->mID = static_cast<WN_UINT32>(syscall(SYS_gettid));
            #endif

            threadData->mReturn = WNMemory::WNMallocT<Return>();

            WN_RELEASE_ASSERT(threadData->mReturn != WN_NULL);

            *(threadData->mReturn) = threadStartData->mFunction(threadStartData->mParameter);

            WN_DELETE(threadStartData);

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNComplete);

            #ifdef _WN_POSIX
                threadData->mSemaphore.Post();
            #endif

            if (threadData->RemoveRef()) {
                WN_DELETE(threadData);
            }

            return(__WN_THREAD_NATIVE_DEFAULT_RETURN_VALUE);
        }

        template <typename Return>
        template <typename Class>
        WN_INLINE __WNThreadNativeReturn WN_OSCALL_BEGIN __WNThreadBase<Return>::StartMember0<Class>::Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END {
            __WNThreadStartDataMember0<Return, Class>* threadStartData = reinterpret_cast<__WNThreadStartDataMember0<Return, Class>*>(_threadStartData);
            __WNThreadData<Return>* threadData = threadStartData->mData;

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNRunning);

            #ifdef _WN_LINUX
                threadData->mID = static_cast<WN_UINT32>(syscall(SYS_gettid));
            #endif

            threadData->mReturn = WNMemory::WNMallocT<Return>();

            WN_RELEASE_ASSERT(threadData->mReturn != WN_NULL);

            *(threadData->mReturn) = (threadStartData->mObject->*(threadStartData->mFunction))();

            WN_DELETE(threadStartData);

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNComplete);

            #ifdef _WN_POSIX
                threadData->mSemaphore.Post();
            #endif

            if (threadData->RemoveRef()) {
                WN_DELETE(threadData);
            }

            return(__WN_THREAD_NATIVE_DEFAULT_RETURN_VALUE);
        }

        template <typename Return>
        template <typename Class, typename Parameter>
        WN_INLINE __WNThreadNativeReturn WN_OSCALL_BEGIN __WNThreadBase<Return>::StartMember1<Class, Parameter>::Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END {
            __WNThreadStartDataMember1<Return, Class, Parameter>* threadStartData = reinterpret_cast<__WNThreadStartDataMember1<Return, Class, Parameter>*>(_threadStartData);
            __WNThreadData<Return>* threadData = threadStartData->mData;

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNRunning);

            #ifdef _WN_LINUX
                threadData->mID = static_cast<WN_UINT32>(syscall(SYS_gettid));
            #endif

            threadData->mReturn = WNMemory::WNMallocT<Return>();

            WN_RELEASE_ASSERT(threadData->mReturn != WN_NULL);

            *(threadData->mReturn) = (threadStartData->mObject->*(threadStartData->mFunction))(threadStartData->mParameter);

            WN_DELETE(threadStartData);

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNComplete);

            #ifdef _WN_POSIX
                threadData->mSemaphore.Post();
            #endif

            if (threadData->RemoveRef()) {
                WN_DELETE(threadData);
            }

            return(__WN_THREAD_NATIVE_DEFAULT_RETURN_VALUE);
        }

        WN_INLINE __WNThreadNativeReturn WN_OSCALL_BEGIN __WNThreadBase<WN_VOID>::StartStatic0::Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END {
            __WNThreadStartDataStatic0<WN_VOID>* threadStartData = reinterpret_cast<__WNThreadStartDataStatic0<WN_VOID>*>(_threadStartData);
            __WNThreadData<WN_VOID>* threadData = threadStartData->mData;

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNRunning);

            #ifdef _WN_LINUX
                threadData->mID = static_cast<WN_UINT32>(syscall(SYS_gettid));
            #endif

            threadStartData->mFunction();

            WN_DELETE(threadStartData);

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNComplete);

            #ifdef _WN_POSIX
                threadData->mSemaphore.Post();
            #endif

            if (threadData->RemoveRef()) {
                WN_DELETE(threadData);
            }

            return(__WN_THREAD_NATIVE_DEFAULT_RETURN_VALUE);
        }

        template <typename Parameter>
        WN_INLINE __WNThreadNativeReturn WN_OSCALL_BEGIN __WNThreadBase<WN_VOID>::StartStatic1<Parameter>::Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END {
            __WNThreadStartDataStatic1<WN_VOID, Parameter>* threadStartData = reinterpret_cast<__WNThreadStartDataStatic1<WN_VOID, Parameter>*>(_threadStartData);
            __WNThreadData<WN_VOID>* threadData = threadStartData->mData;

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNRunning);

            #ifdef _WN_LINUX
                threadData->mID = static_cast<WN_UINT32>(syscall(SYS_gettid));
            #endif

            threadStartData->mFunction(threadStartData->mParameter);

            WN_DELETE(threadStartData);

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNComplete);

            #ifdef _WN_POSIX
                threadData->mSemaphore.Post();
            #endif

            if (threadData->RemoveRef()) {
                WN_DELETE(threadData);
            }

            return(__WN_THREAD_NATIVE_DEFAULT_RETURN_VALUE);
        }

        template <typename Class>
        WN_INLINE __WNThreadNativeReturn WN_OSCALL_BEGIN __WNThreadBase<WN_VOID>::StartMember0<Class>::Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END {
            __WNThreadStartDataMember0<WN_VOID, Class>* threadStartData = reinterpret_cast<__WNThreadStartDataMember0<WN_VOID, Class>*>(_threadStartData);
            __WNThreadData<WN_VOID>* threadData = threadStartData->mData;

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNRunning);

            #ifdef _WN_LINUX
                threadData->mID = static_cast<WN_UINT32>(syscall(SYS_gettid));
            #endif

            (threadStartData->mObject->*(threadStartData->mFunction))();

            WN_DELETE(threadStartData);

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNComplete);

            #ifdef _WN_POSIX
                threadData->mSemaphore.Post();
            #endif

            if (threadData->RemoveRef()) {
                WN_DELETE(threadData);
            }

            return(__WN_THREAD_NATIVE_DEFAULT_RETURN_VALUE);
        }

        template <typename Class, typename Parameter>
        WN_INLINE __WNThreadNativeReturn WN_OSCALL_BEGIN __WNThreadBase<WN_VOID>::StartMember1<Class, Parameter>::Execute(__WNThreadNativeParameter _threadStartData) WN_OSCALL_END {
            __WNThreadStartDataMember1<WN_VOID, Class, Parameter>* threadStartData = reinterpret_cast<__WNThreadStartDataMember1<WN_VOID, Class, Parameter>*>(_threadStartData);
            __WNThreadData<WN_VOID>* threadData = threadStartData->mData;

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNRunning);

            #ifdef _WN_LINUX
                threadData->mID = static_cast<WN_UINT32>(syscall(SYS_gettid));
            #endif

            (threadStartData->mObject->*(threadStartData->mFunction))(threadStartData->mParameter);

            WN_DELETE(threadStartData);

            WNAtomicSwap(&(threadData->mState), WNThreadState::eWNComplete);

            #ifdef _WN_POSIX
                threadData->mSemaphore.Post();
            #endif

            if (threadData->RemoveRef()) {
                WN_DELETE(threadData);
            }

            return(__WN_THREAD_NATIVE_DEFAULT_RETURN_VALUE);
        }
    }
}

#endif // __WN_CONCURRENCY_INTERNAL_THREAD_BASE_INL__
