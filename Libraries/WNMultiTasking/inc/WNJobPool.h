// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_JOB_POOL_H__
#define __WN_MULTI_TASKING_JOB_POOL_H__

#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNList.h"
#include "WNCore/inc/WNPair.h"
#include "WNCore/inc/WNUtility.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNFunctional/inc/WNUniqueFunction.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNMultiTasking/inc/WNThread.h"
#include "WNMultiTasking/inc/WNSynchronized.h"
#include "WNMultiTasking/inc/WNThreadPool.h"

namespace wn {
namespace multi_tasking {

class job_pool;

class job {
public:
  virtual ~job() = default;
  void run() {
    run_internal();
    if (m_done_signal) {
      m_done_signal->increment(1);
    }
    if (m_wait_signal) {
      m_wait_signal->increment(1);
    }
  }

  job_signal* get_wait_signal() const {
    return m_wait_signal;
  }

  void mark_ready() {
    m_ready = true;
  }

  bool is_ready() const {
    return m_ready;
  }

  size_t get_wait_condition() const {
    return m_wait_count;
  }

  WN_FORCE_INLINE job(job_signal* _wait_signal, size_t _wait_count,
      job_signal* _done_signal, bool _ready = false)
    : m_wait_signal(_wait_signal),
      m_wait_count(_wait_count),
      m_done_signal(_done_signal),
      m_ready(_ready) {}

private:
  virtual void run_internal() = 0;
  job_signal* m_wait_signal;
  size_t m_wait_count;
  job_signal* m_done_signal;
  bool m_ready;

  friend class job_pool;
};

template <typename C, typename T>
class synchronized_job : public job {
public:
  template <typename... Args>
  WN_FORCE_INLINE synchronized_job(job_signal* _wait_signal, size_t _wait_count,
      job_signal* _done_signal, C* _class, void (C::*_function)(T),
      Args&&... _args)
    : job(_wait_signal, _wait_count, _done_signal, _wait_signal == nullptr),
      m_c(_class),
      m_function(_function),
      m_t{core::forward<Args>(_args)...} {}

private:
  void run_internal() override {
    (m_c->*m_function)(core::forward<T>(m_t));
  }

  C* m_c;
  typename core::remove_reference<T>::type m_t;
  void (C::*m_function)(T);
};

class unsynchronized_job : public job {
public:
  WN_FORCE_INLINE unsynchronized_job(
      job_signal* _done_signal, functional::function<void()> _function)
    : job(nullptr, 0, _done_signal, true), m_function(_function) {}

private:
  void run_internal() override {
    m_function();
  }
  functional::function<void()> m_function;
};

template <typename T>
class callback {
public:
  virtual ~callback() = default;
  virtual void enqueue_job(job_pool* pool, T&& t) = 0;
  virtual memory::unique_ptr<callback<T>> clone(memory::allocator*) = 0;
};

template <typename T, typename C>
class synchronized_callback : public callback<T> {
public:
  synchronized_callback(C* _c, void (C::*_func)(T))
    : m_c(_c), m_function(_func) {}
  virtual void enqueue_job(job_pool* pool, T&& t);

  virtual memory::unique_ptr<callback<T>> clone(memory::allocator* _allocator) {
    return memory::make_unique<synchronized_callback<T, C>>(
        _allocator, m_c, m_function);
  }

private:
  C* m_c;
  void (C::*m_function)(T);
};

template <typename T>
class unsynchronized_callback : public callback<T> {
public:
  unsynchronized_callback(const functional::function<void(T)>& _func)
    : m_function(_func) {}
  virtual void enqueue_job(job_pool* pool, T&& t);
  virtual memory::unique_ptr<callback<T>> clone(memory::allocator* _allocator) {
    return memory::make_unique<unsynchronized_callback<T>>(
        _allocator, m_function);
  }

private:
  functional::function<void(T)> m_function;
};

using job_ptr = memory::unique_ptr<job>;
template <typename T>
using callback_ptr = memory::unique_ptr<callback<T>>;

class job_task : public thread_task {
public:
  job_task(job_ptr _ptr) : m_ptr(core::move(_ptr)) {}
  void run() override {
    m_ptr->run();
  }

private:
  job_ptr m_ptr;
};

template <typename T>
typename core::enable_if_t<std::is_move_constructible<T>::value, T&&>
move_if_movable(T& t) {
  return core::move(t);
}

template <typename T>
typename core::enable_if_t<!std::is_move_constructible<T>::value, T>
move_if_movable(T& t) {
  return t;
}

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

  // Calls the given blocking function.

  template <typename T, typename Callable, typename... Args>
  T call_blocking_function(Callable func, Args&&... args) {
    notify_blocking();
    T ret = std::bind(func, core::forward<Args>(args)...)();
    move_to_ready();
    return core::move(ret);
  }

  template <typename Callable, typename... Args>
  void call_blocking_function(Callable func, Args&&... args) {
    notify_blocking();
    std::bind(func, core::forward<Args>(args)...)();
    move_to_ready();
  }

  template <typename T, typename... Args>
  WN_FORCE_INLINE typename core::enable_if<
      !core::disjunction<core::is_lvalue_reference<Args>...>::value, bool>::type
  add_unsynchronized_job(job_signal* _signal, T&& _func, Args... _args) {
    add_job_internal(memory::make_unique<unsynchronized_job>(m_allocator,
        _signal, std::bind<void>(_func, move_if_movable<Args>(_args)...)));
    return true;  // This is because of a bug in VS2013.
                  // Remove and switch to void return once that is done
  }

  template <typename T, typename V, typename... Args>
  WN_FORCE_INLINE
      typename core::enable_if<multi_tasking::is_synchronized<T>::type::value,
          void>::type
      add_job(job_signal* _signal, void (T::*_fn)(V), T* _c, Args&&... _args) {
    synchronization_data* data = _c->get_synchronization_data();
    add_job_internal(memory::make_unique<synchronized_job<T, V>>(m_allocator,
        &data->signal, data->increment_job(), _signal, _c, _fn,
        core::forward<Args>(_args)...));
  }

  template <typename T, typename V, typename... Args>
  WN_FORCE_INLINE
      typename core::enable_if<!multi_tasking::is_synchronized<T>::type::value,
          void>::type
      add_job(job_signal* _signal, void (T::*_fn)(V), T* _c, Args&&... _args) {
    add_job_internal(memory::make_unique<synchronized_job<T, V>>(m_allocator,
        nullptr, 0, _signal, _c, _fn, core::forward<Args>(_args)...));
  }

  template <typename T>
  WN_FORCE_INLINE void destroy_synchronized(memory::unique_ptr<T> object) {
    synchronization_data* data = object->get_synchronization_data();
    add_job_internal(
        memory::make_unique<synchronized_job<job_pool, memory::unique_ptr<T>&>>(
            m_allocator, &data->signal, data->increment_job(), nullptr, this,
            &job_pool::destroy_synchronized_internal<T>, core::move(object)));
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
  template <typename T>
  void destroy_synchronized_internal(memory::unique_ptr<T>&) {}

  static void set_this_job_pool(job_pool* _pool);
  memory::allocator* m_allocator;

  virtual void add_job_internal(job_ptr _ptr) = 0;

private:
  // This blocks the current
  virtual void block_job(job_signal* _signal, size_t _value) = 0;
  virtual void notify_signalled(job_signal* _signal, size_t _new_value) = 0;
  virtual void notify_blocking() = 0;
  virtual void move_to_ready() = 0;
  friend class job_signal;
};

template <typename T, typename C>
void synchronized_callback<T, C>::enqueue_job(job_pool* pool, T&& t) {
  pool->add_job(nullptr, m_function, m_c, core::forward<T>(t));
}

template <typename T>
void unsynchronized_callback<T>::enqueue_job(job_pool* pool, T&& t) {
  pool->add_unsynchronized_job(nullptr, m_function, core::forward<T>(t));
}

template <typename T, typename C>
WN_FORCE_INLINE memory::unique_ptr<synchronized_callback<T, C>> make_callback(
    memory::allocator* _allocator, C* m_c, void (C::*_function)(T)) {
  return memory::make_unique<synchronized_callback<T, C>>(
      _allocator, m_c, _function);
}

template <typename T>
WN_FORCE_INLINE memory::unique_ptr<unsynchronized_callback<T>>
make_unsynchronized_callback(memory::allocator* _allocator,
    const functional::function<void(T)>& _function) {
  return memory::make_unique<unsynchronized_callback<T>>(_allocator, _function);
}

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
      m_preblocked_jobs(_allocator),
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

protected:
  void add_job_internal(job_ptr _ptr) override {
    spin_lock_guard guard(m_thread_lock);
    if (!_ptr->is_ready() &&
        _ptr->get_wait_signal()->current_value() !=
            _ptr->get_wait_condition()) {
      WN_DEBUG_ASSERT_DESC(_ptr->get_wait_signal(),
          "Cannot have an unready job without a wait signal");
      job_signal* signal = _ptr->get_wait_signal();
      size_t wait_condition = _ptr->get_wait_condition();
      if (m_preblocked_jobs.find(core::make_pair(signal, wait_condition)) ==
          m_preblocked_jobs.end()) {
        m_preblocked_jobs.insert(
            core::make_pair(core::make_pair(signal, wait_condition),
                containers::deque<job_ptr>(m_allocator)));
      }
      m_preblocked_jobs[core::make_pair(signal, wait_condition)].emplace_back(
          core::move(_ptr));
      m_pending_jobs += 1;
    } else {
      // If we have a wait signal but we are marked ready,
      // meaning we fell through the loop above, then add a pending
      // job right now.
      if (!_ptr->get_wait_signal() || !_ptr->is_ready()) {
        m_pending_jobs += 1;
      }
      _ptr->mark_ready();
      m_thread_pool.enqueue(
          memory::make_intrusive<job_task>(m_allocator, core::move(_ptr)));
    }
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

          core::make_pair(core::make_pair(_signal, _value),
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
    containers::deque<job_ptr> start_jobs;
    {
      spin_lock_guard guard(m_thread_lock);
      auto it = m_blocked_threads.find(core::make_pair(_signal, _value));
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
      auto it2 = m_preblocked_jobs.find(core::make_pair(_signal, _value));
      if (it2 != m_preblocked_jobs.end()) {
        start_jobs = core::move(it2->second);
      }
    }

    for (size_t i = 0; i < num_to_wake; ++i) {
      m_thread_pool.wake_waiting_thread();
    }
    for (auto&& job : start_jobs) {
      job->mark_ready();
      add_job_internal(core::move(job));
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
    size_t operator()(const core::pair<job_signal*, size_t>& _val) const {
      return ::std::hash<const job_signal*>()(_val.first) ^
             ::std::hash<size_t>()(_val.second);
    }
  };

  struct equal {
    size_t operator()(const core::pair<job_signal*, size_t>& _first,
        const core::pair<job_signal*, size_t>& _second) const {
      return _first.first == _second.first && _first.second == _second.second;
    }
  };

  containers::hash_map<core::pair<job_signal*, size_t>,
      containers::list<thread_data>, hasher, equal>
      m_blocked_threads;
  containers::hash_map<core::pair<job_signal*, size_t>,
      containers::deque<job_ptr>, hasher, equal>
      m_preblocked_jobs;
};

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_JOB_POOL_H__
