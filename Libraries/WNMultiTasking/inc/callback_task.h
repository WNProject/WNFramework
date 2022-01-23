// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MULTI_TASKING_CALLBACK_TASK_H__
#define __WN_MULTI_TASKING_CALLBACK_TASK_H__

#include "WNFunctional/inc/WNFunction.h"
#include "WNMemory/inc/intrusive_ptr.h"
#include "WNMultiTasking/inc/thread_task.h"
#include "core/inc/type_traits.h"
#include "core/inc/utilities.h"

#include <functional>

namespace wn {
namespace multi_tasking {

template <typename R>
class callback_task final : public thread_task {
public:
  static_assert(
      !std::is_reference<R>::value, "return type must not be a reference");
  static_assert(!std::is_const<R>::value, "return type must not be const");

  typedef R result_type;

  template <typename F, typename... Args>
  inline callback_task(memory::allocator* _allocator, F&& f, Args&&... args)
    : thread_task() {
    static_assert(core::is_same<R, core::result_of_t<F(Args...)>>::value,
        "thread function return type does not match thread return type");

    m_callback = functional::function<R()>(
        _allocator, std::bind(core::decay_copy(core::forward<F>(f)),
                        core::decay_copy(core::forward<Args>(args))...));
  }

  virtual ~callback_task() override = default;

  using thread_task::join;

  inline bool join(R& result) {
    const bool join_result = join();

    if (join_result) {
      result = m_result;
    }

    return join_result;
  }

private:
  virtual inline void run() override {
    m_result = m_callback();
  }

  functional::function<R()> m_callback;
  R m_result;
};

template <>
class callback_task<void> : public thread_task {
public:
  typedef void result_type;

  template <typename F, typename... Args>
  inline callback_task(memory::allocator* _allocator, F&& f, Args&&... args)
    : thread_task() {
    static_assert(core::is_same<void, core::result_of_t<F(Args...)>>::value,
        "thread function return type does not match thread return type");

    m_callback = functional::function<void()>(
        _allocator, std::bind(core::decay_copy(core::forward<F>(f)),
                        core::decay_copy(core::forward<Args>(args))...));
  }

  virtual ~callback_task() override = default;

private:
  virtual inline void run() override {
    m_callback();
  }

  functional::function<void()> m_callback;
};

template <typename R>
using callback_task_ptr = memory::intrusive_ptr<callback_task<R>>;

template <typename R, typename... Args>
inline callback_task_ptr<R> make_callback_task(
    memory::allocator* _allocator, Args&&... _args) {
  return memory::make_intrusive<callback_task<R>>(
      _allocator, _allocator, core::forward<Args>(_args)...);
}

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_CALLBACK_TASK_H__
