// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_SEMAPHORE_H__
#define __WN_CONCURRENCY_SEMAPHORE_H__

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNUtility.h"

#ifdef _WN_WINDOWS
    #include <windows.h>
    #include <limits.h>
#else
    #include "WNConcurrency/inc/WNSpinLock.h"

    #include <mutex>

    #ifdef _WN_POSIX
        #include <semaphore.h>
        #include <errno.h>
    #else
        #include "WNConcurrency/inc/WNConditionVariable.h"
    #endif
#endif

#define __WN_SEMAPHORE_CREATE_ERR_MSG "Error: Failed to create semaphore object."
#define __WN_SEMAPHORE_WAIT_ERR_MSG "Error: Failed to wait on semaphore object."
#define __WN_SEMAPHORE_POST_ERR_MSG "Error: Failed to post desired m_count to semaphore object."
#define __WN_SEMAPHORE_DESTROY_WARN_MSG "Warning: Failed to destroy semaphore object."
#define __WN_SEMAPHORE_POST_WARN_MSG "Warning: Posted a semaphore object with a m_count of 0."

namespace wn {
    class semaphore final : public non_copyable {
    public:
        WN_FORCE_INLINE explicit semaphore(const wn_uint16 _count = 0) {
            #ifdef _WN_WINDOWS
                m_handle = ::CreateSemaphore(NULL, static_cast<LONG>(_count), LONG_MAX, NULL);

                WN_RELEASE_ASSERT_DESC(m_handle != NULL, __WN_SEMAPHORE_CREATE_ERR_MSG);
            #elif defined _WN_POSIX
                const int result = ::sem_init(&m_semaphore, 0, static_cast<unsigned int>(_count));

                WN_RELEASE_ASSERT_DESC(result == 0, __WN_SEMAPHORE_CREATE_ERR_MSG);
            #else
                m_count = _count;
            #endif
        }

        WN_FORCE_INLINE ~semaphore() {
            #ifdef _WN_WINDOWS
                const BOOL close_result = ::CloseHandle(m_handle);

                WN_DEBUG_ASSERT_DESC(close_result != FALSE, __WN_SEMAPHORE_DESTROY_WARN_MSG);

                #ifndef _WN_DEBUG
                    WN_UNUSED_ARGUMENT(close_result);
                #endif
            #elif defined _WN_POSIX
                const int destroy_result = ::sem_destroy(&m_semaphore);

                WN_DEBUG_ASSERT_DESC(destroy_result == 0, __WN_SEMAPHORE_DESTROY_WARN_MSG);

                #ifndef _WN_DEBUG
                    WN_UNUSED_ARGUMENT(destroy_result);
                #endif
            #endif
        }

        WN_FORCE_INLINE wn_void wait() {
            #ifdef _WN_WINDOWS
                const DWORD wait_result = ::WaitForSingleObject(m_handle, INFINITE);

                WN_RELEASE_ASSERT_DESC(wait_result == WAIT_OBJECT_0, __WN_SEMAPHORE_WAIT_ERR_MSG);
            #elif defined _WN_POSIX
                const std::lock_guard<spin_lock> guard(m_spin_lock);
                int wait_result;

                while ((wait_result = ::sem_wait(&m_semaphore)) == EINTR) {
                    continue;
                }

                WN_RELEASE_ASSERT_DESC(wait_result == 0, __WN_SEMAPHORE_WAIT_ERR_MSG);
            #else
                std::unique_lock<spin_lock> lock(m_spin_lock);

                m_condition_variable.wait(lock, [this]() -> wn_bool { return(m_count > 0); });

                --m_count;
            #endif
        }

        WN_FORCE_INLINE wn_bool try_wait() {
            #ifdef _WN_WINDOWS
                return(::WaitForSingleObject(m_handle, 0) != WAIT_TIMEOUT ? wn_true : wn_false);
            #else
                const std::lock_guard<spin_lock> guard(m_spin_lock);

                #ifdef _WN_POSIX
                    int try_wait_result;

                    while ((try_wait_result = ::sem_trywait(&m_semaphore)) == EINTR) {
                        continue;   
                    }

                    return(try_wait_result == 0 ? wn_true : wn_false);
                #else
                    if (m_count > 0)
                    {
                        --m_count;

                        return(wn_true);
                    }

                    return(wn_false);
                #endif
            #endif
        }

        WN_FORCE_INLINE wn_void notify() {
            #ifdef _WN_WINDOWS
                const BOOL release_result = ::ReleaseSemaphore(m_handle, 1, NULL);

                WN_RELEASE_ASSERT_DESC(release_result != FALSE, __WN_SEMAPHORE_POST_ERR_MSG);
            #else 
                const std::lock_guard<spin_lock> guard(m_spin_lock);

                #ifdef _WN_POSIX
                    const int release_result = ::sem_post(&m_semaphore);

                    WN_RELEASE_ASSERT_DESC(release_result == 0, __WN_SEMAPHORE_POST_ERR_MSG);
                #else
                    ++m_count;

                    m_condition_variable.notify_one();
                #endif
            #endif
        }

        WN_FORCE_INLINE wn_void notify(const wn_uint16 _count) {
            WN_DEBUG_ASSERT_DESC(_count != 0, __WN_SEMAPHORE_POST_WARN_MSG);

            if (_count > 0) {
                #ifdef _WN_WINDOWS
                    const BOOL release_result = ::ReleaseSemaphore(m_handle, static_cast<LONG>(_count), NULL);

                    WN_RELEASE_ASSERT_DESC(release_result != FALSE, __WN_SEMAPHORE_POST_ERR_MSG);
                #else
                    const std::lock_guard<spin_lock> guard(m_spin_lock);

                    #ifdef _WN_POSIX
                        for (wn_uint16 i = 0; i < _count; ++i) {
                            const int release_result = ::sem_post(&m_semaphore);

                            WN_RELEASE_ASSERT_DESC(release_result == 0, __WN_SEMAPHORE_POST_ERR_MSG);
                        }
                    #else
                        m_count += _count;

                        for (wn_uint16 i = 0; i < _count; ++i) {
                            m_condition_variable.notify_one();
                        }
                    #endif
                #endif
            }
        }

    private:
        #ifdef _WN_WINDOWS
            HANDLE m_handle;
        #else
            spin_lock m_spin_lock;

            #ifdef _WN_POSIX
                sem_t m_semaphore;
            #else
                condition_variable m_condition_variable;
                wn_uint16 m_count;
            #endif
        #endif
    };
}

#endif // __WN_CONCURRENCY_SEMAPHORE_H__
