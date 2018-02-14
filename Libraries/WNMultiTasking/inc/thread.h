// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_THREAD_H__
#define __WN_MULTI_TASKING_THREAD_H__

#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNUtility.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNMultiTasking/inc/internal/thread_base.h"

#include <chrono>

namespace wn {
namespace multi_tasking {

class thread final : internal::thread_base {
private:
  using base = internal::thread_base;

public:
  class id;

  thread() : base() {}

  thread(thread&& _other) : base(core::move(_other)) {}

  template <typename F, typename... Args,
      typename =
          core::enable_if_t<!core::is_same<core::decay_t<F>, thread>::value>>
  explicit thread(memory::allocator* _allocator, F&& _f, Args&&... _args) {
    static_assert(core::is_void<core::result_of_t<F(Args...)>>::value,
        "function must be void return");

    WN_RELEASE_ASSERT(_allocator, "allocator must not be nullptr");

    create(_allocator, functional::function<void()>(std::bind(
                           core::decay_copy(core::forward<F>(_f)),
                           core::decay_copy(core::forward<Args>(_args))...)));
  }

  thread& operator=(thread&& _other) {
    thread(core::move(_other)).swap(*this);

    return *this;
  }

  id get_id() const;

  bool joinable() const {
    return base::joinable();
  }

  bool join() const {
    return (joinable() ? base::join() : false);
  }

  void detach() {
    m_data.reset();
  }

  void swap(thread& other) {
    m_data.swap(other.m_data);
  }

private:
  void create(memory::allocator* _allocator, functional::function<void()>&& _f);
};

namespace this_thread {

thread::id get_id();

}  // namespace this_thread

class thread::id final : thread::id_base {
private:
  using base = thread::id_base;

public:
  id() : base() {}

private:
  friend class thread;
  friend thread::id this_thread::get_id();
  friend bool operator==(const thread::id&, const thread::id&);
  friend bool operator<(const thread::id&, const thread::id&);
  friend bool operator>(const thread::id&, const thread::id&);

  id(base&& _id) : base(core::move(_id)) {}
};

inline bool operator==(const thread::id& _lhs, const thread::id& _rhs) {
  return _lhs.equal(_rhs);
}

inline bool operator!=(const thread::id& _lhs, const thread::id& _rhs) {
  return !(_lhs == _rhs);
}

inline bool operator<(const thread::id& _lhs, const thread::id& _rhs) {
  return _lhs.less_than(_rhs);
}

inline bool operator<=(const thread::id& _lhs, const thread::id& _rhs) {
  return !(_lhs > _rhs);
}

inline bool operator>(const thread::id& _lhs, const thread::id& _rhs) {
  return _lhs.greater_than(_rhs);
}

inline bool operator>=(const thread::id& _lhs, const thread::id& _rhs) {
  return !(_lhs < _rhs);
}

inline thread::id thread::get_id() const {
  return base::get_id();
}

namespace this_thread {

inline thread::id get_id() {
  return internal::get_id();
}

inline void yield() {
  internal::yield();
}

template <typename Representation, typename Period>
inline void sleep_for(
    const std::chrono::duration<Representation, Period>& _duration) {
  internal::sleep_for(_duration);
}

template <typename Clock, typename Duration>
inline void sleep_until(
    const std::chrono::time_point<Clock, Duration>& _sleep_time) {
  sleep_for(_sleep_time - Clock::now());
}

}  // namespace this_thread
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_THREAD_H__
