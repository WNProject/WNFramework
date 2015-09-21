// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_FUNCTION_H__
#define __WN_CONTAINERS_FUNCTION_H__

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNUtility.h"
#include "WNMemory/inc/WNBasic.h"

namespace wn {
namespace containers {

template <typename T>
class function final {
  static_assert(core::dependent_false<T>::value,
    "type provided is not a function prototype");
};

template <typename R, typename... Args>
class function<R(Args...)> {
public:
  typedef R result_type;

  WN_FORCE_INLINE function() :
    m_callable(wn_nullptr),
    m_executor(wn_nullptr),
    m_copier(wn_nullptr),
    m_deleter(wn_nullptr) {
  }

  WN_FORCE_INLINE function(const wn_nullptr_t) :
    function() {
  }

  WN_FORCE_INLINE function(function&& other) :
    function() {
    swap(other);
  }

  WN_FORCE_INLINE function(const function& other) :
    m_callable(wn_nullptr),
    m_executor(other.m_executor),
    m_copier(other.m_copier),
    m_deleter(other.m_deleter) {
    if (m_copier) {
      m_copier(&other.m_callable, &m_callable);
    }
  }

  template <typename F,
            typename = core::enable_if_t<core::boolean_and<
              core::is_callable<F, R(Args...)>::value,
              !core::is_same_decayed<F, function>::value>::value>>
  WN_FORCE_INLINE function(F&& f) :
    function() {
    construct(std::forward<F>(f));
  }

  template <typename T>
  WN_FORCE_INLINE function(R(T::*f)(Args...), T* obj) :
    function([obj, f](Args... args) -> R {
      return((obj->*f)(args...));
    }) {
    WN_RELEASE_ASSERT_DESC(f, "member function is invalid");
    WN_RELEASE_ASSERT_DESC(obj, "object instance is invalid");
  }

  WN_FORCE_INLINE ~function() {
    if (m_callable) {
      WN_DEBUG_ASSERT_DESC(m_deleter,
        "function is not in valid state to be cleaned up");

      if (m_deleter) {
        m_deleter(&m_callable);
      }
    }
  }

  WN_FORCE_INLINE function& operator = (const wn_nullptr_t) {
    function(wn_nullptr).swap(*this);

    return(*this);
  }

  WN_FORCE_INLINE function& operator = (function&& other) {
    function(std::move(other)).swap(*this);

    return(*this);
  }

  WN_FORCE_INLINE function& operator = (const function& other) {
    function(other).swap(*this);

    return(*this);
  }

  template <typename F,
            typename = core::enable_if_t<core::boolean_and<
              core::is_callable<F, R(Args...)>::value,
              !core::is_same_decayed<F, function>::value>::value>>
  WN_FORCE_INLINE function& operator = (F&& f) {
    function(std::forward<F>(f)).swap(*this);

    return(*this);
  }

  explicit WN_FORCE_INLINE operator wn_bool () const {
    return(m_callable && m_executor && m_deleter && m_copier);
  }

  WN_FORCE_INLINE R operator () (Args... args) const {
    WN_RELEASE_ASSERT_DESC(m_callable && m_executor,
      "function must be valid in order to be executed");

    return(m_executor(&m_callable, args...));
  }

  WN_FORCE_INLINE wn_void assign(const wn_nullptr_t) {
    *this = wn_nullptr;
  }

  template <typename F,
            typename = core::enable_if_t<core::boolean_and<
              core::is_callable<F, R(Args...)>::value,
              !core::is_same_decayed<F, function>::value>::value>>
  WN_FORCE_INLINE wn_void assign(F&& f) {
    *this = std::forward<F>(f);
  }

  template <typename T>
  WN_FORCE_INLINE wn_void assign(R(T::*f)(Args...), T* obj) {
    function(f, obj).swap(*this);
  }

  WN_FORCE_INLINE wn_void swap(function& other) {
    if ((&other) != this) {
      std::swap(m_callable, other.m_callable);
      std::swap(m_executor, other.m_executor);
      std::swap(m_copier, other.m_copier);
      std::swap(m_deleter, other.m_deleter);
    }
  }

private:
  typedef R(*executor_type)(wn_void* const*, Args...);
  typedef wn_void(*copier_type)(wn_void* const*, wn_void**);
  typedef wn_void(*deleter_type)(wn_void**);

  template <typename F>
  WN_FORCE_INLINE typename core::enable_if<core::boolean_and<
    !std::is_convertible<F, R(*)(Args...)>::value,
    !std::is_function<F>::value>::value>::type
  construct(F&& f) {
    m_callable = memory::construct<F>(std::forward<F>(f));

    WN_RELEASE_ASSERT_DESC(m_callable,
      "failed to construct lambda/function object");

    m_executor = [](wn_void* const* f, Args... args) -> R {
      return((*reinterpret_cast<F* const*>(f))->operator()(args...));
    };

    m_copier = [](wn_void* const* f, wn_void** ptr) -> wn_void {
      F* const p = *reinterpret_cast<F* const*>(f);

      (*ptr) = memory::construct<F>(std::forward<F>(*p));

      WN_RELEASE_ASSERT_DESC((*ptr),
        "failed to construct lambda/function object");
    };

    m_deleter = [](wn_void** f) -> wn_void {
      memory::destroy<F>(reinterpret_cast<F*>(*f));
    };
  }

  template <typename F>
  WN_FORCE_INLINE typename core::enable_if<core::boolean_or<
    std::is_convertible<F, R(*)(Args...)>::value,
    std::is_function<F>::value>::value>::type
  construct(F&& f) {
    m_callable = reinterpret_cast<wn_void*>(static_cast<R(*)(Args...)>(f));

    m_executor = [](wn_void* const* f, Args... args) -> R {
      return((*reinterpret_cast<R(*const*)(Args...)>(f))(args...));
    };

    m_copier = [](wn_void* const* f, wn_void** ptr) -> wn_void {
      (*ptr) = (*f);
    };

    m_deleter = [](wn_void**) -> wn_void {};
  }

  wn_void* m_callable;
  copier_type m_copier;
  deleter_type m_deleter;
  executor_type m_executor;
};

template <typename R, typename... Args>
WN_FORCE_INLINE wn_bool
operator == (const function<R(Args...)>& f, const wn_nullptr_t) {
  return(!f);
}

template <typename R, typename... Args>
WN_FORCE_INLINE wn_bool
operator == (const wn_nullptr_t, const function<R(Args...)>& f) {
  return(f == wn_nullptr);
}

template <typename R, typename... Args>
WN_FORCE_INLINE wn_bool
operator != (const function<R(Args...)>& f, const wn_nullptr_t) {
  return(static_cast<wn_bool>(f));
}

template <typename R, typename... Args>
WN_FORCE_INLINE wn_bool
operator != (const wn_nullptr_t, const function<R(Args...)>& f) {
  return(f != wn_nullptr);
}

} // namespace containers
} // namespace wn

#endif // __WN_CONTAINERS_FUNCTION_H__
