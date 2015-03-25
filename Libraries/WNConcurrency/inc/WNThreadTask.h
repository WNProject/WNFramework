// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_THREAD_TASK_H__
#define __WN_CONCURRENCY_THREAD_TASK_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNIntrusivePtrBase.h"
#include "WNConcurrency/inc/WNSemaphore.h"

#include <atomic>

namespace wn {
    class thread_pool;

    class thread_task : public memory::intrusive_ptr_base {
    public:
        WN_FORCE_INLINE explicit thread_task() :
            memory::intrusive_ptr_base() {
            m_executed.clear(std::memory_order_release);
        }

        WN_FORCE_INLINE virtual ~thread_task() = default;

        WN_FORCE_INLINE wn_void join() {
            if (!run_task()) {
                m_completion_semaphore.wait();
                m_completion_semaphore.notify();
            }
        }

    protected:
        virtual wn_void run() = 0;

    private:
        friend class thread_pool;

        WN_FORCE_INLINE wn_bool run_task() {
            if (!m_executed.test_and_set(std::memory_order_acquire)) {
                run();

                m_completion_semaphore.notify();

                return(wn_true);
            }

            return(wn_false);
        }

        std::atomic_flag m_executed;
        semaphore m_completion_semaphore;
    };
}

#endif // __WN_CONCURRENCY_THREAD_TASK_H__
