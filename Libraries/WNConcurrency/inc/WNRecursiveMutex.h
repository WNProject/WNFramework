////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_RECURSIVE_MUTEX_H__
#define __WN_CONCURRENCY_RECURSIVE_MUTEX_H__

#include "WNCore/inc/WNTypes.h"

#ifdef _WN_WINDOWS
    #include <windows.h>
#elif defined _WN_POSIX
    #include <pthread.h>
#endif

namespace WNConcurrency {
    class WNRecursiveMutex {
    public:
        WNRecursiveMutex(const WN_UINT32 _spinCount = 4000);
        ~WNRecursiveMutex();

        WN_VOID Lock();
        WN_BOOL TryLock();
        WN_VOID Unlock();

        WN_VOID SetSpinCount(const WN_UINT32 _spinCount);
        WN_UINT32 GetSpinCount() const;

    private:
        WNRecursiveMutex(const WNRecursiveMutex&);
        WNRecursiveMutex& operator = (const WNRecursiveMutex&);

    private:
        template <typename Lock>
        friend class WNConditionVariable;

    private:
        #ifdef _WN_WINDOWS
            CRITICAL_SECTION mCriticalSection;
        #elif defined _WN_POSIX
            pthread_mutex_t mMutex;
        #endif

        WN_ATOM_T mSpinCount;
    };
}

#include "WNConcurrency/inc/Internal/WNRecursiveMutex.inl"

#endif // __WN_CONCURRENCY_RECURSIVE_MUTEX_H__
