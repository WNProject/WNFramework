// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_POSIX_INTERNAL_THREAD_BASE_H__
#define __WN_MULTI_TASKING_POSIX_INTERNAL_THREAD_BASE_H__

#include "WNCore/inc/WNUtility.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNMultiTasking/inc/internal/system_thread_id.h"
#include "WNMultiTasking/inc/internal/system_thread_yield.h"
#include "WNMultiTasking/inc/internal/thread_base_common.h"

#include <pthread.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

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
      : memory::intrusive_ptr_base(_allocator), m_joined(false) {}

    pthread_t m_pthread;
    pid_t m_id;
    bool m_joined;
  };

  thread_base() = default;

  thread_base(thread_base&& _other) : m_data(core::move(_other.m_data)) {}

  id_base get_id() const;

  bool joinable() const {
    return (!m_data->m_joined);
  }

  bool join() const {
    m_data->m_joined = true;

    return (::pthread_join(m_data->m_pthread, NULL) == 0);
  }

  bool create(private_data* _data,
      private_execution_data<private_data>* _execution_data) {
    typedef void* (*start_routine_t)(void*);

    pthread_t pthread;
    const bool creation_success =
        ::pthread_create(&pthread, NULL, static_cast<start_routine_t>(&wrapper),
            static_cast<void*>(_execution_data)) == 0;

    if (creation_success) {
      _data->m_pthread = pthread;
    }

    return creation_success;
  }

  memory::intrusive_ptr<private_data> m_data;

private:
  static void* wrapper(void* _arguments) {
    private_execution_data<private_data>* execution_data =
        convert<private_data>(_arguments);
    private_data* data = execution_data->m_data.get();

    WN_RELEASE_ASSERT(data, "invalid thread data");

    data->m_id = system_thread_id();

    execute(execution_data);

    return NULL;
  }
};

thread_base::id_base get_id();

class thread_base::id_base : core::non_copyable {
protected:
  friend class thread_base;
  friend thread_base::id_base get_id();

  id_base() : m_valid(false) {}

  id_base(id_base&& _other)
    : m_id(core::move(_other.m_id)), m_valid(core::move(_other.m_valid)) {
    _other.m_valid = false;
  }

  bool equal(const id_base& _other) const {
    return ((!m_valid && !_other.m_valid) ||
            (m_valid && _other.m_valid &&
                (static_cast<uint64_t>(m_id) ==
                    static_cast<uint64_t>(_other.m_id))));
  }

  bool less_than(const id_base& _other) const {
    return (m_valid && _other.m_valid &&
            (static_cast<uint64_t>(m_id) < static_cast<uint64_t>(_other.m_id)));
  }

  bool greater_than(const id_base& _other) const {
    return (m_valid && _other.m_valid &&
            (static_cast<uint64_t>(m_id) > static_cast<uint64_t>(_other.m_id)));
  }

private:
  id_base(const pid_t _id) : m_id(_id), m_valid(true) {}

  pid_t m_id;
  bool m_valid;
};

inline thread_base::id_base thread_base::get_id() const {
  return (m_data ? id_base(m_data->m_id) : id_base());
}

inline thread_base::id_base get_id() {
  return thread_base::id_base(system_thread_id());
}

inline void yield() {
  const int result = system_thread_yield();

  WN_RELEASE_ASSERT(result == 0, "failed to yield");
}

template <typename Representation, typename Period>
inline void sleep_for(
    const std::chrono::duration<Representation, Period>& _duration) {
  const std::chrono::seconds sec =
      std::chrono::duration_cast<std::chrono::seconds>(_duration);
  const std::chrono::nanoseconds nano_sec =
      std::chrono::duration_cast<std::chrono::nanoseconds>(_duration) -
      std::chrono::duration_cast<std::chrono::nanoseconds>(sec);
  timespec time_requested = {0};

  time_requested.tv_sec = static_cast<int>(sec.count());
  time_requested.tv_nsec = static_cast<int>(nano_sec.count());

  while (::nanosleep(&time_requested, &time_requested) == -1) {
    WN_RELEASE_ASSERT(errno == EINTR, "failed to sleep");
  }
}

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_POSIX_INTERNAL_THREAD_BASE_H__
