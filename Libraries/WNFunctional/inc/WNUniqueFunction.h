// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FUNCTIONAL_UNIQUE_FUNCTION_H__
#define __WN_FUNCTIONAL_UNIQUE_FUNCTION_H__

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNUtility.h"
#include "WNMemory/inc/WNBasic.h"

namespace wn {
namespace functional {

template <typename T>
class unique_function final {};

template <typename R, typename... Args>
class unique_function<R(Args...)> {
public:
  using result_type = R;

  WN_FORCE_INLINE unique_function()
    : m_callable(nullptr), m_executor(nullptr), m_deleter(nullptr) {}

  WN_FORCE_INLINE unique_function(const nullptr_t) : unique_function() {}

  WN_FORCE_INLINE unique_function(unique_function&& _other)
    : unique_function() {
    swap(_other);
  }

  template <typename F,
      typename = core::enable_if_t<core::conjunction<
          core::is_callable<F, R(Args...)>,
          core::negation<core::is_same_decayed<F, unique_function>>>::value>>
  WN_FORCE_INLINE unique_function(F&& _f) : unique_function() {
    construct(core::forward<F>(_f));
  }

  template <typename T>
  WN_FORCE_INLINE unique_function(R (T::*_f)(Args...), T* _obj)
    : unique_function(
          [_obj, _f](Args... _args) -> R { return ((_obj->*_f)(_args...)); }) {
    WN_RELEASE_ASSERT_DESC(_f, "member function is invalid");
    WN_RELEASE_ASSERT_DESC(_obj, "object instance is invalid");
  }

  WN_FORCE_INLINE ~unique_function() {
    if (m_callable) {
      WN_DEBUG_ASSERT_DESC(m_deleter, "not in valid state to be cleaned up");

      if (m_deleter) {
        m_deleter(&m_callable);
      }
    }
  }

  WN_FORCE_INLINE unique_function& operator=(const nullptr_t) {
    unique_function(nullptr).swap(*this);

    return *this;
  }

  WN_FORCE_INLINE unique_function& operator=(unique_function&& _other) {
    unique_function(core::move(_other)).swap(*this);

    return *this;
  }

  template <typename F,
      typename = core::enable_if_t<core::conjunction<
          core::is_callable<F, R(Args...)>,
          core::negation<core::is_same_decayed<F, unique_function>>>::value>>
  WN_FORCE_INLINE unique_function& operator=(F&& _f) {
    unique_function(core::forward<F>(_f)).swap(*this);

    return *this;
  }

  explicit WN_FORCE_INLINE operator bool() const {
    return (m_callable && m_executor && m_deleter);
  }

  WN_FORCE_INLINE R operator()(Args... _args) const {
    WN_RELEASE_ASSERT_DESC(
        m_callable && m_executor, "must be valid in order to be executed");

    return m_executor(&m_callable, core::forward<Args>(_args)...);
  }

  WN_FORCE_INLINE void assign(const nullptr_t) {
    *this = nullptr;
  }

  template <typename F,
      typename = core::enable_if_t<core::conjunction<
          core::is_callable<F, R(Args...)>,
          core::negation<core::is_same_decayed<F, unique_function>>>::value>>
  WN_FORCE_INLINE void assign(F&& _f) {
    *this = core::forward<F>(_f);
  }

  template <typename T>
  WN_FORCE_INLINE void assign(R (T::*_f)(Args...), T* _obj) {
    unique_function(_f, _obj).swap(*this);
  }

  WN_FORCE_INLINE void swap(unique_function& _other) {
    if ((&_other) != this) {
      core::swap(m_callable, _other.m_callable);
      core::swap(m_executor, _other.m_executor);
      core::swap(m_deleter, _other.m_deleter);
    }
  }

private:
  using executor_type = R (*)(void* const*, Args...);
  using deleter_type = void (*)(void**);

  template <typename F>
  WN_FORCE_INLINE typename core::enable_if<
      core::conjunction<core::negation<core::is_convertible<F, R (*)(Args...)>>,
          core::negation<core::is_function<F>>>::value>::type
  construct(F&& _f) {
    m_callable = memory::construct<F>(core::forward<F>(_f));

    WN_RELEASE_ASSERT_DESC(
        m_callable, "failed to construct lambda/function object");

    m_executor = [](void* const* _f, Args... _args) -> R {
      return ((*reinterpret_cast<F* const*>(_f))
                  ->
                  operator()(core::forward<Args>(_args)...));
    };

    m_deleter = [](
        void** _f) -> void { memory::destroy<F>(reinterpret_cast<F*>(*_f)); };
  }

  template <typename F>
  WN_FORCE_INLINE typename core::enable_if<
      core::disjunction<core::is_convertible<F, R (*)(Args...)>,
          core::is_function<F>>::value>::type
  construct(F&& _f) {
    m_callable = reinterpret_cast<void*>(static_cast<R (*)(Args...)>(_f));

    m_executor = [](void* const* _f, Args... _args) -> R {
      return ((*reinterpret_cast<R (*const*)(Args...)>(_f))(
          core::forward<Args>(_args)...));
    };

    m_deleter = [](void**) -> void {};
  }

  void* m_callable;
  deleter_type m_deleter;
  executor_type m_executor;
};

template <typename R, typename... Args>
WN_FORCE_INLINE bool operator==(
    const unique_function<R(Args...)>& _f, const nullptr_t) {
  return !_f;
}

template <typename R, typename... Args>
WN_FORCE_INLINE bool operator==(
    const nullptr_t, const unique_function<R(Args...)>& _f) {
  return (_f == nullptr);
}

template <typename R, typename... Args>
WN_FORCE_INLINE bool operator!=(
    const unique_function<R(Args...)>& _f, const nullptr_t) {
  return static_cast<bool>(_f);
}

template <typename R, typename... Args>
WN_FORCE_INLINE bool operator!=(
    const nullptr_t, const unique_function<R(Args...)>& _f) {
  return (_f != nullptr);
}

}  // namespace functional
}  // namespace wn

#endif  // __WN_FUNCTIONAL_UNIQUE_FUNCTION_H__
