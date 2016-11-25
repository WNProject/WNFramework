// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_JOB_POOL_H__
#define __WN_MULTI_TASKING_JOB_POOL_H__

#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNFunction.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNList.h"
#include "WNContainers/inc/WNPair.h"
#include "WNCore/inc/WNUtility.h"
#include "WNMultiTasking/inc/WNCallbackTask.h"
#include "WNMultiTasking/inc/WNThread.h"
#include "WNMultiTasking/inc/WNSynchronized.h"
#include "WNMultiTasking/inc/WNThreadPool.h"

namespace wn {
namespace multi_tasking {

class job_pool;

using job = callback_task<void>;
using job_ptr = memory::intrusive_ptr<job>;

template <typename T, typename... Args>
WN_FORCE_INLINE job_ptr make_job(memory::allocator* _allocator,
    job_signal* _signal, T&& func, Args&&... _args) {
  // TODO(awoloszyn) Add more data to job such that we can
  // detect this more easily.
  if (!_signal) {
    return memory::make_intrusive<job>(
        _allocator, std::bind(func, core::forward<Args>(_args)...));
  } else {
    containers::function<void()>* fn =
        _allocator->construct<containers::function<void()>>(
            std::bind(func, core::forward<Args>(_args)...));

    return memory::make_intrusive<job>(_allocator, [_allocator, fn, _signal]() {
      (*fn)();
      _allocator->destroy(fn);
      _signal->increment(1);
    });
  }
}

template <typename T, typename... Args>
WN_FORCE_INLINE
    typename core::enable_if<wn::multi_tasking::is_synchronized<T>::type::value,
        job_ptr>::type
    make_job(memory::allocator* _allocator, job_signal* _signal,
        void (T::*fn)(Args...), T* _c, Args&&... _args) {
  // TODO(awoloszyn) Add more data to job such that we can
  // detect this more easily, and, instead of wrapping the job,
  // put it directly into the sleep state.
  synchronization_data* data = _c->get_synchronization_data();
  containers::function<void()>* func =
      _allocator->construct<containers::function<void()>>(
          std::bind(fn, _c, core::forward<Args>(_args)...));
  size_t wait_value = data->increment_job();
  return memory::make_intrusive<job>(
      _allocator, [_allocator, wait_value, data, func, _signal]() {
        data->signal.wait_until(wait_value);
        (*func)();
        _allocator->destroy(func);
        data->signal.increment(1);
        if (_signal) {
          _signal->increment(1);
        }
      });
}

// T must either be a thread or a fiber.
// Jobs are a bit different than tasks (in WNThreadPool.h).
// Jobs should never use normal threading constructs, they
// should instead use those provided by the job pool.
//
// The scheduling policy for the job pool is simple for now.
// 1) Any job but becomes unblocked is highest priority.
// 2) Any job that has not been started is next.
// 3) Jobs are restarted in the order that they
//    become unblocked.
class job_pool : public core::non_copyable {
public:
  // This exists just to ensure that all subclasses provide a
  // number of threads.
  explicit job_pool(memory::allocator* _allocator, size_t /*_n*/)
    : m_allocator(_allocator) {}
  WN_INLINE void block_job_until(job_signal* _signal, size_t _value) {
    block_job(_signal, _value);
  }

  virtual void add_job(const job_ptr& _ptr) = 0;
  virtual void add_job(job_ptr&& _ptr) = 0;

  // Calls the given blocking function.

  template <typename T, typename... Args>
  T call_blocking_function(
      const containers::function<T(Args...)>& func, Args&&... args) {
    notify_blocking();
    T ret = func(std::forward<Args>(args)...);
    move_to_ready();
    return core::move(ret);
  }

  template <typename... Args>
  void call_blocking_function(
      const containers::function<void(Args...)>& func, Args&&... args) {
    notify_blocking();
    func(std::forward<Args>(args)...);
    move_to_ready();
  }

  // Calls the given blocking function asynchronously. _signal is incremented
  // when the call completes.
  void call_blocking_function_async(
      const containers::function<void()>& func, job_signal* _signal) {
    add_job(wn::multi_tasking::make_job(
        m_allocator, nullptr, [this, func, _signal]() {
          notify_blocking();
          func();
          _signal->increment(1);
          move_to_ready();
        }));
  }

  template <typename T, typename... Args>
  void call_async(job_signal* _signal, T&& t, Args&&... args) {
    add_job(wn::multi_tasking::make_job(
        m_allocator, _signal, t, std::forward<Args>(args)...));
  }

  void update_signal(job_signal* _signal, size_t _num) {
    _signal->value.exchange(_num, std::memory_order::memory_order_release);
    notify_signalled(_signal, _num);
  }

  void increment_signal(job_signal* _signal, size_t _num) {
    size_t new_value = _signal->value.fetch_add(
                           _num, std::memory_order::memory_order_release) +
                       _num;
    notify_signalled(_signal, new_value);
  }

  void decrement_signal(job_signal* _signal, size_t _num) {
    size_t new_value = _signal->value.fetch_sub(
                           _num, std::memory_order::memory_order_release) -
                       _num;
    notify_signalled(_signal, new_value);
  }

  static job_pool* this_job_pool();

protected:
  static void set_this_job_pool(job_pool* _pool);
  memory::allocator* m_allocator;

private:
  // This blocks the current
  virtual void block_job(job_signal* _signal, size_t _value) = 0;
  virtual void notify_signalled(job_signal* _signal, size_t _new_value) = 0;
  virtual void notify_blocking() = 0;
  virtual void move_to_ready() = 0;
  friend class job_signal;
};

struct thread_data {
  thread_data() {}
  ~thread_data() {}
  thread_data(thread_data&& _other)
    : m_current_position(core::move(_other.m_current_position)),
      m_dormant_semaphore(core::move(_other.m_dormant_semaphore)),
      m_blocked_semaphore(core::move(_other.m_blocked_semaphore)) {}
  containers::list<thread_data>::iterator m_current_position;
  // This is kept up to date as the current iterator
  // to this thread.
  semaphore m_dormant_semaphore;  // This is the semaphore to wait
                                  // when we have gone dormant.
  semaphore m_blocked_semaphore;  // This is the semaphore will be
                                  // signaled when we are no longer
                                  // blocked and should continue.
};

// This constructs a job_pool that is backed by threads.
// Any time a thread blocks, a new thread may be spun up to replace it.
// However only at most "N" threads willever be active at any given time,
// even if many more threads have been created to handle blocked threads.
class thread_job_pool : public job_pool {
public:
  thread_job_pool(memory::allocator* _allocator, size_t _n)
    : job_pool(_allocator, _n),
      m_max_active_threads(_n),
      m_shutdown(false),
      m_thread_pool(_allocator),
      m_all_threads(_allocator),
      // Is is important that active/dormant/pending/blocking
      // all use the same allocator. This allows us to transfer
      // elements between lists without incurring any allocation.
      m_active_threads(_allocator),
      m_dormant_threads(_allocator),
      m_pending_threads(_allocator),
      m_blocked_threads(_allocator),
      // This is the list of threads that are currently
      // running a blocking call, they will be made
      // pending when they wake up.
      m_blocking_threads(_allocator),
      m_pending_jobs(0) {
    // We initialize with 0 threads because we handle managing
    // the thread lifetimes ourselves. We just want to use the
    // thread pool for efficiently managing tasks.
    m_thread_pool.initialize(0);
    spin_lock_guard guard(m_thread_lock);
    for (size_t i = 0; i < _n; ++i) {
      m_active_threads.push_back(thread_data());
      (m_active_threads.end() - 1)->m_current_position =
          (m_active_threads.end() - 1);
      m_all_threads.push_back(thread(_allocator, &thread_job_pool::thread_func,
          this, &(*(m_active_threads.end() - 1))));
    }
  }

  ~thread_job_pool() {
    join();
  }

  void join() {
    {
      spin_lock_guard guard(m_thread_lock);
      if (m_shutdown) {
        return;
      }
      m_shutdown = true;
    }
    for (size_t i = 0; i < m_max_active_threads; ++i) {
      m_thread_pool.wake_waiting_thread();
      m_shutdown_semaphore.wait();
    }

    for (auto& thread : m_dormant_threads) {
      m_thread_pool.wake_waiting_thread();
      thread.m_dormant_semaphore.notify();
    }

    for (auto& thread : m_all_threads) {
      thread.join();
    }
  }

  void add_job(const job_ptr& _ptr) override {
    spin_lock_guard guard(m_thread_lock);
    m_pending_jobs += 1;
    m_thread_pool.enqueue(_ptr);
  }
  void add_job(job_ptr&& _ptr) override {
    spin_lock_guard guard(m_thread_lock);
    m_pending_jobs += 1;
    m_thread_pool.enqueue(core::move(_ptr));
  }

private:
  static thread_data* this_thread();
  static void set_this_thread(thread_data*);

  void block_job(job_signal* _signal, size_t _value) override {
    thread_data* self_thread = this_thread();
    {
      spin_lock_guard guard(m_thread_lock);
      if (_signal->value == _value) {
        return;
      }
      auto list = m_blocked_threads.insert(

          containers::make_pair(containers::make_pair(_signal, _value),
              containers::list<thread_data>(m_allocator)));

      m_active_threads.transfer_to(self_thread->m_current_position,
          list.first->second.end(), list.first->second);

      (list.first->second.end() - 1)->m_current_position =
          (list.first->second.end() - 1);

      if (!m_dormant_threads.empty()) {
        m_dormant_threads.transfer_to(m_dormant_threads.begin(),
            m_active_threads.end(), m_active_threads);

        (m_active_threads.end() - 1)->m_current_position =
            (m_active_threads.end() - 1);
        (m_active_threads.end() - 1)->m_dormant_semaphore.notify();
      } else {
        m_active_threads.push_back(thread_data());
        (m_active_threads.end() - 1)->m_current_position =
            (m_active_threads.end() - 1);

        m_all_threads.push_back(
            thread(m_allocator, &thread_job_pool::thread_func, this,
                &(*(m_active_threads.end() - 1))));
      }
    }
    self_thread->m_blocked_semaphore.wait();
    // Make sure that memory is coherent again.
    _signal->value.load(std::memory_order_acquire);
  }

  void notify_blocking() override {
    {
      spin_lock_guard guard(m_thread_lock);
      if (!m_dormant_threads.empty()) {
        m_dormant_threads.transfer_to(m_dormant_threads.begin(),
            m_active_threads.end(), m_active_threads);

        (m_active_threads.end() - 1)->m_current_position =
            (m_active_threads.end() - 1);
        (m_active_threads.end() - 1)->m_dormant_semaphore.notify();
      } else {
        m_active_threads.push_back(thread_data());
        (m_active_threads.end() - 1)->m_current_position =
            (m_active_threads.end() - 1);

        m_all_threads.push_back(
            thread(m_allocator, &thread_job_pool::thread_func, this,
                &(*(m_active_threads.end() - 1))));
      }
    }
  }

  void move_to_ready() override {
    {
      thread_data* self_thread = this_thread();
      {
        spin_lock_guard guard(m_thread_lock);
        m_active_threads.transfer_to(self_thread->m_current_position,
            m_pending_threads.end(), m_pending_threads);
        (m_pending_threads.end() - 1)->m_current_position =
            m_pending_threads.end() - 1;
        m_thread_pool.wake_waiting_thread();
      }
      self_thread->m_blocked_semaphore.wait();
    }
  }

  void notify_signalled(job_signal* _signal, size_t _value) override {
    size_t num_to_wake = 0;
    {
      spin_lock_guard guard(m_thread_lock);

      auto it = m_blocked_threads.find(containers::make_pair(_signal, _value));
      if (it != m_blocked_threads.end()) {
        num_to_wake = it->second.size();
        auto it2 = it->second.begin();
        while (it2 != it->second.end()) {
          it2 = it->second.transfer_to(
              it2, m_pending_threads.end(), m_pending_threads);
          (m_pending_threads.end() - 1)->m_current_position =
              m_pending_threads.end() - 1;
        }
        m_blocked_threads.erase(it);
      }
    }

    for (size_t i = 0; i < num_to_wake; ++i) {
      m_thread_pool.wake_waiting_thread();
    }
  }

  static void thread_func(thread_job_pool* _pool, thread_data* _data) {
    return _pool->thread_function(_data);
  }

  void thread_function(thread_data* _data) {
    set_this_job_pool(this);
    set_this_thread(_data);
    for (;;) {
      if (!m_thread_pool.process_single_task()) {
        // We were kicked from processing a single task.
        // This means one of 2 things.
        //  a) we have been asked to shut-down or
        //  b) we have been asked to make ourselves dormant and
        //     wake up the next pending thread.
        if (m_shutdown && !m_pending_jobs) {
          m_shutdown_semaphore.notify();
          return;
        }

        // Time to go dormant.
        containers::list<thread_data>::iterator last_active_thread;
        {
          spin_lock_guard guard(m_thread_lock);
          if (m_pending_threads.size() == 0) {
            continue;
          }
          m_active_threads.transfer_to(_data->m_current_position,
              m_dormant_threads.end(), m_dormant_threads);
          _data->m_current_position = m_dormant_threads.end() - 1;

          m_pending_threads.transfer_to(
              m_pending_threads.begin()->m_current_position,
              m_active_threads.end(), m_active_threads);
          last_active_thread = m_active_threads.end() - 1;
        }
        last_active_thread->m_current_position = last_active_thread;
        last_active_thread->m_blocked_semaphore.notify();
        _data->m_dormant_semaphore.wait();
        // We just got woken up from begin dormant. This means we are NOT
        // just finishing a job.
        continue;
      }

      {
        spin_lock_guard guard(m_thread_lock);

        --m_pending_jobs;
        if (m_shutdown && m_pending_jobs == 0) {
          for (size_t i = 0; i < m_active_threads.size(); ++i) {
            m_thread_pool.wake_waiting_thread();
          }
        }
      }
    }
  }

  const size_t m_max_active_threads;
  thread_pool m_thread_pool;
  std::atomic<bool> m_shutdown;
  spin_lock m_thread_lock;
  size_t m_total_threads;
  containers::deque<thread> m_all_threads;
  containers::list<thread_data> m_active_threads;
  containers::list<thread_data> m_dormant_threads;
  containers::list<thread_data> m_pending_threads;
  containers::list<thread_data> m_blocking_threads;
  semaphore m_shutdown_semaphore;
  size_t m_pending_jobs;
  // Because we only ever transfer ownership of the thread
  // data, we can know for sure that the pointers never move.

  struct hasher {
    size_t operator()(const containers::pair<job_signal*, size_t>& _val) const {
      return ::std::hash<const job_signal*>()(_val.first) ^
             ::std::hash<size_t>()(_val.second);
    }
  };

  struct equal {
    size_t operator()(const containers::pair<job_signal*, size_t>& _first,
        const containers::pair<job_signal*, size_t>& _second) const {
      return _first.first == _second.first && _first.second == _second.second;
    }
  };

  containers::hash_map<containers::pair<job_signal*, size_t>,
      containers::list<thread_data>, hasher, equal>
      m_blocked_threads;
};

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_JOB_POOL_H__
