// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_THREAD_POOL_H__
#define __WN_MULTI_TASKING_THREAD_POOL_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNMemory/inc/intrusive_ptr.h"
#include "WNMultiTasking/inc/semaphore.h"
#include "WNMultiTasking/inc/spin_lock.h"
#include "WNMultiTasking/inc/thread.h"
#include "WNMultiTasking/inc/thread_task.h"
#include "core/inc/utilities.h"

#ifndef _WN_WINDOWS
#include "WNContainers/inc/WNDeque.h"
#endif

namespace wn {
namespace multi_tasking {

class thread_pool final : public core::non_copyable {
public:
  enum class result {
#include "core/inc/internal/errors.inc"
  };

  typedef result result_type;

  inline thread_pool(memory::allocator* _allocator)
    : m_allocator(_allocator),
      m_threads(_allocator),
#ifdef _WN_POSIX
      m_tasks(_allocator),
#endif
      m_shutdown(true) {
  }

  inline ~thread_pool() {
    cleanup();
  }

  inline result_type initialize(const uint32_t _worker_count) {
    if (m_shutdown == false) {
      return result::already_initialized;
    }

#ifdef _WN_WINDOWS
    const HANDLE io_completion_port =
        ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);

    if (io_completion_port == INVALID_HANDLE_VALUE) {
      return result::error;
    }

    m_io_completion_port = io_completion_port;
#endif

    {
      const spin_lock_guard guard(m_thread_mutex);

      for (uint32_t i = 0; i < _worker_count; ++i) {
        thread* worker = m_allocator->construct<thread>(
            m_allocator, &thread_pool::worker_thread, this);

        m_threads.push_back(worker);
        m_worker_start_mutex.wait();
      }
    }

    m_shutdown = false;

    return result::ok;
  }

  // This forces one thread that is waiting for a job to
  // wake up. This many cancel any thread that is currently
  // running.
  inline void wake_waiting_thread() {
#ifdef _WN_WINDOWS
    ::PostQueuedCompletionStatus(m_io_completion_port.value(), 0, 0, 0);
#else
    {
      const spin_lock_guard guard(m_task_lock);

      m_tasks.push_back(thread_task_ptr());
    }

    m_task_available_semaphore.notify();
#endif
  }

  inline result_type enqueue(thread_task_ptr&& task) {
    if (!task || m_shutdown) {
      return result::invalid_parameters;
    }

#ifdef _WN_WINDOWS
    thread_task_ptr* task_ptr =
        m_allocator->construct<thread_task_ptr>(core::move(task));

    if (!::PostQueuedCompletionStatus(m_io_completion_port.value(), 0,
            reinterpret_cast<ULONG_PTR>(task_ptr), 0)) {
      m_allocator->destroy(task_ptr);

      return result::uninitialized;
    }
#else
    {
      const spin_lock_guard guard(m_task_lock);

      m_tasks.push_back(std::move(task));
    }

    m_task_available_semaphore.notify();
#endif

    return result::ok;
  }

  inline result_type enqueue(const thread_task_ptr& task) {
    return enqueue(thread_task_ptr(task));
  }

#ifdef _WN_WINDOWS
  bool process_single_task() {
    LPOVERLAPPED overlapped = nullptr;
    DWORD bytes_transferred = 0;
    ULONG_PTR completion_key = 0;

    const BOOL completion_status_result =
        ::GetQueuedCompletionStatus(m_io_completion_port.value(),
            &bytes_transferred, &completion_key, &overlapped, INFINITE);

    WN_RELEASE_ASSERT(completion_status_result,
        "failed to get completion status from the queue");

    if (completion_key == 0) {
      switch (bytes_transferred) {
        case 0:
          return false;
        default:
          WN_RELEASE_ASSERT(false, "invalid message received");
          return false;
      }
    } else {
      thread_task_ptr* task =
          reinterpret_cast<thread_task_ptr*>(completion_key);

      (*task)->run_task();

      m_allocator->destroy(task);
    }
    return true;
  }
#else
  bool process_single_task() {
    m_task_available_semaphore.wait();
    m_task_lock.lock();

    thread_task_ptr task = m_tasks.front();

    m_tasks.pop_front();
    m_task_lock.unlock();

    if (!task) {
      return false;
    }

    task->run_task();
    return true;
  }
#endif

private:
  inline void cleanup() {
    if (m_shutdown) {
      return;
    }
    m_shutdown = true;

    {
      const spin_lock_guard guard(m_thread_mutex);

#ifdef _WN_WINDOWS
      for (size_t i = 0; i < m_threads.size(); ++i) {
        ::PostQueuedCompletionStatus(m_io_completion_port.value(), 0, 0, 0);
      }
#else
      for (size_t i = 0; i < m_threads.size(); ++i) {
        {
          const spin_lock_guard guard(m_task_lock);

          m_tasks.push_back(thread_task_ptr());
        }

        m_task_available_semaphore.notify();
      }
#endif

      for (size_t i = 0; i < m_threads.size(); ++i) {
        m_threads[i]->join();

        m_allocator->destroy(m_threads[i]);
      }

      m_threads.clear();
    }

#ifdef _WN_WINDOWS
    m_io_completion_port.dispose();
#else
    m_tasks.clear();
#endif
  }

  void worker_thread() {
    m_worker_start_mutex.notify();
    while (process_single_task())
      ;  // Intentional
  }

private:
  memory::allocator* m_allocator;
  containers::dynamic_array<thread*> m_threads;
#ifdef _WN_WINDOWS
  utilities::internal::handle m_io_completion_port;
#else
  containers::deque<thread_task_ptr> m_tasks;
  semaphore m_task_available_semaphore;
  spin_lock m_task_lock;
#endif
  spin_lock m_thread_mutex;
  semaphore m_worker_start_mutex;
  volatile bool m_shutdown;
};

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_THREAD_POOL_H__
