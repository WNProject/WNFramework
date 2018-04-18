// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_INTERNAL_THREAD_BASE_COMMON_H__
#define __WN_MULTI_TASKING_INTERNAL_THREAD_BASE_COMMON_H__

#include "WNCore/inc/WNAssert.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNMemory/inc/intrusive_ptr.h"
#include "WNMultiTasking/inc/semaphore.h"

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace multi_tasking {
namespace internal {

class thread_base_common : core::non_copyable {
protected:
  struct attributes final {
    size_t stack_size;
    bool low_priority;
  };

  template <typename T>
  struct private_execution_data_base {
    private_execution_data_base(memory::allocator* _allocator,
        semaphore* _start_lock, functional::function<void()>&& _function,
        const memory::intrusive_ptr<T>& _data)
      : m_allocator(_allocator),
        m_start_lock(_start_lock),
        m_function(core::move(_function)),
        m_data(_data) {}

    memory::allocator* m_allocator;
    semaphore* m_start_lock;
    functional::function<void()> m_function;
    memory::intrusive_ptr<T> m_data;
  };

  template <typename T>
  static void execute(private_execution_data_base<T>* _execution_data) {
    _execution_data->m_start_lock->notify();
    _execution_data->m_function();
  }
};

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_INTERNAL_THREAD_BASE_COMMON_H__
