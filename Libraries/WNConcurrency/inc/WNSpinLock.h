// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_SPIN_LOCK_H__
#define __WN_CONCURRENCY_SPIN_LOCK_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNUtility.h"
#include "WNConcurrency/inc/WNConfig.h"

#ifdef _WN_WINDOWS
    #include <windows.h>
#elif defined _WN_LINUX
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/syscall.h>
#endif

#ifdef __WN_PTHREAD_SPIN_LOCKS
    #include <pthread.h>
    #include <errno.h>

    #define __WN_SPIN_LOCK_CREATE_ERR_MSG "Error: Failed to create spin lock object."
    #define __WN_SPIN_LOCK_LOCK_ERR_MSG "Error: Failed to lock spin lock object."
    #define __WN_SPIN_LOCK_UNLOCK_ERR_MSG "Error: Failed to unlock spin lock object."
    #define __WN_SPIN_LOCK_DESTROY_WARN_MSG "Warning: Failed to destroy spin lock object."
#else
    #include <atomic>
#endif

namespace wn {
    class spin_lock final : public core::non_copyable {
    public:
        WN_FORCE_INLINE explicit spin_lock() {
            #ifdef __WN_PTHREAD_SPIN_LOCKS
                const int result_type = ::pthread_spin_init(&m_spin_lock, PTHREAD_PROCESS_PRIVATE);

                WN_RELEASE_ASSERT_DESC(result_type == 0, __WN_SPIN_LOCK_CREATE_ERR_MSG);
            #else
                m_flag.clear(std::memory_order_release);
            #endif
        }

        WN_FORCE_INLINE ~spin_lock() {
            #ifdef __WN_PTHREAD_SPIN_LOCKS
                const int result_type = ::pthread_spin_destroy(&m_spin_lock);

                WN_DEBUG_ASSERT_DESC(result_type == 0, __WN_SPIN_LOCK_DESTROY_WARN_MSG);

                #ifdef _WN_RELEASE
                    WN_UNUSED_ARGUMENT(result_type);
                #endif
            #endif
        }

        WN_FORCE_INLINE wn_void lock() {
            #ifdef __WN_PTHREAD_SPIN_LOCKS
                const int result_type = ::pthread_spin_lock(&m_spin_lock);

                WN_RELEASE_ASSERT_DESC(result_type == 0, __WN_SPIN_LOCK_LOCK_ERR_MSG);
            #else
                while (!try_lock()) {
                    continue;
                }
            #endif
        }

        WN_FORCE_INLINE wn_bool try_lock() {
            #ifdef __WN_PTHREAD_SPIN_LOCKS
                return(::pthread_spin_trylock(&m_spin_lock) != EBUSY ? wn_true : wn_false);
            #else
                return(!m_flag.test_and_set(std::memory_order_acquire));
            #endif
        }

        WN_FORCE_INLINE wn_void unlock() {
            #ifdef __WN_PTHREAD_SPIN_LOCKS
                const int result_type = ::pthread_spin_unlock(&m_spin_lock);

                WN_RELEASE_ASSERT_DESC(result_type == 0, __WN_SPIN_LOCK_UNLOCK_ERR_MSG);
            #else
                m_flag.clear(std::memory_order_release);
            #endif
        }

    private:
        #ifdef __WN_PTHREAD_SPIN_LOCKS
            pthread_spinlock_t m_spin_lock;
        #else
            std::atomic_flag m_flag;
        #endif
    };

    class recursive_spin_lock final : public core::non_copyable {
    public:
        WN_FORCE_INLINE explicit recursive_spin_lock() :
            m_recursion_count(0),
            m_owning_thread(0) {
            #ifdef __WN_PTHREAD_SPIN_LOCKS
                const int result_type = ::pthread_spin_init(&m_spin_lock, PTHREAD_PROCESS_PRIVATE);

                WN_RELEASE_ASSERT_DESC(result_type == 0, __WN_SPIN_LOCK_CREATE_ERR_MSG);
            #else
                m_flag.clear(std::memory_order_release);
            #endif
        }

        WN_FORCE_INLINE ~recursive_spin_lock() {
            #ifdef __WN_PTHREAD_SPIN_LOCKS
                const int result_type = ::pthread_spin_destroy(&m_spin_lock);

                WN_DEBUG_ASSERT_DESC(result_type == 0, __WN_SPIN_LOCK_DESTROY_WARN_MSG);

                #ifdef _WN_RELEASE
                    WN_UNUSED_ARGUMENT(result_type);
                #endif
            #endif
        }

        WN_FORCE_INLINE wn_void lock() {
            #ifdef _WN_WINDOWS
                const DWORD current_thread = ::GetCurrentThreadId();
            #elif defined _WN_LINUX
                const pid_t current_thread = static_cast<pid_t>(::syscall(SYS_gettid));
            #endif

            if (current_thread != m_owning_thread) {
                #ifdef __WN_PTHREAD_SPIN_LOCKS
                    const int result_type = ::pthread_spin_lock(&m_spin_lock);

                    WN_RELEASE_ASSERT_DESC(result_type == 0, __WN_SPIN_LOCK_LOCK_ERR_MSG);
                #else
                    while (m_flag.test_and_set(std::memory_order_acquire)) {
                        continue;
                    }
                #endif
            }

            m_owning_thread = current_thread;
            m_recursion_count++;
        }

        WN_FORCE_INLINE wn_bool try_lock() {
            #ifdef _WN_WINDOWS
                const DWORD current_thread = ::GetCurrentThreadId();
            #elif defined _WN_LINUX
                const pid_t current_thread = static_cast<pid_t>(::syscall(SYS_gettid));
            #endif

            if (m_owning_thread != current_thread) {
                #ifdef __WN_PTHREAD_SPIN_LOCKS
                    if (::pthread_spin_trylock(&m_spin_lock) == EBUSY) {
                        return(wn_false);
                    }
                #else
                    if (m_flag.test_and_set(std::memory_order_acquire)) {
                        return(wn_false);
                    }
                #endif

                m_owning_thread = current_thread;
            }

            m_recursion_count++;

            return(wn_true);
        }

        WN_FORCE_INLINE wn_void unlock() {
            const wn_size_t recursion_count = --m_recursion_count;

            if (recursion_count == 0) {
                m_owning_thread = 0;

                #ifdef __WN_PTHREAD_SPIN_LOCKS
                    const int result_type = ::pthread_spin_unlock(&m_spin_lock);

                    WN_RELEASE_ASSERT_DESC(result_type == 0, __WN_SPIN_LOCK_UNLOCK_ERR_MSG);
                #else
                    m_flag.clear(std::memory_order_release);
                #endif
            }
        }

    private:
        #ifdef _WN_WINDOWS
            DWORD m_owning_thread;
        #elif defined _WN_LINUX
            pid_t m_owning_thread;
        #endif

        wn_size_t m_recursion_count;

        #ifdef __WN_PTHREAD_SPIN_LOCKS
            pthread_spinlock_t m_spin_lock;
        #else
            std::atomic_flag m_flag;
        #endif
    };
}

#endif // __WN_CONCURRENCY_SPIN_LOCK_H__
