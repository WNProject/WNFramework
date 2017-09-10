// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FUNCTIONAL_FUNCTION_H__
#define __WN_FUNCTIONAL_FUNCTION_H__

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNUtility.h"
#include "WNMemory/inc/WNBasic.h"

namespace wn {
namespace functional {

template <typename T>
class function final {};

template <typename R, typename... Args>
class function<R(Args...)> {
public:
  using result_type = R;

  WN_FORCE_INLINE function()
    : m_callable(nullptr),
      m_executor(nullptr),
      m_copier(nullptr),
      m_deleter(nullptr) {}

  WN_FORCE_INLINE function(const nullptr_t) : function() {}

  WN_FORCE_INLINE function(function&& _other) : function() {
    swap(_other);
  }

  WN_FORCE_INLINE function(const function& _other)
    : m_callable(nullptr),
      m_executor(_other.m_executor),
      m_copier(_other.m_copier),
      m_deleter(_other.m_deleter) {
    if (m_copier) {
      m_copier(&_other.m_callable, &m_callable);
    }
  }

  template <typename F,
      typename =
          core::enable_if_t<core::conjunction<core::is_callable<F, R(Args...)>,
              core::negation<core::is_same_decayed<F, function>>>::value>>
  WN_FORCE_INLINE function(F&& _f) : function() {
    construct(core::forward<F>(_f));
  }

  template <typename T>
  WN_FORCE_INLINE function(R (T::*_f)(Args...), T* _obj)
    : function(
          [_obj, _f](Args... _args) -> R { return ((_obj->*_f)(_args...)); }) {
    WN_RELEASE_ASSERT_DESC(_f, "member function is invalid");
    WN_RELEASE_ASSERT_DESC(_obj, "object instance is invalid");
  }

  WN_FORCE_INLINE ~function() {
    if (m_callable) {
      WN_DEBUG_ASSERT_DESC(m_deleter, "not in valid state to be cleaned up");

      if (m_deleter) {
        m_deleter(&m_callable);
      }
    }
  }

  WN_FORCE_INLINE function& operator=(const nullptr_t) {
    function(nullptr).swap(*this);

    return *this;
  }

  WN_FORCE_INLINE function& operator=(function&& _other) {
    function(core::move(_other)).swap(*this);

    return *this;
  }

  WN_FORCE_INLINE function& operator=(const function& _other) {
    function(_other).swap(*this);

    return *this;
  }

  template <typename F,
      typename =
          core::enable_if_t<core::conjunction<core::is_callable<F, R(Args...)>,
              core::negation<core::is_same_decayed<F, function>>>::value>>
  WN_FORCE_INLINE function& operator=(F&& _f) {
    function(core::forward<F>(_f)).swap(*this);

    return *this;
  }

  explicit WN_FORCE_INLINE operator bool() const {
    return (m_callable && m_executor && m_deleter && m_copier);
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
      typename =
          core::enable_if_t<core::conjunction<core::is_callable<F, R(Args...)>,
              core::negation<core::is_same_decayed<F, function>>>::value>>
  WN_FORCE_INLINE void assign(F&& _f) {
    *this = core::forward<F>(_f);
  }

  template <typename T>
  WN_FORCE_INLINE void assign(R (T::*_f)(Args...), T* _obj) {
    function(_f, _obj).swap(*this);
  }

  WN_FORCE_INLINE void swap(function& _other) {
    if ((&_other) != this) {
      core::swap(m_callable, _other.m_callable);
      core::swap(m_executor, _other.m_executor);
      core::swap(m_copier, _other.m_copier);
      core::swap(m_deleter, _other.m_deleter);
    }
  }

private:
  using executor_type = R (*)(void* const*, Args...);
  using copier_type = void (*)(void* const*, void**);
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

    m_copier = [](void* const* _f, void** _ptr) -> void {
      F* const p = *reinterpret_cast<F* const*>(_f);

      (*_ptr) = memory::construct<F>(*p);

      WN_RELEASE_ASSERT_DESC(
          (*_ptr), "failed to construct lambda/function object");
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

    m_copier = [](void* const* _f, void** _ptr) -> void { (*_ptr) = (*_f); };

    m_deleter = [](void**) -> void {};
  }

  void* m_callable;
  executor_type m_executor;
  copier_type m_copier;
  deleter_type m_deleter;
};

template <typename R, typename... Args>
WN_FORCE_INLINE bool operator==(
    const function<R(Args...)>& _f, const nullptr_t) {
  return !_f;
}

template <typename R, typename... Args>
WN_FORCE_INLINE bool operator==(
    const nullptr_t, const function<R(Args...)>& _f) {
  return (_f == nullptr);
}

template <typename R, typename... Args>
WN_FORCE_INLINE bool operator!=(
    const function<R(Args...)>& _f, const nullptr_t) {
  return static_cast<bool>(_f);
}

template <typename R, typename... Args>
WN_FORCE_INLINE bool operator!=(
    const nullptr_t, const function<R(Args...)>& _f) {
  return (_f != nullptr);
}

}  // namespace functional
}  // namespace wn

#endif  // __WN_FUNCTIONAL_FUNCTION_H__
