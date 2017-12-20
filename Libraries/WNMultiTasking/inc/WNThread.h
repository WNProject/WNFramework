// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_THREAD_H__
#define __WN_MULTI_TASKING_THREAD_H__

#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNUtility.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNMultiTasking/inc/semaphore.h"

#if defined _WN_WINDOWS
#include "WNUtilities/inc/handle.h"

#include <mutex>
#elif defined _WN_POSIX
#include <pthread.h>
#include <unistd.h>

#ifdef _WN_LINUX
#include <sys/syscall.h>
#elif defined _WN_ANDROID
#include <sys/types.h>
#endif
#endif

#include <chrono>

namespace wn {
namespace multi_tasking {

class thread;
class thread_id;

namespace this_thread {

thread_id get_id();

}  // namespace this_thread

class thread_id final {
public:
  WN_FORCE_INLINE thread_id() {
#ifdef _WN_WINDOWS
    m_id = 0;
#elif defined _WN_POSIX
    m_valid = false;
#endif
  }

private:
  friend class thread;

  friend thread_id this_thread::get_id();

  friend bool operator==(const thread_id&, const thread_id&);

#ifdef _WN_WINDOWS
  typedef DWORD native_id_type;
#elif defined _WN_POSIX
  typedef pid_t native_id_type;
#endif

  WN_FORCE_INLINE thread_id(const native_id_type id) : m_id(id) {
#ifdef _WN_POSIX
    m_valid = true;
#endif
  }

#ifdef _WN_WINDOWS
  DWORD m_id;
#elif defined _WN_POSIX
  pid_t m_id;
  bool m_valid;
#endif
};

WN_FORCE_INLINE bool operator==(const thread_id& lhs, const thread_id& rhs) {
#ifdef _WN_WINDOWS
  return (lhs.m_id == rhs.m_id);
#elif defined _WN_POSIX
  return (static_cast<uint64_t>(lhs.m_id) == static_cast<uint64_t>(rhs.m_id));
#endif
}

WN_FORCE_INLINE bool operator!=(const thread_id& _lhs, const thread_id& _rhs) {
  return (!(_lhs == _rhs));
}

class thread final : public core::non_copyable {
public:
  thread() = default;

  WN_FORCE_INLINE thread(thread&& _other) : m_data(core::move(_other.m_data)) {}

  template <typename F, typename... Args>
  WN_FORCE_INLINE explicit thread(
      memory::allocator* _allocator, F&& _f, Args&&... _args) {
    static_assert(core::is_void<core::result_of_t<F(Args...)>>::value,
        "function must not be void return");

    WN_RELEASE_ASSERT_DESC(_allocator, "allocator must not be nullptr");

    create(_allocator, functional::function<void()>(std::bind(
                           core::decay_copy(core::forward<F>(_f)),
                           core::decay_copy(core::forward<Args>(_args))...)));
  }

  WN_FORCE_INLINE thread& operator=(thread&& _other) {
    thread(core::move(_other)).swap(*this);

    return *this;
  }

  WN_FORCE_INLINE bool joinable() const {
    return (m_data && !m_data->m_joined);
  }

  WN_FORCE_INLINE thread_id get_id() const {
    if (m_data) {
      return thread_id(m_data->m_id);
    }

    return thread_id();
  }

  WN_FORCE_INLINE bool join() const {
    if (m_data) {
      if (!m_data->m_joined) {
        m_data->m_joined = true;

#ifdef _WN_WINDOWS
        const DWORD result =
            ::WaitForSingleObject(m_data->m_handle.value(), INFINITE);

        if (result == WAIT_OBJECT_0) {
          return true;
        }
#elif defined _WN_POSIX
        if (::pthread_join(m_data->m_pthread, NULL) == 0) {
          return true;
        }
#endif
      }
    }

    return false;
  }

  WN_FORCE_INLINE void detach() {
    m_data.reset();
  }

  WN_FORCE_INLINE void swap(thread& other) {
    m_data.swap(other.m_data);
  }

private:
  struct private_data : memory::intrusive_ptr_base {
    WN_FORCE_INLINE private_data(memory::allocator* _allocator)
      : memory::intrusive_ptr_base(_allocator),
#ifdef _WN_WINDOWS
        m_id(0),
#endif
        m_joined(false) {
    }

#ifdef _WN_WINDOWS
    utilities::internal::handle m_handle;
    DWORD m_id;
#elif defined _WN_POSIX
    semaphore m_start_lock;
    pthread_t m_pthread;
    pid_t m_id;
#endif

    bool m_joined;
  };

  struct private_execution_data final {
    WN_FORCE_INLINE private_execution_data(memory::allocator* _allocator,
        functional::function<void()>&& _function,
        const memory::intrusive_ptr<private_data>& _data)
      : m_allocator(_allocator),
        m_function(core::move(_function)),
        m_data(_data) {}

    memory::allocator* m_allocator;
    functional::function<void()> m_function;
    memory::intrusive_ptr<private_data> m_data;
  };

  void create(memory::allocator* _allocator, functional::function<void()>&& _f);

#ifdef _WN_WINDOWS
  using argument_type = LPVOID;
  using result_type = DWORD;
#elif defined _WN_POSIX
  using argument_type = void*;
  using result_type = void*;
#endif

  static result_type WN_OSCALL_BEGIN wrapper(argument_type _arg) WN_OSCALL_END;

  memory::intrusive_ptr<private_data> m_data;
};

namespace this_thread {

WN_INLINE thread_id get_id() {
#ifdef _WN_WINDOWS
  return thread_id(::GetCurrentThreadId());
#elif defined _WN_LINUX
  return thread_id(static_cast<pid_t>(::syscall(SYS_gettid)));
#elif defined _WN_ANDROID
  return thread_id(::gettid());
#endif
}

WN_INLINE void yield() {
#ifdef _WN_WINDOWS
  static std::once_flag once;
  static bool mutli_threaded = false;
  static const auto mutli_threaded_test = [](bool& mutli_threaded) {
    SYSTEM_INFO sysInfo = {0};

    ::GetSystemInfo(&sysInfo);

    mutli_threaded = sysInfo.dwNumberOfProcessors > 1;
  };

  std::call_once(once, mutli_threaded_test, std::ref(mutli_threaded));

  if (mutli_threaded) {
    ::YieldProcessor();
  } else {
    ::SwitchToThread();
  }
#elif defined _WN_ANDROID
  const int result = ::sched_yield();

  WN_RELEASE_ASSERT(result == 0);
#elif defined _WN_POSIX
  const int result = ::pthread_yield();

  WN_RELEASE_ASSERT(result == 0);
#endif
}

template <typename Rep, typename Period>
WN_INLINE void sleep_for(const std::chrono::duration<Rep, Period>& duration) {
#ifdef _WN_WINDOWS
  ::Sleep(static_cast<DWORD>(
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()));
#elif defined _WN_POSIX
  const std::chrono::seconds sec =
      std::chrono::duration_cast<std::chrono::seconds>(duration);
  const std::chrono::nanoseconds nano_sec =
      std::chrono::duration_cast<std::chrono::nanoseconds>(duration) -
      std::chrono::duration_cast<std::chrono::nanoseconds>(sec);
  timespec timeRequested = {0};

  timeRequested.tv_sec = static_cast<int>(sec.count());
  timeRequested.tv_nsec = static_cast<int>(nano_sec.count());

  while (::nanosleep(&timeRequested, &timeRequested) == -1) {
    WN_RELEASE_ASSERT_DESC(errno == EINTR, "failed to sleep");
  }
#endif
}

}  // namespace this_thread
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_THREAD_H__
