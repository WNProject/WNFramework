// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_FIBER_H__
#define __WN_MULTI_TASKING_FIBER_H__

#include "WNContainers/inc/WNFunction.h"
#include "WNCore/inc/WNUtility.h"
#include "WNMemory/inc/WNUniquePtr.h"

#ifdef _WN_POSIX
#include <ucontext.h>
#endif

namespace wn {
namespace multi_tasking {

class fiber;
class fiber_id final {};

static const size_t default_fiber_stack_size = (1024 * 1024);

namespace this_fiber {

fiber* get_self();
void convert_to_fiber(wn::memory::allocator* _allocator);
void revert_from_fiber();

WN_FORCE_INLINE fiber_id get_id() {
  return fiber_id();
}

void* get_local_storage();
void set_local_storage(void*);
void swap_to(fiber* _fiber);

}  // namespace this_fiber

class fiber final : public core::non_copyable {
public:
  fiber() : m_is_top_level_fiber(false), m_allocator(nullptr) {
#ifdef _WN_WINDOWS
    m_fiber_context = nullptr;
#elif defined _WN_POSIX
    m_stack_pointer = nullptr;
#endif
  }

  ~fiber() {
#ifdef _WN_WINDOWS
    if (m_fiber_context && !m_is_top_level_fiber) {
      ::DeleteFiber(m_fiber_context);
    }
#elif defined _WN_POSIX
    if (m_stack_pointer) {
      m_allocator->deallocate(m_stack_pointer);
    }
#endif
  }

  WN_FORCE_INLINE fiber(memory::allocator* _allocator)
    : m_is_top_level_fiber(false), m_allocator(_allocator) {
#ifdef _WN_WINDOWS
    m_fiber_context = nullptr;
#elif defined _WN_POSIX
    m_stack_pointer = nullptr;
#endif
    m_data = memory::make_unique<fiber_data>(_allocator);
    WN_DEBUG_ASSERT_DESC(m_data, "fiber data creation failed");
    m_data->m_fiber = this;
  }

  WN_FORCE_INLINE fiber(fiber&& _other) : fiber() {
    _other.swap(*this);
  }

  template <typename F, typename... Args>
  WN_FORCE_INLINE explicit fiber(
      memory::allocator* _allocator, F&& _f, Args&&... _args)
    : m_is_top_level_fiber(false), m_allocator(_allocator) {
    create(default_fiber_stack_size,
        containers::function<void()>(
               std::bind(core::decay_copy(std::forward<F>(_f)),
                   core::decay_copy(std::forward<Args>(_args))...)));
  }

  template <typename F, typename... Args>
  WN_FORCE_INLINE explicit fiber(const size_t _stack_size,
      memory::allocator* _allocator, F&& _f, Args&&... _args)
    : m_is_top_level_fiber(false), m_allocator(_allocator) {
    create(_stack_size, containers::function<void()>(std::bind(
                            core::decay_copy(std::forward<F>(_f)),
                            core::decay_copy(std::forward<Args>(_args))...)));
  }

  WN_FORCE_INLINE fiber& operator=(fiber&& _other) {
    // Clear out this.
    m_data.release();
    m_allocator = nullptr;
    m_is_top_level_fiber = false;

#ifdef _WN_WINDOWS
    if (m_fiber_context && !m_is_top_level_fiber) {
      ::DeleteFiber(m_fiber_context);
    }
    m_fiber_context = nullptr;
#elif defined _WN_POSIX
    if (m_stack_pointer) {
      m_allocator->deallocate(m_stack_pointer);
    }
    m_stack_pointer = nullptr;
#endif

    _other.swap(*this);

    return *this;
  }

  WN_FORCE_INLINE void swap(fiber& _other) {
    core::swap(m_data, _other.m_data);
    core::swap(m_is_top_level_fiber, _other.m_is_top_level_fiber);
    core::swap(m_allocator, _other.m_allocator);

#if defined _WN_POSIX
    core::swap(m_stack_pointer, _other.m_stack_pointer);
    core::swap(m_fiber_context, _other.m_fiber_context);
#elif defined _WN_WINDOWS
    core::swap(m_fiber_context, _other.m_fiber_context);
#endif

    if (m_data) {
      m_data->m_fiber = this;
    }

    if (_other.m_data) {
      _other.m_data->m_fiber = &_other;
    }
  }

  struct fiber_data final {
    containers::function<void()> m_function;
    fiber* m_fiber;
  };

  void execute_function();

private:
  friend void this_fiber::convert_to_fiber(wn::memory::allocator*);
  friend void this_fiber::revert_from_fiber();
  friend fiber* this_fiber::get_self();
  friend void this_fiber::swap_to(fiber*);
  friend void* this_fiber::get_local_storage();
  friend void this_fiber::set_local_storage(void*);

#ifdef _WN_WINDOWS
  LPVOID m_fiber_context;
#elif defined _WN_POSIX
  ucontext_t m_fiber_context;
  void* m_stack_pointer;
#endif

  void* m_fiber_local_data;
  void create(const size_t _stack_size, containers::function<void()>&& _f);

  bool m_is_top_level_fiber;
  memory::allocator* m_allocator;
  memory::unique_ptr<fiber_data> m_data;
};

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_FIBER_H__
