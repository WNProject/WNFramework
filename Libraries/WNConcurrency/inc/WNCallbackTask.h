// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_CALLBACK_TASK_H__
#define __WN_CONCURRENCY_CALLBACK_TASK_H__

#include "WNConcurrency/inc/WNThreadTask.h"
#include "WNContainers/inc/WNFunction.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNUtility.h"

namespace wn {
namespace concurrency {

template <typename R>
class callback_task final : public thread_task {
public:
  static_assert(!std::is_reference<R>::value,
    "return type must not be a reference");
  static_assert(!std::is_const<R>::value, "return type must not be const");

  typedef R result_type;

  template <typename F, typename... Args>
  WN_FORCE_INLINE callback_task(F&& f, Args&&... args) :
    thread_task() {
    static_assert(core::is_same<R, core::result_of_t<F(Args...)>>::value,
      "thread function return type does not match thread return type");

    m_callback = std::bind(core::decay_copy(std::forward<F>(f)),
                           core::decay_copy(std::forward<Args>(args))...);
  }

  virtual ~callback_task() override = default;

  using thread_task::join;

  WN_FORCE_INLINE wn_bool join(R& result) {
    const wn_bool join_result = join();

    if (join_result) {
      result = m_result;
    }

    return(join_result);
  }

private:
  virtual WN_FORCE_INLINE wn_void run() override {
    m_result = m_callback();
  }

  containers::function<R()> m_callback;
  R m_result;
};

template <>
class callback_task<wn_void> : public thread_task {
public:
  typedef wn_void result_type;

  template <typename F, typename... Args>
  WN_FORCE_INLINE callback_task(F&& f, Args&&... args) :
    thread_task() {
    static_assert(core::is_same<wn_void, core::result_of_t<F(Args...)>>::value,
      "thread function return type does not match thread return type");

    m_callback = std::bind(core::decay_copy(std::forward<F>(f)),
                           core::decay_copy(std::forward<Args>(args))...);
  }

  virtual ~callback_task() override = default;

private:
  virtual WN_FORCE_INLINE wn_void run() override {
    m_callback();
  }

  containers::function<wn_void()> m_callback;
};

template <typename R, typename... Args>
memory::intrusive_ptr<callback_task<R>> make_callback_task(Args&&... args) {
  return(memory::make_intrusive<callback_task<R>>(std::forward<Args>(args)...));
}

template <typename R>
using callback_task_ptr = memory::intrusive_ptr<callback_task<R>>;

} // namespace concurrency
} // namespace wn

#endif // __WN_CONCURRENCY_CALLBACK_TASK_H__
