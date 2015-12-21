// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFibers/inc/WNFiber.h"
#include "WNMemory/inc/WNBasic.h"

#if defined _WN_ANDROID
#include "WNFibers/src/Android/WNContext.h"
#elif defined _WN_POSIX
#include <ucontext.h>
#endif

namespace wn {
namespace fibers {

void fiber::execute(
    const size_t _stack_size, containers::function<void()>&& _f) {
  memory::unique_ptr<fiber_data> data(memory::make_unique<fiber_data>());

  if (data) {
    data->m_function = std::move(_f);

#ifdef _WN_WINDOWS
    const SIZE_T stack_size = static_cast<SIZE_T>(_stack_size);
    const LPVOID context =
        ::CreateFiberEx(stack_size, stack_size, FIBER_FLAG_FLOAT_SWITCH,
            static_cast<LPFIBER_START_ROUTINE>(&wrapper), NULL);

    if (context) {
      m_data = std::move(data);
    }
#elif _WN_ANDROID

#else

#endif
  }
}

namespace this_fiber {

namespace {
WN_THREAD_LOCAL fiber::fiber_data* tl_this_fiber = 0;
}  // anonymous namespace

void* get_local_storage() {
  return get_self()->m_fiber_local_data;
}

void set_local_storage(void* _dat) {
  get_self()->m_fiber_local_data = _dat;
}

fiber* get_self(wn::memory::allocator* _allocator) {
  if (tl_this_fiber == 0) {
    fiber* f;
    if (_allocator) {
      f = _allocator->make_allocated<fiber>(_allocator);
    } else {
      f = wn::memory::construct<fiber>(_allocator);
    }
    tl_this_fiber = f->m_data.get();

#if defined _WN_WINDOWS
    f->m_fiber_context =
        ConvertThreadToFiberEx(wn_nullptr, FIBER_FLAG_FLOAT_SWITCH);
#elif defined _WN_ANDROID
    wn_getcontext(&f->m_fiber_context);
#elif defined _WN_POSIX
    getcontext(&f->m_fiber_context);
#endif
  }
  // We do this instead of just setting tl_this_fiber to a
  // fiber* so that non-running fibers can safely be
  // moved around in containers.
  return tl_this_fiber->m_fiber;
}

void swap_to(fiber* _fiber) {
#ifndef _WN_WINDOWS
  fiber* this_fiber = get_self();
#endif
  tl_this_fiber = _fiber->m_data.get();

#if defined _WN_WINDOWS
  SwitchToFiber(_fiber->m_fiber_context);
#elif defined _WN_ANDROID
  wn_swapcontext(&this_fiber->m_fiber_context, &_fiber->m_fiber_context);
#elif defined _WN_POSIX
  swapcontext(&this_fiber->m_fiber_context, &_fiber->m_fiber_context);
#endif
  // We don't have to restore tl_this_fiber here, because
  // the only way to get here is for someone to call swap_to.
}

}  // namespace this_fiber
}  // namespace fibers
}  // namespace wn
