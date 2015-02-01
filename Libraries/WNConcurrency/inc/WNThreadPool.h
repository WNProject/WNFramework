// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_THREAD_POOL_H__
#define __WN_CONCURRENCY_THREAD_POOL_H__

#include "WNCore/inc/WNUtility.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNConcurrency/inc/WNThreadTask.h"
#include "WNConcurrency/inc/WNSpinLock.h"
#include "WNConcurrency/inc/WNSemaphore.h"
#include "WNConcurrency/inc/WNThread.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)
    #pragma warning(disable: 4996)
#endif

#include <vector>

#ifndef _WN_WINDOWS
    #include <queue>
#endif

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

namespace wn {
    class thread_pool final : public non_copyable {
    public:
        enum class result {
            #include "WNCore/inc/Internal/WNErrors.inc"
        };

        typedef result result_type;

        WN_FORCE_INLINE explicit thread_pool() :
            #ifdef _WN_WINDOWS
                m_io_completion_port(INVALID_HANDLE_VALUE),
            #endif
            m_shutdown(wn_true) {
        }

        WN_FORCE_INLINE ~thread_pool() {
            cleanup();
        }

        WN_INLINE result_type initialize(const wn_uint32 _worker_count) {
            if (m_shutdown == wn_false) {
                return(result::already_initialized);
            }

            #ifdef _WN_WINDOWS
                const HANDLE io_completion_port = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

                if (io_completion_port == INVALID_HANDLE_VALUE) {
                    return(result::error);
                }

                m_io_completion_port = std::move(io_completion_port);
            #endif

            {
                const std::lock_guard<spin_lock> guard(m_thread_mutex);

                for (wn_uint32 i = 0; i < _worker_count; ++i) {
                    thread<wn_void>* worker = WN_NEW thread<wn_void>(&thread_pool::worker_thread, this);

                    m_threads.push_back(worker);
                    m_worker_start_mutex.wait();
                }
            }

            m_shutdown = wn_false;

            return(result::ok);
        }

        WN_FORCE_INLINE result_type enqueue(intrusive_ptr<thread_task>&& _thread_task) {
            if (!_thread_task || m_shutdown) {
                return(result::invalid_parameters);
            }

            #ifdef _WN_WINDOWS
                intrusive_ptr<thread_task>* task = WN_NEW intrusive_ptr<thread_task>(std::move(_thread_task));

                if (!::PostQueuedCompletionStatus(m_io_completion_port, 0, reinterpret_cast<ULONG_PTR>(task), 0)) {
                    WN_DELETE(task);

                    return(result::uninitialized);
                }
            #else
                {
                    const std::lock_guard<spin_lock> guard(m_task_lock);

                    m_tasks.push_back(std::move(_thread_task));
                }

                m_task_available_semaphore.notify();
            #endif

            return(result::ok);
        }

        WN_FORCE_INLINE result_type enqueue(const intrusive_ptr<thread_task>& _thread_task) {
            return(enqueue(intrusive_ptr<thread_task>(_thread_task)));
        }

    private:
        WN_INLINE wn_void cleanup() {
            m_shutdown = wn_true;

            {
                const std::lock_guard<spin_lock> guard(m_thread_mutex);

                #ifdef _WN_WINDOWS
                    for (wn_size_t i = 0; i < m_threads.size(); ++i) {
                        ::PostQueuedCompletionStatus(m_io_completion_port, 0, NULL, 0);
                    }
                #else
                    for (wn_size_t i = 0; i < m_threads.size(); ++i) {
                        {
                            const std::lock_guard<spin_lock> guard(m_task_lock);

                            m_tasks.push_back(intrusive_ptr<thread_task>());
                        }

                        m_task_available_semaphore.notify();
                    }
                #endif

                for (wn_size_t i = 0; i < m_threads.size(); ++i) {
                    m_threads[i]->join();

                    WN_DELETE(m_threads[i]);
                }

                m_threads.clear();
            }

            #ifdef _WN_WINDOWS
                ::CloseHandle(m_io_completion_port);

                m_io_completion_port = INVALID_HANDLE_VALUE;
            #else
                m_tasks.clear();
            #endif
        }

        wn_void worker_thread() {
            m_worker_start_mutex.notify();

            #ifdef _WN_WINDOWS
                LPOVERLAPPED overlapped = NULL;
                DWORD bytes_transferred = 0;
                ULONG_PTR completion_key = 0;

                for (;;) {
                    const BOOL completion_status_result = ::GetQueuedCompletionStatus(m_io_completion_port,
                                                                                      &bytes_transferred,
                                                                                      &completion_key,
                                                                                      &overlapped,
                                                                                      INFINITE);

                    WN_RELEASE_ASSERT_DESC(completion_status_result, "Failed to get completion status from the queue");

                    if (completion_key == 0) {
                        switch (bytes_transferred) {
                            case 0:
                                return;
                            default:
                                WN_RELEASE_ASSERT_DESC(wn_false, "We got a message from the queue we don't understand");

                                break;
                        }
                    } else {
                        intrusive_ptr<thread_task>* task = reinterpret_cast<intrusive_ptr<thread_task>*>(completion_key);

                        if ((*task)->run_task()) {
                            WN_DELETE(task);
                        }
                    }
               }
            #else
                for (;;) {
                    m_task_available_semaphore.wait();
                    m_task_lock.lock();

                    intrusive_ptr<thread_task> job = m_tasks.front();

                    m_tasks.pop_front();
                    m_task_lock.unlock();

                    if (job == wn_nullptr) {
                        return;
                    }

                    job->run_task();
                }
            #endif
        }

    private:
       #ifdef _WN_WINDOWS
            HANDLE m_io_completion_port;
        #else
            std::deque<intrusive_ptr<thread_task>> m_tasks;
            semaphore m_task_available_semaphore;
            spin_lock m_task_lock;
        #endif

        std::vector<thread<wn_void>*> m_threads;
        spin_lock m_thread_mutex;
        semaphore m_worker_start_mutex;
        volatile wn_bool m_shutdown;
    };
}

#endif // __WN_CONCURRENCY_THREAD_POOL_H__

