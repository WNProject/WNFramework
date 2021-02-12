// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MULTI_TASKING_WINDOWS_INTERNAL_THREAD_BASE_H__
#define __WN_MULTI_TASKING_WINDOWS_INTERNAL_THREAD_BASE_H__

#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/intrusive_ptr.h"
#include "WNMultiTasking/inc/call_once.h"
#include "WNMultiTasking/inc/internal/thread_base_common.h"
#include "WNUtilities/inc/handle.h"
#include "core/inc/utilities.h"

#include <chrono>

namespace wn {
namespace multi_tasking {
namespace internal {

class thread_base : protected thread_base_common {
public:
  class id_base;

protected:
  using base = thread_base_common;

  struct private_data : memory::intrusive_ptr_base {
    private_data(memory::allocator* _allocator)
      : memory::intrusive_ptr_base(_allocator) {}

    utilities::internal::handle m_handle;
    DWORD m_id;
  };

  using private_execution_data = private_execution_data_base<private_data>;

  thread_base() : base() {}

  thread_base(thread_base&& _other)
    : base(), m_data(core::move(_other.m_data)) {}

  id_base get_id() const;

  void join() {
    const DWORD result =
        ::WaitForSingleObject(m_data->m_handle.value(), INFINITE);

    WN_RELEASE_ASSERT(
        result == WAIT_OBJECT_0, "failed to wait on thread handle");
  }

  bool create(const attributes& _attributes, private_data* _data,
      private_execution_data* _execution_data) {
    size_t stack_size = _attributes.stack_size;

    if (stack_size != 0) {
      SYSTEM_INFO system_info;

      ::GetSystemInfo(&system_info);

      const size_t page_size = static_cast<size_t>(system_info.dwPageSize);
      const size_t remainder = stack_size % page_size;

      if (remainder != 0) {
        stack_size += page_size - remainder;
      }
    }

    DWORD id = 0;
    const DWORD flags = _attributes.low_priority ? CREATE_SUSPENDED : 0;
    const HANDLE handle = ::CreateThread(NULL, static_cast<SIZE_T>(stack_size),
        static_cast<LPTHREAD_START_ROUTINE>(&wrapper),
        static_cast<LPVOID>(_execution_data), flags, &id);

    if (handle == NULL) {
      return false;
    }

    if (_attributes.low_priority) {
      if (::SetThreadPriority(handle, THREAD_PRIORITY_LOWEST) != TRUE) {
        return false;
      }

      const int result = ::ResumeThread(handle);

      if (result == static_cast<DWORD>(-1) || result > 1) {
        return false;
      }
    }

    _data->m_handle = handle;
    _data->m_id = id;

    return true;
  }

  memory::intrusive_ptr<private_data> m_data;

private:
  static DWORD WINAPI wrapper(LPVOID _arguments) {
    private_execution_data* execution_data =
        static_cast<private_execution_data*>(_arguments);

    WN_RELEASE_ASSERT(execution_data, "invalid thread execution data");

    execute(execution_data);

    memory::allocator* allocator = execution_data->m_allocator;

    allocator->destroy(execution_data);

    return 0;
  }
};

thread_base::id_base get_id();

class thread_base::id_base : core::non_copyable {
protected:
  friend class thread_base;
  friend thread_base::id_base get_id();

  id_base() : m_id(0) {}

  id_base(id_base&& _other) : m_id(core::move(_other.m_id)) {
    _other.m_id = 0;
  }

  bool equal(const id_base& _other) const {
    return (m_id == _other.m_id);
  }

  bool less_than(const id_base& _other) const {
    return (m_id < _other.m_id);
  }

  bool greater_than(const id_base& _other) const {
    return (m_id > _other.m_id);
  }

private:
  id_base(const DWORD _id) : m_id(_id) {}

  DWORD m_id;
};

inline thread_base::id_base thread_base::get_id() const {
  return (m_data ? id_base(m_data->m_id) : id_base());
}

inline thread_base::id_base get_id() {
  return thread_base::id_base(::GetCurrentThreadId());
}

inline void yield() {
  ::SwitchToThread();
}

template <typename Representation, typename Period>
inline void sleep_for(
    const std::chrono::duration<Representation, Period>& _duration) {
  ::Sleep(static_cast<DWORD>(
      std::chrono::duration_cast<std::chrono::milliseconds>(_duration)
          .count()));
}

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_WINDOWS_INTERNAL_THREAD_BASE_H__
