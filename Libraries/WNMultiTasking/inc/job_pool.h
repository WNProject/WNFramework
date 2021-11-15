// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MULTI_TASKING_JOB_POOL_H__
#define __WN_MULTI_TASKING_JOB_POOL_H__

#include <atomic>
#include <chrono>
#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNList.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNMultiTasking/inc/condition_variable.h"
#include "WNMultiTasking/inc/semaphore.h"
#include "WNMultiTasking/inc/spin_lock.h"
#include "WNMultiTasking/inc/thread.h"
#include "core/inc/pair.h"
#include "osfiber.h"
namespace wn {
namespace multi_tasking {

class job_pool;

enum class fiber_status {
  initial,
  blocking_on_signal,
  blocked_on_signal,
  moving_to_idle,
  idle,
  active
};

enum class job_priority { standard, low, high };

enum class task_type { job, fiber };

class signal;
struct fiber {
  std::atomic<uint64_t> id;
  marl::Allocator::unique_ptr<marl::OSFiber> fib;
  fiber_status status;
  job_priority priority;
  signal* blocking_signal = nullptr;
  size_t lock_to_thread = ~static_cast<size_t>(0);
};

class signal {
public:
  signal() : m_ref_count(0) {}

  uint64_t get_value() const {
    return m_current_value;
  }
  void wait_until(uint64_t value);
  void increment_by(uint64_t i);

private:
  std::atomic<uint64_t> m_current_value;
  std::atomic<uint64_t> m_next_job_value;
  multi_tasking::spin_lock m_lock;
  containers::deque<core::pair<containers::list<fiber>::list_node*, uint64_t>>
      m_waiting_fibers;
  job_pool* m_pool;
  std::atomic<uint64_t> m_ref_count;
  friend class job_pool;
  friend class signal_ptr;
};

class signal_ptr {
public:
  signal_ptr(signal_ptr&& _other) {
    m_signal = _other.m_signal;
    _other.m_signal = nullptr;
  }
  signal_ptr(nullptr_t) : m_signal(nullptr) {}
  signal_ptr& operator=(signal_ptr&& _other) {
    m_signal = _other.m_signal;
    _other.m_signal = nullptr;
    return *this;
  }

  signal_ptr(const signal_ptr& _other) {
    m_signal = _other.m_signal;
    m_signal->element()->m_ref_count++;
  }
  signal_ptr() {
    m_signal = nullptr;
  }

  ~signal_ptr();

  operator bool() const {
    return m_signal != nullptr;
  }

  uint64_t get_value() const {
    return m_signal->element()->get_value();
  }

  void wait_until(uint64_t value) const {
    return m_signal->element()->wait_until(value);
  }

  void increment_by(uint64_t i) const {
    return m_signal->element()->increment_by(i);
  }

private:
  signal_ptr(containers::list<signal>::list_node* _signal) : m_signal(_signal) {
    m_signal->element()->m_ref_count = 1;
  }

  containers::list<signal>::list_node* m_signal;
  friend class job_pool;
};

#define S1(x) #x
#define S2(x) S1(x)
#define JOB_NAME __FILE__ ":" S2(__LINE__)
class job_pool {
public:
  job_pool(memory::allocator* _allocator, logging::log* _log,
      uint32_t _num_threads, uint64_t stack_size, bool _bind_main_thread);
  ~job_pool();

  void add_job(const char* job_name, functional::function<void()> _function,
      signal_ptr _signal = nullptr,
      job_priority _priority = job_priority::standard);

  template <typename T>
  void add_job(const char* job_name, T t, signal_ptr _signal = nullptr,
      job_priority _priority = job_priority::standard) {
    return add_job(job_name, functional::function<void()>(m_allocator, t),
        _signal, _priority);
  }

  void add_job_on_main(const char* job_name,
      functional::function<void()> _function, signal_ptr _signal = nullptr,
      job_priority priority = job_priority::standard, bool blocking = false);

  void call_blocking_job_on_main(const char* job_name,
      functional::function<void()> _function,
      job_priority priority = job_priority::standard) {
    auto sig = get_signal();
    add_job_on_main(job_name, _function, sig, priority, true);
    sig.wait_until(1);
  }

  template <typename T>
  void add_job_on_main(const char* job_name, T t, signal_ptr _signal = nullptr,
      job_priority _priority = job_priority::standard) {
    return add_job_on_main(job_name,
        functional::function<void()>(m_allocator, t), _signal, _priority);
  }

  void call_blocking_function(const char* job_name,
      functional::function<void()> _function, signal_ptr signal = nullptr);
  template <typename T>
  void call_blocking_function(
      const char* job_name, const T& t, signal_ptr signal = nullptr) {
    return call_blocking_function(
        job_name, functional::function<void()>(m_allocator, t), signal);
  }

  static job_pool* this_job_pool();
  static void set_this_job_pool(job_pool* pool);

  void attach_main_thread();
  void exit_this_thread();

  signal_ptr get_signal();

private:
  enum class job_flag { none, blocking_on_main };

  struct job {
    std::atomic<uint64_t> id;
    functional::function<void()> _fn;
    job_priority priority;
    signal_ptr signal;
    size_t lock_to_thread = ~static_cast<size_t>(0);
    job_flag flag = job_flag::none;
    const char* name;
  };

  struct thread_data {
    void initialize(
        memory::allocator* _allocator, job_pool* _pool, uint64_t thread_id);
    ~thread_data();

    containers::list<job> m_ready_jobs;
    containers::list<fiber> m_ready_fibers;
    std::atomic<size_t> m_num_tasks;

    containers::list<fiber> m_sleeping_fibers;

    containers::list<signal> m_inactive_signals;

    multi_tasking::spin_lock m_job_lock;
    multi_tasking::spin_lock m_signal_lock;

    std::chrono::high_resolution_clock::time_point m_next_sleeping_fiber;

    job_pool* m_pool = nullptr;
    containers::list<fiber>::list_node* m_current_running_fiber;
    containers::list<fiber>::list_node* m_next_running_fiber;
    containers::list<job>::list_node* m_pending_job = nullptr;
    fiber m_initial_fiber;
    bool m_made_idle_fiber = false;
    std::atomic<bool> m_exit = false;
    uint64_t m_thread_id;
  };

  containers::list<fiber>::list_node* get_new_fiber();

  void post_transition_fiber();
  void swap_to(containers::list<fiber>::list_node* _node);
  void spin_up_next_fiber();
  bool run_next_task(bool new_fiber_for_job);
  void run_blocking_thread();

  uint64_t rand_seed =
      std::chrono::high_resolution_clock::now().time_since_epoch().count();
  uint64_t rand() {
    uint64_t s = rand_seed;
    s ^= s >> 12;
    s ^= s << 25;
    s ^= s >> 27;
    rand_seed = s;
    return s * 0x2545F4914F6CDD1DULL;
  }

  void find_fiber();

  void cycle_for_work();
  void initialize_thread_for_work();
  static thread_data*& this_thread_data();
  memory::allocator* m_allocator;
  logging::log* m_log;
  memory::unique_ptr<marl::Allocator> m_fiber_allocator;
  bool m_bind_main_thread;

  multi_tasking::spin_lock m_idle_fiber_lock;
  containers::list<fiber> m_idle_fibers;
  multi_tasking::spin_lock m_idle_job_lock;
  containers::list<job> m_idle_jobs;

  containers::list<job> m_low_priority_jobs;
  containers::list<fiber> m_low_priority_fibers;
  multi_tasking::spin_lock m_low_priority_lock;
  std::atomic<size_t> m_num_low_priority_tasks = 0;

  containers::list<job> m_priority_jobs;
  containers::list<fiber> m_priority_fibers;
  std::atomic<size_t> m_num_priority_tasks = 0;
  multi_tasking::spin_lock m_priority_lock;

  containers::list<multi_tasking::thread> m_unused_blocking_threads;
  containers::list<multi_tasking::thread> m_used_blocking_threads;

  containers::dynamic_array<multi_tasking::thread> m_worker_threads;
  containers::dynamic_array<multi_tasking::thread> m_blocking_threads;
  multi_tasking::spin_lock m_blocking_job_lock;
  multi_tasking::semaphore m_blocking_job_sem;
  std::atomic<size_t> m_active_blocking_job_threads = 0;

  struct blocking_job {
    functional::function<void()> function;
    signal_ptr signal;
    const char* name;
  };
  containers::list<blocking_job> m_blocking_jobs;

  // This exists solely to make sure that we can
  // track if there is any actual work to do. This way we
  // can shut down threads that have no useful work.
  std::atomic<uint64_t> m_waiting_work;
  multi_tasking::spin_lock m_waiting_work_lock;
  multi_tasking::condition_variable m_waiting_work_cv;

  std::atomic<uint64_t> m_next_job_id;
  const uint64_t m_stack_size;

  std::atomic<bool> m_exit;
  std::atomic<uint64_t> m_total_jobs;
  multi_tasking::semaphore m_setup_semaphore;
  friend class signal;
  friend class signal_ptr;
  containers::dynamic_array<thread_data> m_threads;
};

inline signal_ptr::~signal_ptr() {
  if (!m_signal) {
    return;
  }
  uint64_t ct = --m_signal->element()->m_ref_count;
  if (ct == 0) {
    WN_DEBUG_ASSERT(m_signal->element()->m_waiting_fibers.empty(),
        "Cannot delete a fiber that is being waited on");
    auto wt = m_signal->element()->m_pool->this_thread_data();
    if (!wt) {
      wt = &m_signal->element()->m_pool->m_threads[0];
    }
    wt->m_signal_lock.lock();
    wt->m_inactive_signals.link_node(wt->m_inactive_signals.end(), m_signal);
    wt->m_signal_lock.unlock();
  }
  m_signal = nullptr;
}

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_JOB_POOL_H__
