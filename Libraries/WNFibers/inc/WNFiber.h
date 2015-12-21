// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNCore/inc/WNUtility.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNContainers/inc/WNFunction.h"

#ifdef _WN_WINDOWS
#define WN_FIBER_CALL_BEGIN CALLBACK
#define WN_FIBER_CALL_END
#endif

#ifdef _WN_POSIX
#include <ucontext.h>
#endif


namespace wn {
namespace fibers {

class fiber;
class fiber_id final {};

namespace this_fiber {

fiber* get_self(wn::memory::allocator* _allocator = wn_nullptr);

fiber_id get_id() { return fiber_id(); }

void* get_local_storage();

void set_local_storage(void*);

void swap_to(fiber* _fiber);

}  // namespace this_fiber

class fiber final : public core::non_copyable {
public:
  fiber() {}
  WN_FORCE_INLINE fiber(fiber&& _other) {
    WN_UNUSED_ARGUMENT(_other);
  }

  template <typename F, typename... Args>
  WN_FORCE_INLINE explicit fiber(F&&, Args&&...) {}

  template <typename F, typename... Args>
  WN_FORCE_INLINE explicit fiber(const size_t _stack_size,
      memory::allocator* _allocator, F&& _f, Args&&... _args) {
    execute(_stack_size, containers::function<void()>(std::bind(
                             core::decay_copy(std::forward<F>(f)),
                             core::decay_copy(std::forward<Args>(args))...)));
  }

  WN_FORCE_INLINE fiber& operator=(fiber&& _other) {
    fiber(core::move(_other)).swap(*this);

    return *this;
  }

  WN_FORCE_INLINE void swap(fiber& _other) {
    core::swap(m_data, _other.m_data);
  }

  struct fiber_data final {
    containers::function<void()> m_function;
    fiber* m_fiber;
  };

private:

  friend fiber* this_fiber::get_self(wn::memory::allocator* _allocator);
  friend void this_fiber::swap_to(fiber*);
  friend void* this_fiber::get_local_storage();
  friend void this_fiber::set_local_storage(void*);

#ifdef _WN_WINDOWS
  LPVOID m_fiber_context;
  typedef PVOID fiber_argument_type;
  typedef VOID fiber_result_type;
#elif defined _WN_POSIX
  ucontext_t m_fiber_context;
  typedef void* fiber_argument_type;
  typedef void fiber_result_type;
#endif
  void* m_fiber_local_data;

  friend void swap_to(fiber*);

  void execute(const size_t _stack_size, containers::function<void()>&& _f);

  static fiber_result_type WN_FIBER_CALL_BEGIN wrapper(
      fiber_argument_type) WN_FIBER_CALL_END {
    fiber* self = this_fiber::get_self();

    WN_RELEASE_ASSERT_DESC(self, "invalid fiber");
    WN_RELEASE_ASSERT_DESC(self->m_data, "invalid fiber data");

    self->m_data->m_function();
  }

  memory::unique_ptr<fiber_data> m_data;
};

}  // namespace fibers
}  // namespace wn
