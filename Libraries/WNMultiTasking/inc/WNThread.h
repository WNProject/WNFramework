// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_THREAD_H__
#define __WN_MULTI_TASKING_THREAD_H__

#include "WNContainers/inc/WNFunction.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNUtility.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNMultiTasking/inc/WNSemaphore.h"

#if defined _WN_WINDOWS
#include <Windows.h>
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
namespace internal {

struct thread_data_common : public memory::intrusive_ptr_base {
  WN_FORCE_INLINE thread_data_common(memory::allocator* _allocator)
    : memory::intrusive_ptr_base(_allocator),
#ifdef _WN_WINDOWS
      m_handle(NULL),
      m_id(0),
#endif
      m_joined(false) {
  }

  WN_FORCE_INLINE ~thread_data_common() {
#ifdef _WN_WINDOWS
    if (m_handle != NULL) {
      const BOOL close_result = ::CloseHandle(m_handle);

      WN_DEBUG_ASSERT_DESC(
          close_result != FALSE, "failed to destory thread object");

#ifndef _WN_DEBUG
      WN_UNUSED_ARGUMENT(close_result);
#endif
    }
#endif
  }

#ifdef _WN_WINDOWS
  HANDLE m_handle;
  DWORD m_id;
#elif defined _WN_POSIX
  semaphore m_start_lock;
  pthread_t m_pthread;
  pid_t m_id;
#endif

  bool m_joined;
};

template <typename T>
struct thread_data final : public thread_data_common {
  WN_FORCE_INLINE thread_data(memory::allocator* _allocator)
    : thread_data_common(_allocator) {}

  T m_result;
};

template <>
struct thread_data<void> : public thread_data_common {
  WN_FORCE_INLINE thread_data(memory::allocator* _allocator)
    : thread_data_common(_allocator) {}
};

}  // namespace internal

class thread_id;

namespace this_thread {
thread_id get_id();
}

template <typename R>
class thread;

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
  template <typename R>
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

WN_FORCE_INLINE bool operator!=(const thread_id& lhs, const thread_id& rhs) {
  return (!(lhs == rhs));
}

template <typename R>
class thread final : public core::non_copyable {
public:
  static_assert(
      !std::is_reference<R>::value, "return type must not be a reference");
  static_assert(!std::is_const<R>::value, "return type must not be const");

  typedef R result_type;

  WN_FORCE_INLINE thread() = default;

  WN_FORCE_INLINE thread(thread&& other) : m_data(core::move(other.m_data)) {}

  template <typename F, typename... Args>
  WN_FORCE_INLINE explicit thread(
      memory::allocator* _allocator, F&& f, Args&&... args) {
    static_assert(core::is_same<R, core::result_of_t<F(Args...)>>::value,
        "function return type does not match return type");

    WN_RELEASE_ASSERT_DESC(_allocator, "allocator must not be nullptr");

    execute(_allocator, containers::function<R()>(std::bind(
                            core::decay_copy(core::forward<F>(f)),
                            core::decay_copy(core::forward<Args>(args))...)));
  }

  WN_FORCE_INLINE thread& operator=(thread&& other) {
    thread(core::move(other)).swap(*this);

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
        const DWORD result = ::WaitForSingleObject(m_data->m_handle, INFINITE);

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

  template <typename T = R,
      typename = core::enable_if_t<core::bool_and<core::is_same<T, R>::value,
          !core::is_same<T, void>::value>::value>>
  WN_FORCE_INLINE bool join(T& result) const {
    const bool join_result = join();

    if (join_result) {
      result = m_data->m_result;
    }

    return join_result;
  }

  WN_FORCE_INLINE void detach() {
    m_data.reset();
  }

  WN_FORCE_INLINE void swap(thread& other) {
    m_data.swap(other.m_data);
  }

private:
#ifdef _WN_WINDOWS
  typedef LPVOID thread_argument_type;
  typedef DWORD thread_result_type;
#elif defined _WN_POSIX
  typedef void* thread_argument_type;
  typedef void* thread_result_type;
#endif

  struct thread_execution_data final {
    WN_FORCE_INLINE thread_execution_data(memory::allocator* _allocator,
        containers::function<R()>&& _function,
        const memory::intrusive_ptr<internal::thread_data<R>>& _data)
      : m_allocator(_allocator),
        m_function(core::move(_function)),
        m_data(_data) {}

    containers::function<R()> m_function;
    memory::intrusive_ptr<internal::thread_data<R>> m_data;
    memory::allocator* m_allocator;
  };

  static thread_result_type WN_OSCALL_BEGIN wrapper(
      thread_argument_type _arg) WN_OSCALL_END {
    thread_execution_data* execution_data =
        static_cast<thread_execution_data*>(_arg);

    WN_RELEASE_ASSERT_DESC(execution_data, "invalid thread execution data");

#ifdef _WN_POSIX
    internal::thread_data<R>* data = execution_data->m_data.get();

    WN_RELEASE_ASSERT_DESC(data, "invalid thread data");

#ifdef _WN_LINUX
    data->m_id = ::syscall(SYS_gettid);
#elif defined _WN_ANDROID
    data->m_id = ::gettid();
#endif

    data->m_start_lock.notify();
#endif

    execute_helper(execution_data);

    memory::allocator* allocator = execution_data->m_allocator;

    allocator->destroy(execution_data);

#ifdef _WN_WINDOWS
    return 0;
#elif defined _WN_POSIX
    return NULL;
#endif
  }

  static WN_FORCE_INLINE void execute_helper(
      const thread_execution_data* execution_data) {
    internal::thread_data<R>* data = execution_data->m_data.get();

    WN_RELEASE_ASSERT_DESC(data, "invalid thread data");

    data->m_result = execution_data->m_function();
  }

  WN_INLINE void execute(
      memory::allocator* _allocator, containers::function<R()>&& _function) {
    memory::intrusive_ptr<internal::thread_data<R>> data(
        memory::make_intrusive<internal::thread_data<R>>(
            _allocator, _allocator));

    if (data) {
      thread_execution_data* execution_data =
          _allocator->construct<thread_execution_data>(
              _allocator, core::move(_function), data);

      if (execution_data) {
#ifdef _WN_WINDOWS
        DWORD id;
        const HANDLE handle = ::CreateThread(NULL, 0,
            static_cast<LPTHREAD_START_ROUTINE>(&wrapper),
            static_cast<LPVOID>(execution_data), 0, &id);
        const bool creation_success = handle != NULL;
#elif defined _WN_POSIX
        typedef void* (*start_routine_t)(void*);

        pthread_t pthread;
        const bool creation_success =
            ::pthread_create(&pthread, NULL,
                static_cast<start_routine_t>(&wrapper),
                static_cast<void*>(execution_data)) == 0;
#endif

        if (creation_success) {
#ifdef _WN_WINDOWS
          data->m_id = id;
          data->m_handle = handle;
#elif defined _WN_POSIX
          data->m_start_lock.wait();

          data->m_pthread = pthread;
#endif

          m_data = core::move(data);
        } else {
          _allocator->destroy(execution_data);

          WN_RELEASE_ASSERT_DESC(creation_success, "failed to create thread");
        }
      } else {
        WN_RELEASE_ASSERT_DESC(execution_data,
            "failed to allocate needed execution data for thread");
      }
    } else {
      WN_RELEASE_ASSERT_DESC(data, "failed to allocate needed data for thread");
    }
  }

  memory::intrusive_ptr<internal::thread_data<R>> m_data;
};

template <>
WN_FORCE_INLINE void thread<void>::execute_helper(
    const thread_execution_data* execution_data) {
  execution_data->m_function();
}

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
