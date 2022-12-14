// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMultiTasking/inc/job_pool.h"
#include "WNMultiTasking/inc/lock_guard.h"
#include "WNMultiTasking/inc/thread.h"
#include "marl/memory.h"
#if defined(_WIN32)
#include <intrin.h>
#endif

namespace wn {
namespace multi_tasking {
namespace {

inline void nop() {
#if defined(_WIN32)
  __nop();
#else
  __asm__ __volatile__("nop");
#endif
}

class fiber_allocator : public marl::Allocator {
public:
  fiber_allocator(memory::allocator* _allocator) : m_allocator(_allocator) {}

  marl::Allocation allocate(const marl::Allocation::Request& req) override {
    void* v = m_allocator->aligned_allocate(req.size, req.alignment);
    return marl::Allocation{v, req};
  }

  void free(const marl::Allocation& allocation) override {
    m_allocator->aligned_deallocate(allocation.ptr);
  }

private:
  memory::allocator* m_allocator;
};

}  // namespace

void job_pool::thread_data::initialize(
    memory::allocator* _allocator, job_pool* _pool, uint64_t _thread_id) {
  WN_RELEASE_ASSERT(m_pool == nullptr, "This has been initialized once before");
  m_ready_jobs.set_allocator(_allocator);
  m_ready_fibers.set_allocator(_allocator);
  m_num_tasks = 0;
  m_sleeping_fibers.set_allocator(_allocator);
  m_inactive_signals.set_allocator(_allocator);
  m_next_sleeping_fiber = std::chrono::high_resolution_clock::time_point::max();
  m_pool = _pool;
  m_thread_id = _thread_id;
}

job_pool::thread_data::~thread_data() {
  //
}

job_pool::~job_pool() {
  m_exit = true;
  m_waiting_work_cv.notify_all();
  for (auto& t : m_worker_threads) {
    t.join();
  }

  m_blocking_job_sem.notify(1);
  for (auto& t : m_blocking_threads) {
    t.join();
  }
}

job_pool::job_pool(memory::allocator* _allocator, logging::log* _log,
    uint32_t _num_threads, uint64_t stack_size, bool _bind_main_thread)
  : m_allocator(_allocator),
    m_log(_log),
    m_fiber_allocator(
        memory::make_unique<fiber_allocator>(m_allocator, m_allocator)),
    m_bind_main_thread(_bind_main_thread),
    m_idle_fibers(_allocator),
    m_idle_jobs(_allocator),
    m_low_priority_jobs(_allocator),
    m_low_priority_fibers(_allocator),
    m_priority_jobs(_allocator),
    m_priority_fibers(_allocator),
    m_unused_blocking_threads(_allocator),
    m_used_blocking_threads(_allocator),
    m_worker_threads(_allocator),
    m_blocking_threads(_allocator),
    m_active_blocking_job_threads(0),
    m_blocking_jobs(_allocator),
    m_next_job_id(0),
    m_stack_size(stack_size),
    m_exit(false),
    m_total_jobs(0),
    m_threads(_allocator, _num_threads + _bind_main_thread) {
  m_blocking_threads.push_back(
      multi_tasking::thread(m_allocator, [this]() { run_blocking_thread(); }));

  for (size_t i = 0; i < _num_threads + _bind_main_thread; ++i) {
    m_threads[i].initialize(m_allocator, this, i);
  }

  for (size_t i = 0; i < _num_threads; ++i) {
    m_worker_threads.push_back(multi_tasking::thread(
        _allocator, [this, num = i + _bind_main_thread]() {
          this_thread_data() = &m_threads[num];
          initialize_thread_for_work();
        }));
  }
}

void job_pool::run_blocking_thread() {
  set_this_job_pool(this);
  m_blocking_job_lock.lock();
  while (m_blocking_jobs.size() || !m_exit) {
    m_blocking_job_lock.unlock();
    blocking_job job;
    m_blocking_job_sem.wait();
    m_blocking_job_lock.lock();
    if (m_blocking_jobs.size() > 0) {
      job = core::move(*m_blocking_jobs.begin());
      m_blocking_jobs.erase(m_blocking_jobs.begin());
      m_active_blocking_job_threads++;
    }
    m_blocking_job_lock.unlock();

    if (job.function) {
      m_log->log_info("Staring blocking job: ", job.name);
      m_log->flush();
      job.function();
      m_log->log_info("Ending blocking job: ", job.name);
      m_log->flush();
      if (job.signal) {
        job.signal.increment_by(1);
      }
      m_active_blocking_job_threads--;
    }
    m_blocking_job_lock.lock();
  }
  m_blocking_job_lock.unlock();
  m_blocking_job_sem.notify(1);
}

void job_pool::call_blocking_function(const char* job_name,
    functional::function<void()> _function, signal_ptr signal) {
  bool needs_wait = !signal;
  signal_ptr sig = core::move(needs_wait ? get_signal() : signal);
  m_blocking_job_lock.lock();
  if (!m_exit && m_active_blocking_job_threads >= m_blocking_threads.size()) {
    // TODO: set thread affinity here as 0
    m_blocking_threads.push_back(multi_tasking::thread(m_allocator,
        [this, sig = core::move(signal)]() { run_blocking_thread(); }));
  }

  m_blocking_jobs.push_back({_function, sig, job_name});
  m_blocking_job_lock.unlock();
  m_blocking_job_sem.notify();
  if (needs_wait) {
    sig.wait_until(1);
  }
}

void job_pool::add_job(const char* name, functional::function<void()> _function,
    signal_ptr _signal, job_priority _priority) {
  (void)name;
  containers::list<job>::list_node* n = nullptr;

  if (m_idle_job_lock.try_lock()) {
    if (!m_idle_jobs.empty()) {
      n = m_idle_jobs.unlink_node(m_idle_jobs.begin());
    }
    m_idle_job_lock.unlock();
  }
  if (!n) {
    n = m_idle_jobs.make_node();
  }
  m_total_jobs++;
  n->element()->id = ++m_next_job_id;
  n->element()->_fn = core::move(_function);
  n->element()->priority = _priority;
  n->element()->signal = core::move(_signal);
  n->element()->name = name;

  if (_priority == job_priority::low) {
    m_low_priority_lock.lock();
    m_low_priority_jobs.link_node(m_low_priority_jobs.end(), n);
    m_low_priority_lock.unlock();
    n = nullptr;
    {
      lock_guard<spin_lock> lg(m_waiting_work_lock);
      m_waiting_work++;
    }
    m_waiting_work_cv.notify_all();
    return;
  }

  if (_priority == job_priority::high) {
    m_priority_lock.lock();
    m_priority_jobs.link_node(m_priority_jobs.end(), n);
    m_priority_lock.unlock();
    m_num_priority_tasks++;
    n = nullptr;
    {
      lock_guard<spin_lock> lg(m_waiting_work_lock);
      m_waiting_work++;
    }
    m_waiting_work_cv.notify_all();
    return;
  }

  for (size_t i = 0; i < m_worker_threads.size(); ++i) {
    uint64_t thread_id = rand() % m_threads.size();
    auto& wt = m_threads[thread_id];
    if (wt.m_job_lock.try_lock()) {
      wt.m_ready_jobs.link_node(wt.m_ready_jobs.end(), n);
      n = nullptr;
      wt.m_num_tasks++;
      wt.m_job_lock.unlock();
      {
        lock_guard<spin_lock> lg(m_waiting_work_lock);
        m_waiting_work++;
      }
      m_waiting_work_cv.notify_all();
      return;
    }
  }
  // All threads were locked when we tried to insert, now we just wait on a
  // specific thread.
  uint64_t thread_id = rand() % m_threads.size();
  auto& wt = m_threads[thread_id];
  wt.m_job_lock.lock();
  wt.m_ready_jobs.link_node(wt.m_ready_jobs.end(), n);
  n = nullptr;
  wt.m_num_tasks++;
  wt.m_job_lock.unlock();
  {
    lock_guard<spin_lock> lg(m_waiting_work_lock);
    m_waiting_work++;
  }
  m_waiting_work_cv.notify_all();
}

void job_pool::add_job_on_main(const char* name,
    functional::function<void()> _function, signal_ptr signal,
    job_priority _priority, bool _blocking) {
  (void)name;
  containers::list<job>::list_node* n = nullptr;

  if (m_idle_job_lock.try_lock()) {
    if (!m_idle_jobs.empty()) {
      n = m_idle_jobs.unlink_node(m_idle_jobs.begin());
    }
    m_idle_job_lock.unlock();
  }
  if (!n) {
    n = m_idle_jobs.make_node();
  }
  m_total_jobs++;
  n->element()->id = ++m_next_job_id;
  n->element()->_fn = core::move(_function);
  n->element()->priority = _priority;
  n->element()->lock_to_thread = 0;
  n->element()->signal = core::move(signal);
  n->element()->name = name;
  n->element()->flag = _blocking ? job_flag::blocking_on_main : job_flag::none;
  // All threads were locked when we tried to insert, now we just wait on a
  // specific thread.
  auto& wt = m_threads[0];
  wt.m_job_lock.lock();
  wt.m_ready_jobs.link_node(wt.m_ready_jobs.end(), n);
  n = nullptr;
  wt.m_num_tasks++;
  wt.m_job_lock.unlock();
  {
    lock_guard<spin_lock> lg(m_waiting_work_lock);
    m_waiting_work++;
  }
  m_waiting_work_cv.notify_all();
}

void job_pool::attach_main_thread() {
  WN_RELEASE_ASSERT(
      m_bind_main_thread, "Cannot attach main thread if none exists");
  WN_RELEASE_ASSERT(
      this_thread_data() == nullptr, "Main thread already attached");
  set_this_job_pool(this);
  this_thread_data() = &m_threads[0];
  initialize_thread_for_work();
  this_thread_data() = nullptr;
}

void job_pool::initialize_thread_for_work() {
  set_this_job_pool(this);
  auto wt = this_thread_data();
  wt->m_initial_fiber.id = static_cast<uint64_t>(-1);
  wt->m_initial_fiber.fib =
      marl::OSFiber::createFiberFromCurrentThread(m_fiber_allocator.get());
  wt->m_initial_fiber.status = fiber_status::initial;
  wt->m_initial_fiber.on_swap_to_fiber =
      containers::dynamic_array<functional::function<void()>>(m_allocator);
  {
    wt->m_pending_job = nullptr;
    wt->m_current_running_fiber = wt->m_ready_fibers.make_node();
    wt->m_current_running_fiber->element()->on_swap_to_fiber =
        containers::dynamic_array<functional::function<void()>>(m_allocator);
    wt->m_current_running_fiber->element()->id = ++m_next_job_id;
    wt->m_current_running_fiber->element()->fib =
        marl::OSFiber::createFiber(m_fiber_allocator.get(), m_stack_size,
            [thread = this]() { thread->cycle_for_work(); });
    wt->m_current_running_fiber->element()->status = fiber_status::active;
    do {
      wt->m_initial_fiber.fib->switchTo(
          wt->m_current_running_fiber->element()->fib.get());
      if (wt->m_pending_job &&
          wt->m_pending_job->element()->flag == job_flag::blocking_on_main) {
        wt->m_pending_job->element()->_fn();
        continue;
      }
      break;
    } while (true);
  }
  wt = this_thread_data();
  WN_RELEASE_ASSERT(!wt->m_next_running_fiber, "We shouldn't end up here");
  m_idle_fiber_lock.lock();
  wt->m_current_running_fiber->element()->status = fiber_status::idle;
  m_idle_fibers.link_node(m_idle_fibers.end(), wt->m_current_running_fiber);
  wt->m_current_running_fiber = nullptr;
  m_idle_fiber_lock.unlock();
}

void job_pool::post_transition_fiber() {
  auto wt = this_thread_data();
  auto last_node = wt->m_current_running_fiber;
  wt->m_current_running_fiber = wt->m_next_running_fiber;
  wt->m_current_running_fiber->element()->blocking_signal = nullptr;
  wt->m_current_running_fiber->element()->status = fiber_status::active;
  wt->m_next_running_fiber = nullptr;
  switch (last_node->element()->status) {
    case fiber_status::moving_to_idle:
      last_node->element()->status = fiber_status::idle;
      m_idle_fiber_lock.lock();
      m_idle_fibers.link_node(m_idle_fibers.begin(), last_node);
      m_idle_fiber_lock.unlock();
      break;
    case fiber_status::blocking_on_signal: {
      last_node->element()->status = fiber_status::blocked_on_signal;
      auto sig = last_node->element()->blocking_signal;
      last_node->element()->blocking_signal = nullptr;
      sig->m_lock.unlock();
    } break;
    case fiber_status::initial:
      // Initial fibers are a bit special
      break;
    default:
      WN_RELEASE_ASSERT(false, "We should not have gotten here");
  }
  for (auto& fn : wt->m_current_running_fiber->element()->on_swap_to_fiber) {
    fn();
  }
}

void job_pool::swap_to(containers::list<fiber>::list_node* _node) {
  auto wt = this_thread_data();
  WN_RELEASE_ASSERT(
      _node != wt->m_current_running_fiber, "Cannot transition to self");
  WN_RELEASE_ASSERT(
      _node->element()->status == fiber_status::idle ||
          _node->element()->status == fiber_status::blocked_on_signal,
      "Invalid status for target fiber");

  wt->m_next_running_fiber = _node;
  if (wt->m_current_running_fiber->element()->status == fiber_status::active) {
    wt->m_current_running_fiber->element()->status =
        fiber_status::moving_to_idle;
  }
  wt->m_current_running_fiber->element()->fib->switchTo(
      _node->element()->fib.get());
  post_transition_fiber();
}

void job_pool::spin_up_next_fiber() {
  post_transition_fiber();

  cycle_for_work();
}

void job_pool::yield() {}

containers::list<fiber>::list_node* job_pool::get_new_fiber() {
  m_idle_fiber_lock.lock();
  containers::list<fiber>::list_node* _node;
  if (!m_idle_fibers.empty()) {
    _node = m_idle_fibers.unlink_node(m_idle_fibers.begin());
  } else {
    _node = m_idle_fibers.make_node();
    _node->element()->on_swap_to_fiber =
        containers::dynamic_array<functional::function<void()>>(m_allocator);
  }
  m_idle_fiber_lock.unlock();
  return _node;
}

bool job_pool::run_next_task(bool new_fiber_for_job) {
  auto wt = this_thread_data();

  auto run_job = [this, &wt](containers::list<job>::list_node*& _node) {
    wt->m_current_running_fiber->element()->priority =
        _node->element()->priority;
    wt->m_current_running_fiber->element()->lock_to_thread =
        _node->element()->lock_to_thread;
    m_log->log_verbose("Starting job ", _node->element()->name);
    m_log->flush();
    _node->element()->_fn();
    m_log->log_verbose("Ending job ", _node->element()->name);
    m_log->flush();
    m_idle_job_lock.lock();
    _node->element()->_fn = nullptr;
    auto sig = core::move(_node->element()->signal);
    m_idle_jobs.link_node(m_idle_jobs.end(), _node);
    m_idle_job_lock.unlock();
    _node = nullptr;
    m_total_jobs--;
    if (sig) {
      sig.increment_by(1);
    }
  };
  containers::list<fiber>::list_node* next_fiber = nullptr;
  containers::list<job>::list_node* next_job = nullptr;

  auto try_get_work = [&next_job, &next_fiber, this](thread_data* data,
                          bool is_this_thread, bool only_try) {
    if (is_this_thread && data->m_pending_job) {
      next_job = data->m_pending_job;
      data->m_pending_job = nullptr;
      return;
    }
    if (m_num_priority_tasks) {
      m_priority_lock.lock();
      if (!m_priority_fibers.empty()) {
        next_fiber = m_priority_fibers.unlink_node(m_priority_fibers.begin());
        m_num_priority_tasks--;
        m_priority_lock.unlock();
        return;
      } else if (!m_priority_jobs.empty()) {
        next_job = m_priority_jobs.unlink_node(m_priority_jobs.begin());
        m_num_priority_tasks--;
        m_priority_lock.unlock();
        return;
      }
      m_priority_lock.unlock();
    }
    if (data->m_num_tasks) {
      if (only_try) {
        if (!data->m_job_lock.try_lock()) {
          return;
        }
      } else {
        data->m_job_lock.lock();
      }

      if (!data->m_ready_fibers.empty()) {
        if (is_this_thread || data->m_ready_fibers.begin()->lock_to_thread ==
                                  ~static_cast<size_t>(0)) {
          next_fiber =
              data->m_ready_fibers.unlink_node(data->m_ready_fibers.begin());
          --data->m_num_tasks;
        }
      } else if (!data->m_ready_jobs.empty()) {
        if (is_this_thread || data->m_ready_jobs.begin()->lock_to_thread ==
                                  ~static_cast<size_t>(0)) {
          next_job = data->m_ready_jobs.unlink_node(data->m_ready_jobs.begin());
          --data->m_num_tasks;
        }
      } else {
        WN_DEBUG_ASSERT(
            0 == data->m_num_tasks, "Inconsistent number of tasks present");
      }
      data->m_job_lock.unlock();
    }
  };
  do {
    try_get_work(wt, true, false);
    // TODO: priority
    WN_DEBUG_ASSERT(!(next_job && next_fiber),
        "We got both a job and a fiber... shouldn't be possible");

    if (next_job && next_job->element()->flag == job_flag::blocking_on_main) {
      m_waiting_work--;
      // This is ugly but for now it may be enough
      containers::list<fiber>::list_node* _node = get_new_fiber();
      _node->element()->id = static_cast<uint64_t>(++m_next_job_id);
      _node->element()->fib = marl::OSFiber::createFiber(
          m_fiber_allocator.get(), m_stack_size, [this, next_job]() {
            post_transition_fiber();
            auto wt = this_thread_data();
            wt->m_pending_job = next_job;
            wt->m_current_running_fiber->element()->fib->switchTo(
                wt->m_initial_fiber.fib.get());
            m_idle_job_lock.lock();
            next_job->element()->_fn = nullptr;
            m_idle_jobs.link_node(m_idle_jobs.end(), next_job);
            m_idle_job_lock.unlock();
            if (next_job->element()->signal) {
              next_job->element()->signal.increment_by(1);
            }
            wt->m_pending_job = nullptr;
            m_total_jobs--;
            cycle_for_work();
          });
      _node->element()->status = fiber_status::idle;
      swap_to(_node);
      return true;
    }
    break;
  } while (true);

  if (next_fiber) {
    m_waiting_work--;
    swap_to(next_fiber);
    return true;
  }
  if (next_job) {
    m_waiting_work--;
    if (new_fiber_for_job) {
      containers::list<fiber>::list_node* _node = get_new_fiber();
      wt->m_pending_job = next_job;
      _node->element()->id = static_cast<uint64_t>(++m_next_job_id);
      _node->element()->fib =
          marl::OSFiber::createFiber(m_fiber_allocator.get(), m_stack_size,
              [this]() { spin_up_next_fiber(); });
      _node->element()->status = fiber_status::idle;
      swap_to(_node);
      return true;
    } else {
      run_job(next_job);
    }
    return true;
  }

  auto duration = std::chrono::microseconds(new_fiber_for_job ? 0 : 10);
  auto start = std::chrono::high_resolution_clock::now();
  // We had no fiber work, OR job work.
  // Lets start trying to steal from a neighbour

  // Shamelessly stolen from Marl. One change, minimum 1 loop
  do {
    for (int i = 0; i < 256; i++) {
      // clang-format off
      nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
      nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
      nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
      nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();
      // clang-format on
      if (wt->m_num_tasks) {
        break;
      }
      if (m_num_priority_tasks) {
        break;
      }
    }

    uint64_t thread_id = rand() % m_threads.size();
    if (thread_id != wt->m_thread_id) {
      try_get_work(&m_threads[thread_id], false, true);
      if (next_fiber || next_job) {
        break;
      }
    }
    this_thread::yield();
  } while (std::chrono::high_resolution_clock::now() - start < duration);

  if (!next_fiber && !next_job && !wt->m_num_tasks && !m_num_priority_tasks) {
    m_low_priority_lock.lock();
    // We should start low priority jobs
    // before continuing with low priority fibers because
    // these jobs may be infinitely long running
    // so we want to cycle through all of the low-priority
    // work.

    if (!m_low_priority_jobs.empty()) {
      next_job = m_low_priority_jobs.unlink_node(m_low_priority_jobs.begin());
    } else if (!m_low_priority_fibers.empty()) {
      next_fiber =
          m_low_priority_fibers.unlink_node(m_low_priority_fibers.begin());
    }
    m_low_priority_lock.unlock();
  }

  if (next_fiber) {
    m_waiting_work--;
    swap_to(next_fiber);
    return true;
  }
  if (next_job) {
    m_waiting_work--;
    if (new_fiber_for_job) {
      wt->m_pending_job = next_job;
      containers::list<fiber>::list_node* _node = get_new_fiber();
      _node->element()->id = static_cast<uint64_t>(++m_next_job_id);
      _node->element()->fib =
          marl::OSFiber::createFiber(m_fiber_allocator.get(), m_stack_size,
              [this]() { spin_up_next_fiber(); });
      _node->element()->status = fiber_status::idle;
      swap_to(_node);
      return true;
    }
    run_job(next_job);
    return true;
  }

  return false;
}

void job_pool::pop_swap_function(size_t idx) {
  auto ttd = this_thread_data();
  WN_RELEASE_ASSERT(
      idx == ttd->m_current_running_fiber->element()->on_swap_to_fiber.size(),
      "swap functions are expected to be pushed/popped in stack order");
  ttd->m_current_running_fiber->element()->on_swap_to_fiber.pop_back();
}

job_pool::fiber_swap_function_handle job_pool::add_fiber_swap_function(
    functional::function<void()> on_fiber_swap) {
  auto ttd = this_thread_data();
  ttd->m_current_running_fiber->element()->on_swap_to_fiber.push_back(
      core::move(on_fiber_swap));
  size_t sz = ttd->m_current_running_fiber->element()->on_swap_to_fiber.size();
  return fiber_swap_function_handle(this, sz);
}

void job_pool::exit_this_thread() {
  auto ttd = this_thread_data();
  WN_RELEASE_ASSERT(ttd == &m_threads[0],
      "Cannot manually exit anything but the main thread");
  ttd->m_exit = true;
}

void job_pool::cycle_for_work() {
  while (!m_exit || (m_total_jobs != 0)) {
    while (run_next_task(false)) {
    }

    auto ttd = this_thread_data();
    if (ttd->m_exit) {
      break;
    }
    // If there was no work to do, wait until there is some work
    // that can be done on SOME thread (maybe we can steal it)
    m_waiting_work_lock.lock();
    m_waiting_work_cv.wait_until(m_waiting_work_lock,
        std::chrono::high_resolution_clock::now() +
            std::chrono::milliseconds(1),
        [this]() { return (m_waiting_work > 0) || m_exit; });
    m_waiting_work_lock.unlock();
  }
  // If any fibers return from this function, they
  // will stop the thread they are on, so lets make sure
  // to swap back to the initial fiber that we were using.
  auto wt = this_thread_data();
  wt->m_current_running_fiber->element()->fib->switchTo(
      wt->m_initial_fiber.fib.get());
}

bool signal::might_wait(uint64_t value) const {
  return !(m_current_value >= value);
}

void signal::wait_until(uint64_t value) {
  if (m_current_value >= value) {
    return;
  }
  m_lock.lock();
  if (m_current_value >= value) {
    m_lock.unlock();
    return;
  }
  if (value < m_next_job_value) {
    m_next_job_value = value;
  }
  auto wt = m_pool->this_thread_data();
  if (!wt) {
    wt = &m_pool->m_threads[0];
  }
  for (auto it = m_waiting_fibers.begin();; ++it) {
    if (it == m_waiting_fibers.end())
      if (it == m_waiting_fibers.end() || it->second > value) {
        m_waiting_fibers.insert(
            it, core::make_pair(wt->m_current_running_fiber, value));
        break;
      }
  }
  wt->m_current_running_fiber->element()->status =
      fiber_status::blocking_on_signal;
  wt->m_current_running_fiber->element()->blocking_signal = this;

  // If true, then a task was run, and we are back to ourselves
  if (!m_pool->run_next_task(true)) {
    // If false, then a task was NOT run, and we need to spin up a new fiber
    // to wait for tasks.
    auto _node = m_pool->get_new_fiber();
    _node->element()->id = static_cast<uint64_t>(++(m_pool->m_next_job_id));
    _node->element()->fib =
        marl::OSFiber::createFiber(m_pool->m_fiber_allocator.get(),
            m_pool->m_stack_size, [this]() { m_pool->spin_up_next_fiber(); });
    _node->element()->status = fiber_status::idle;
    m_pool->swap_to(_node);
  }
}

void signal::increment_by(uint64_t i) {
  m_lock.lock();
  m_current_value += i;
  if (m_next_job_value > m_current_value) {
    m_lock.unlock();
    return;
  }

  int n_jobs = 0;
  for (auto& wf : m_waiting_fibers) {
    if (wf.second <= m_current_value) {
      ++n_jobs;
    } else {
      break;
    }
  }
  if (!n_jobs) {
    m_lock.unlock();
    return;
  }
  size_t jobs_per_thread = (n_jobs / m_pool->m_threads.size()) + 1;

  auto wt = m_pool->this_thread_data();
  if (!wt) {
    wt = &m_pool->m_threads[0];
  }

  auto thread_id = wt->m_thread_id;
  while (n_jobs) {
    wt = &m_pool->m_threads[thread_id];
    wt->m_job_lock.lock();
    for (size_t j = 0; (j < jobs_per_thread) && n_jobs; ++j, --n_jobs) {
      auto fib = m_waiting_fibers.begin()->first;
      if (fib->element()->lock_to_thread != ~static_cast<size_t>(0)) {
        auto wwt = &m_pool->m_threads[fib->element()->lock_to_thread];
        if (wt != wwt) {
          wt->m_job_lock.unlock();
          wwt->m_job_lock.lock();
        }
        wwt->m_ready_fibers.link_node(
            wwt->m_ready_fibers.end(), m_waiting_fibers.begin()->first);
        wwt->m_num_tasks++;
        m_waiting_fibers.pop_front();
        if (wt != wwt) {
          wwt->m_job_lock.unlock();
          wt->m_job_lock.lock();
        }
        lock_guard<spin_lock> lg(m_pool->m_waiting_work_lock);
        m_pool->m_waiting_work++;
        continue;
      }
      if (fib->element()->priority == job_priority::high) {
        m_pool->m_priority_lock.lock();
        m_pool->m_priority_fibers.link_node(
            m_pool->m_priority_fibers.end(), m_waiting_fibers.begin()->first);
        m_pool->m_priority_lock.unlock();
        m_waiting_fibers.pop_front();
        m_pool->m_num_priority_tasks++;
        lock_guard<spin_lock> lg(m_pool->m_waiting_work_lock);
        m_pool->m_waiting_work++;
        continue;
      }
      if (fib->element()->priority == job_priority::low) {
        m_pool->m_low_priority_lock.lock();
        m_pool->m_low_priority_fibers.link_node(
            m_pool->m_low_priority_fibers.end(),
            m_waiting_fibers.begin()->first);
        m_pool->m_low_priority_lock.unlock();
        m_waiting_fibers.pop_front();
        lock_guard<spin_lock> lg(m_pool->m_waiting_work_lock);
        m_pool->m_waiting_work++;
        continue;
      }
      wt->m_ready_fibers.link_node(
          wt->m_ready_fibers.end(), m_waiting_fibers.begin()->first);
      wt->m_num_tasks++;
      m_waiting_fibers.pop_front();
      {
        lock_guard<spin_lock> lg(m_pool->m_waiting_work_lock);
        m_pool->m_waiting_work++;
      }
    }
    wt->m_job_lock.unlock();
    thread_id = (thread_id + 1) % m_pool->m_threads.size();
  }
  m_lock.unlock();
  m_pool->m_waiting_work_cv.notify_all();
}

signal_ptr job_pool::get_signal() {
  auto wt = this_thread_data();
  if (!wt) {
    wt = &m_threads[rand() % m_threads.size()];
  }
  wt->m_signal_lock.lock();
  containers::list<signal>::list_node* ln = nullptr;
  if (!wt->m_inactive_signals.empty()) {
    ln = wt->m_inactive_signals.unlink_node(wt->m_inactive_signals.begin());
  } else {
    ln = wt->m_inactive_signals.make_node();
    ln->element()->m_waiting_fibers = containers::deque<
        core::pair<containers::list<fiber>::list_node*, uint64_t>>(m_allocator);
  }
  ln->element()->m_pool = this;
  ln->element()->m_waiting_fibers.clear();
  ln->element()->m_ref_count = 0;
  ln->element()->m_next_job_value = 0;
  ln->element()->m_current_value = 0;

  wt->m_signal_lock.unlock();

  return signal_ptr(ln);
}

}  // namespace multi_tasking
}  // namespace wn
