////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_RECURSIVE_SPIN_LOCK_H__
#define __WN_CONCURRENCY_RECURSIVE_SPIN_LOCK_H__

#include "WNCore/inc/WNTypes.h"
#include "WNConcurrency/inc/WNConfig.h"

#ifdef _WN_WINDOWS
    #include <windows.h>
#elif defined _WN_LINUX
    #include <unistd.h>
#endif

#ifdef __WN_PTHREAD_SPIN_LOCKS
    #include <pthread.h>
#endif

namespace WNConcurrency {
    class WNRecursiveSpinLock {
    public:
        WNRecursiveSpinLock();
        ~WNRecursiveSpinLock();

        WN_VOID Lock();
        WN_BOOL TryLock();
        WN_VOID Unlock();

    private:
        WNRecursiveSpinLock(const WNRecursiveSpinLock&);
        WNRecursiveSpinLock& operator = (const WNRecursiveSpinLock&);

    private:
        #ifdef _WN_WINDOWS
            volatile DWORD mOwningThread;
        #elif defined _WN_LINUX
            volatile pid_t mOwningThread;
        #endif

        volatile WN_UINT32 mRecursionCount;

        #ifdef __WN_PTHREAD_SPIN_LOCKS
            pthread_spinlock_t mSpinLock;
        #else
            WN_ATOM_T mSpinFlag;
        #endif
    };
}

#include "WNConcurrency/inc/Internal/WNRecursiveSpinLock.inl"

#endif // __WN_CONCURRENCY_RECURSIVE_SPIN_LOCK_H__