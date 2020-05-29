// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMultiTasking/inc/fiber.h"

#if defined _WN_ANDROID || defined _WN_POSIX
#include "WNMemory/inc/manipulation.h"
#endif

#if defined _WN_WINDOWS
#define __FIBER_ARGUMENT_TYPE PVOID
#define __FIBER_RESULT_TYPE VOID
#define WN_FIBER_CALL_BEGIN CALLBACK
#define WN_FIBER_CALL_END
#elif defined _WN_ANDROID
#include "WNMultiTasking/src/context.h"
#define __FIBER_ARGUMENT_TYPE void*
#define __FIBER_RESULT_TYPE void
#elif defined _WN_POSIX || defined _WN_MACOS
#if defined _WN_MACOS
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <ucontext.h>
#define __FIBER_ARGUMENT_TYPE void
#define __FIBER_RESULT_TYPE void
#endif

#if defined _WN_POSIX
#define WN_FIBER_CALL_BEGIN WN_CDECL_BEGIN
#define WN_FIBER_CALL_END WN_CDECL_END
#endif

namespace wn {
namespace multi_tasking {

namespace {
static __FIBER_RESULT_TYPE WN_FIBER_CALL_BEGIN wrapper(
    __FIBER_ARGUMENT_TYPE) WN_FIBER_CALL_END {
  wn::multi_tasking::fiber* self = this_fiber::get_self();

  WN_RELEASE_ASSERT(self, "invalid fiber");

  self->execute_function();
}
}  // namespace

void fiber::create(
    const size_t _stack_size, functional::function<void()>&& _f) {
  memory::unique_ptr<fiber_data> data(
      memory::make_unique<fiber_data>(m_allocator));

  if (data) {
    data->m_fiber = this;
    data->m_function = std::move(_f);

#if defined _WN_WINDOWS
    const SIZE_T stack_size = static_cast<SIZE_T>(_stack_size);
    const LPVOID context =
        ::CreateFiberEx(stack_size, stack_size, FIBER_FLAG_FLOAT_SWITCH,
            static_cast<LPFIBER_START_ROUTINE>(&wrapper), NULL);
    if (context) {
      m_fiber_context = context;
      m_data = std::move(data);
    }
#elif defined _WN_ANDROID
    m_stack_pointer = m_allocator->allocate(_stack_size);
    m_fiber_context = memory::make_unique<ucontext_t>(m_allocator);
    memory::memory_zero(m_fiber_context.get());

    wn_get_context(m_fiber_context.get());

    m_fiber_context->uc_stack.ss_sp = m_stack_pointer;
    m_fiber_context->uc_stack.ss_size = _stack_size;

    wn_make_context(m_fiber_context.get(), &wrapper, NULL);

    m_data = std::move(data);
#elif defined _WN_POSIX
    m_stack_pointer = m_allocator->allocate(_stack_size);
    m_fiber_context = memory::make_unique<ucontext_t>(m_allocator);

    memory::memory_zero(m_fiber_context.get());

    getcontext(m_fiber_context.get());

    m_fiber_context->uc_stack.ss_sp = m_stack_pointer;
    m_fiber_context->uc_stack.ss_size = _stack_size;

    makecontext(m_fiber_context.get(), &wrapper, 0);

    m_data = std::move(data);
#endif
  }
}

void fiber::execute_function() {
  WN_RELEASE_ASSERT(m_data, "invalid fiber data");
  m_data->m_function();
}

namespace this_fiber {

namespace {
#ifdef _WN_WINDOWS
thread_local LPVOID tl_thread_as_fiber = 0;
#endif
thread_local fiber::fiber_data* tl_this_fiber = 0;
}  // anonymous namespace

void* get_local_storage() {
  return get_self()->m_fiber_local_data;
}

void set_local_storage(void* _dat) {
  get_self()->m_fiber_local_data = _dat;
}

void convert_to_fiber(memory::allocator* _allocator) {
  WN_DEBUG_ASSERT(_allocator, "Invalid allocator, can't be nullptrs");

  if (tl_this_fiber == 0) {
    fiber* f = _allocator->construct<fiber>(_allocator);

    f->m_is_top_level_fiber = true;
    tl_this_fiber = f->m_data.get();
#if defined _WN_WINDOWS
    f->m_fiber_context = tl_thread_as_fiber ? tl_thread_as_fiber
                                            : ::ConvertThreadToFiberEx(nullptr,
                                                  FIBER_FLAG_FLOAT_SWITCH);
    tl_thread_as_fiber = f->m_fiber_context;
#elif defined _WN_ANDROID
    f->m_fiber_context = memory::make_unique<ucontext_t>(_allocator);
    wn_get_context(f->m_fiber_context.get());
#elif defined _WN_POSIX
    f->m_fiber_context = memory::make_unique<ucontext_t>(_allocator);
    getcontext(f->m_fiber_context.get());
#endif
  }
}

void revert_from_fiber() {
  WN_DEBUG_ASSERT(tl_this_fiber->m_fiber->m_is_top_level_fiber,
      "You cannot revert a non-top-level fiber");
  memory::allocator* alloc = tl_this_fiber->m_fiber->m_allocator;
  fiber* f = tl_this_fiber->m_fiber;
  f->~fiber();
  alloc->deallocate(f);
  tl_this_fiber = nullptr;
}

fiber* get_self() {
  WN_DEBUG_ASSERT(
      tl_this_fiber != 0, "Call convert_to_fiber before calling get_self");
  fiber* fiber = tl_this_fiber->m_fiber;
  WN_DEBUG_ASSERT(fiber, "Inconsistent state detected.");
  // We do this instead of just setting tl_this_fiber to a
  // fiber* so that non-running fibers can safely be
  // moved around in containers.
  return fiber;
}

void swap_to(fiber* _fiber) {
  WN_DEBUG_ASSERT(get_self(), "Error cannot  swap from a non-fiber");
#ifndef _WN_WINDOWS
  fiber* this_fiber = get_self();
#endif
  tl_this_fiber = _fiber->m_data.get();

#if defined _WN_WINDOWS
  SwitchToFiber(_fiber->m_fiber_context);
#elif defined _WN_ANDROID
  wn_swap_context(
      this_fiber->m_fiber_context.get(), _fiber->m_fiber_context.get());
#elif defined _WN_POSIX
  swapcontext(this_fiber->m_fiber_context.get(), _fiber->m_fiber_context.get());
#endif
  // We don't have to restore tl_this_fiber here, because
  // the only way to get here is for someone to call swap_to.
}

}  // namespace this_fiber
}  // namespace multi_tasking
}  // namespace wn
