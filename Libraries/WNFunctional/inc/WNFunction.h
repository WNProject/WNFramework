// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FUNCTIONAL_FUNCTION_H__
#define __WN_FUNCTIONAL_FUNCTION_H__

#include "WNMemory/inc/allocator.h"
#include "core/inc/assert.h"
#include "core/inc/utilities.h"

namespace wn {
namespace functional {

template <typename T>
class function final {};

template <typename R, typename... Args>
class function<R(Args...)> {
public:
  using result_type = R;

  function()
    : m_allocator(nullptr),
      m_callable(nullptr),
      m_executor(nullptr),
      m_copier(nullptr),
      m_deleter(nullptr) {}

  function(const nullptr_t) : function() {}

  function(function&& _other)
    : m_allocator(core::move(_other.m_allocator)),
      m_callable(core::move(_other.m_callable)),
      m_executor(core::move(_other.m_executor)),
      m_copier(core::move(_other.m_copier)),
      m_deleter(core::move(_other.m_deleter)) {
    _other.clear();
  }

  function(const function& _other)
    : m_allocator(_other.m_allocator),
      m_callable(nullptr),
      m_executor(_other.m_executor),
      m_copier(_other.m_copier),
      m_deleter(_other.m_deleter) {
    if (m_copier) {
      m_copier(m_allocator, &_other.m_callable, &m_callable);
    }
  }

  explicit function(memory::allocator* _allocator)
    : m_allocator(_allocator),
      m_callable(nullptr),
      m_executor(nullptr),
      m_copier(nullptr),
      m_deleter(nullptr) {
    WN_DEBUG_ASSERT(m_allocator != nullptr, "allocator must not be null");
  }

  template <typename F,
      typename = core::enable_if_t<
          core::conjunction<core::is_invocable_r<R, F, Args...>,
              core::negation<core::is_same_decayed<F, function>>>::value>>
  function(memory::allocator* _allocator, F&& _f) : function(_allocator) {
    construct(core::forward<F>(_f));
  }

  function(memory::allocator* _allocator, const function& _other)
    : m_allocator(_allocator),
      m_callable(nullptr),
      m_executor(_other.m_executor),
      m_copier(_other.m_copier),
      m_deleter(_other.m_deleter) {
    if (m_copier) {
      m_copier(m_allocator, &_other.m_callable, &m_callable);
    }
  }

  ~function() {
    if (m_callable) {
      WN_DEBUG_ASSERT(m_deleter, "not in valid state to be cleaned up");

      if (m_deleter) {
        m_deleter(m_allocator, &m_callable);
      }
    }
  }

  function& operator=(const nullptr_t) {
    function(nullptr).swap(*this);

    return *this;
  }

  function& operator=(function&& _other) {
    function(core::move(_other)).swap(*this);

    return *this;
  }

  function& operator=(const function& _other) {
    function(_other).swap(*this);

    return *this;
  }

  function& operator=(memory::allocator* _allocator) {
    function(_allocator).swap(*this);

    return *this;
  }

  template <typename F,
      typename = core::enable_if_t<
          core::conjunction<core::is_invocable_r<R, F, Args...>,
              core::negation<core::is_same_decayed<F, function>>>::value>>
  function& operator=(F&& _f) {
    function(m_allocator, core::forward<F>(_f)).swap(*this);

    return *this;
  }

  explicit operator bool() const {
    return (m_allocator && m_callable && m_executor && m_deleter && m_copier);
  }

  R operator()(Args... _args) const {
    WN_RELEASE_ASSERT(
        m_callable && m_executor, "must be valid in order to be executed");

    return m_executor(&m_callable, core::forward<Args>(_args)...);
  }

  memory::allocator* get_allocator() const {
    return m_allocator;
  }

  void reset() {
    function(nullptr).swap(*this);
  }

  void reset(const nullptr_t) {
    reset();
  }

  template <typename F,
      typename = core::enable_if_t<
          core::conjunction<core::is_invocable_r<R, F, Args...>,
              core::negation<core::is_same_decayed<F, function>>>::value>>
  void reset(F&& _f) {
    reset(m_allocator, core::forward<F>(_f)).swap(*this);
  }

  template <typename F,
      typename = core::enable_if_t<
          core::conjunction<core::is_invocable_r<R, F, Args...>,
              core::negation<core::is_same_decayed<F, function>>>::value>>
  void reset(memory::allocator* _allocator, F&& _f) {
    function(_allocator, core::forward<F>(_f)).swap(*this);
  }

  void swap(function& _other) {
    if ((&_other) != this) {
      core::swap(m_allocator, _other.m_allocator);
      core::swap(m_callable, _other.m_callable);
      core::swap(m_executor, _other.m_executor);
      core::swap(m_copier, _other.m_copier);
      core::swap(m_deleter, _other.m_deleter);
    }
  }

private:
  using executor_type = R (*)(void* const*, Args...);
  using copier_type = void (*)(memory::allocator*, void* const*, void**);
  using deleter_type = void (*)(memory::allocator*, void**);

  template <typename F>
  typename core::enable_if<
      core::conjunction<core::negation<core::is_convertible<F, R (*)(Args...)>>,
          core::negation<core::is_function<F>>>::value>::type
  construct(F&& _f) {
    m_callable = m_allocator->construct<F>(core::forward<F>(_f));

    WN_RELEASE_ASSERT(m_callable, "failed to construct lambda/function object");

    m_executor = [](void* const* _f, Args... _args) -> R {
      F* const f = (*reinterpret_cast<F* const*>(_f));

      return (f->operator()(core::forward<Args>(_args)...));
    };

    m_copier = [](memory::allocator* _allocator, void* const* _f,
                   void** _ptr) -> void {
      F* const f = *reinterpret_cast<F* const*>(_f);

      (*_ptr) = _allocator->construct<F>(*f);

      WN_RELEASE_ASSERT((*_ptr), "failed to construct lambda/function object");
    };

    m_deleter = [](memory::allocator* _allocator, void** _f) -> void {
      _allocator->destroy<F>(reinterpret_cast<F*>(*_f));
    };
  }

  template <typename F>
  typename core::enable_if<
      core::disjunction<core::is_convertible<F, R (*)(Args...)>,
          core::is_function<F>>::value>::type
  construct(F&& _f) {
    m_callable = reinterpret_cast<void*>(static_cast<R (*)(Args...)>(_f));

    m_executor = [](void* const* _f, Args... _args) -> R {
      return ((*reinterpret_cast<R (*const*)(Args...)>(_f))(
          core::forward<Args>(_args)...));
    };

    m_copier = [](memory::allocator*, void* const* _f, void** _ptr) -> void {
      (*_ptr) = (*_f);
    };

    m_deleter = [](memory::allocator*, void**) -> void {};
  }

  void clear() {
    m_allocator = nullptr;
    m_callable = nullptr;
    m_executor = nullptr;
    m_copier = nullptr;
    m_deleter = nullptr;
  }

  memory::allocator* m_allocator;
  void* m_callable;
  executor_type m_executor;
  copier_type m_copier;
  deleter_type m_deleter;
};

template <typename R, typename... Args>
inline bool operator==(const function<R(Args...)>& _f, const nullptr_t) {
  return (!_f);
}

template <typename R, typename... Args>
inline bool operator==(const nullptr_t, const function<R(Args...)>& _f) {
  return (_f == nullptr);
}

template <typename R, typename... Args>
inline bool operator!=(const function<R(Args...)>& _f, const nullptr_t) {
  return static_cast<bool>(_f);
}

template <typename R, typename... Args>
inline bool operator!=(const nullptr_t, const function<R(Args...)>& _f) {
  return (_f != nullptr);
}

}  // namespace functional
}  // namespace wn

#endif  // __WN_FUNCTIONAL_FUNCTION_H__
