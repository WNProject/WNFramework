// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_WINDOWS_INTERNAL_THREAD_BASE_H__
#define __WN_MULTI_TASKING_WINDOWS_INTERNAL_THREAD_BASE_H__

#include "WNCore/inc/WNUtility.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNMultiTasking/inc/call_once.h"
#include "WNMultiTasking/inc/internal/thread_base_common.h"
#include "WNUtilities/inc/handle.h"

#include <chrono>

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace multi_tasking {
namespace internal {

class thread_base : protected thread_base_common {
public:
  class id_base;

protected:
  struct private_data : memory::intrusive_ptr_base {
    private_data(memory::allocator* _allocator)
      : memory::intrusive_ptr_base(_allocator) {}

    utilities::internal::handle m_handle;
  };

  thread_base() = default;

  thread_base(thread_base&& _other) : m_data(core::move(_other.m_data)) {}

  id_base get_id() const;

  void join() {
    const DWORD result =
        ::WaitForSingleObject(m_data->m_handle.value(), INFINITE);

    WN_RELEASE_ASSERT(
        (result == WAIT_OBJECT_0), "failed to wait on thread handle");
  }

  bool create(private_data* _data,
      private_execution_data<private_data>* _execution_data) {
    const HANDLE handle =
        ::CreateThread(NULL, 0, static_cast<LPTHREAD_START_ROUTINE>(&wrapper),
            static_cast<LPVOID>(_execution_data), 0, NULL);
    const bool creation_success = handle != NULL;

    if (creation_success) {
      _data->m_handle = handle;
    }

    return creation_success;
  }

  memory::intrusive_ptr<private_data> m_data;

private:
  static DWORD WINAPI wrapper(LPVOID _arguments) {
    execute(convert<private_data>(static_cast<void*>(_arguments)));

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
  if (m_data && m_data->m_handle.is_valid()) {
    return id_base(::GetThreadId(m_data->m_handle.value()));
  }

  return id_base();
}

inline thread_base::id_base get_id() {
  return thread_base::id_base(::GetCurrentThreadId());
}

inline void yield() {
  static once_flag once;
  static bool multi_threaded = false;
  static const auto multi_threaded_test = [](bool* multi_threaded) {
    SYSTEM_INFO sysInfo = {0};

    ::GetSystemInfo(&sysInfo);

    *multi_threaded = sysInfo.dwNumberOfProcessors > 1;
  };

  call_once(once, multi_threaded_test, &multi_threaded);

  multi_threaded ? ::YieldProcessor() : ::SwitchToThread();
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
