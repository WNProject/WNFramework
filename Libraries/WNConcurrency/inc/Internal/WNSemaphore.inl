////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONCURRENCY_SEMAPHORE_INL__
#define __WN_CONCURRENCY_SEMAPHORE_INL__

#ifndef __WN_CONCURRENCY_SEMAPHORE_H__
    #error "WNSemaphore.inl should never be included directly. Please include WNSemaphore.h instead"
#endif

#include "WNCore/inc/WNAssert.h"

#ifdef _WN_WINDOWS
    #include <windows.h>
    #include <limits.h>
#elif defined _WN_POSIX
    #include <errno.h>
#endif

#define __WN_SEMAPHORE_CREATE_ERR_MSG "Error: Failed to create semaphore object."
#define __WN_SEMAPHORE_WAIT_ERR_MSG "Error: Failed to wait on semaphore object."
#define __WN_SEMAPHORE_POST_ERR_MSG "Error: Failed to post desired count to semaphore object."
#define __WN_SEMAPHORE_GET_COUNT_ERR_MSG "Error: Failed to get semaphore object count."
#define __WN_SEMAPHORE_DESTROY_WARN_MSG "Warning: Failed to destroy semaphore object."
#define __WN_SEMAPHORE_POST_WARN_MSG "Warning: Posted a semaphore object with a count of 0."

namespace WNConcurrency {
    WN_FORCE_INLINE WNSemaphore::WNSemaphore(const WN_UINT32 _count) {
        #ifdef _WN_WINDOWS
            mHandle = CreateSemaphore(NULL, _count, LONG_MAX, NULL);

            WN_RELEASE_ASSERT_DESC(mHandle != NULL, __WN_SEMAPHORE_CREATE_ERR_MSG);
        #elif defined _WN_POSIX
            const int result = sem_init(&mSemaphore, 0, _count);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_SEMAPHORE_CREATE_ERR_MSG);
        #endif
    }

    WN_FORCE_INLINE WNSemaphore::~WNSemaphore() {
        #ifdef _WN_WINDOWS
            const BOOL result = CloseHandle(mHandle);

            WN_DEBUG_ASSERT_DESC(result != FALSE, __WN_SEMAPHORE_DESTROY_WARN_MSG);
        #elif defined _WN_POSIX
            const int result = sem_destroy(&mSemaphore);

            WN_DEBUG_ASSERT_DESC(result == 0, __WN_SEMAPHORE_DESTROY_WARN_MSG);
        #endif

        #ifdef _WN_RELEASE
            WN_UNUSED_ARG(result);
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNSemaphore::Wait() {
        #ifdef _WN_WINDOWS
            const DWORD result = WaitForSingleObject(mHandle, INFINITE);

            WN_RELEASE_ASSERT_DESC(result == WAIT_OBJECT_0, __WN_SEMAPHORE_WAIT_ERR_MSG);
        #elif defined _WN_POSIX
            int result;

            while ((result = sem_wait(&mSemaphore)) == EINTR) {
                continue;
            }

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_SEMAPHORE_WAIT_ERR_MSG);
        #endif
    }

    WN_FORCE_INLINE WN_BOOL WNSemaphore::TryWait() {
        #ifdef _WN_WINDOWS
            return(WaitForSingleObject(mHandle, 0) != WAIT_TIMEOUT ? WN_TRUE : WN_FALSE);
        #elif defined _WN_POSIX
            int result;

            while ((result = sem_trywait(&mSemaphore)) == EINTR) {
                continue;
            }

            return(result == 0 ? WN_TRUE : WN_FALSE);
        #endif
    }

    WN_FORCE_INLINE WN_VOID WNSemaphore::Post(const WN_UINT32 _count) {
        WN_DEBUG_ASSERT_DESC(_count != 0, __WN_SEMAPHORE_POST_WARN_MSG);

        #ifdef _WN_WINDOWS
            const BOOL result = ReleaseSemaphore(mHandle, _count, NULL);

            WN_RELEASE_ASSERT_DESC(result != FALSE, __WN_SEMAPHORE_POST_ERR_MSG);
        #elif defined _WN_POSIX
            for (WN_UINT32 i = 0; i < _count; ++i) {
                const int result = sem_post(&mSemaphore);

                WN_RELEASE_ASSERT_DESC(result == 0, __WN_SEMAPHORE_POST_ERR_MSG);
            }
        #endif
    }

    WN_FORCE_INLINE WN_UINT32 WNSemaphore::GetCount() const {
        #ifdef _WN_WINDOWS
            LONG currentCount = -1;
            const BOOL result = ReleaseSemaphore(mHandle, 0, &currentCount);

            WN_RELEASE_ASSERT_DESC(result != FALSE, __WN_SEMAPHORE_GET_COUNT_ERR_MSG);
        #elif defined _WN_POSIX
            int currentCount = -1;
            const int result = sem_getvalue(const_cast<sem_t*>(&mSemaphore), &currentCount);

            WN_RELEASE_ASSERT_DESC(result == 0, __WN_SEMAPHORE_GET_COUNT_ERR_MSG);
        #endif

        return(static_cast<WN_UINT32>(currentCount));
    }
}

#endif // __WN_CONCURRENCY_SEMAPHORE_INL__
