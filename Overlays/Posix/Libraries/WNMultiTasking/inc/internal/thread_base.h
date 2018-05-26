// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_POSIX_INTERNAL_THREAD_BASE_H__
#define __WN_MULTI_TASKING_POSIX_INTERNAL_THREAD_BASE_H__

#include "WNCore/inc/WNUtility.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/intrusive_ptr.h"
#include "WNMultiTasking/inc/internal/system_thread_functions.h"
#include "WNMultiTasking/inc/internal/thread_base_common.h"

#include <limits.h>
#include <pthread.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

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

    pthread_t m_pthread;
    pid_t m_id;
  };

  struct private_execution_data final
    : private_execution_data_base<private_data> {
    private_execution_data(memory::allocator* _allocator,
        semaphore* _start_lock, functional::function<void()>&& _function,
        const memory::intrusive_ptr<private_data>& _data)
      : private_execution_data_base<private_data>(
            _allocator, _start_lock, core::move(_function), _data),
        m_low_priority(false) {}

    bool m_low_priority;
  };

  thread_base() : base() {}

  thread_base(thread_base&& _other)
    : base(), m_data(core::move(_other.m_data)) {}

  id_base get_id() const;

  void join() {
    const int result = ::pthread_join(m_data->m_pthread, NULL);

    WN_RELEASE_ASSERT(result == 0, "failed to join thread");
  }

  bool create(const attributes& _attributes, private_data* _data,
      private_execution_data* _execution_data) {
    pthread_attr_t attributes;

    if (::pthread_attr_init(&attributes) != 0) {
      return false;
    }

    if (_attributes.stack_size != 0) {
      const long raw_page_size = ::sysconf(_SC_PAGESIZE);

      if (raw_page_size == -1) {
        return false;
      }

      size_t stack_size = _attributes.stack_size;
      const size_t page_size = static_cast<size_t>(raw_page_size);
      const size_t remainder = stack_size % page_size;

      if (remainder != 0) {
        stack_size += page_size - remainder;

        if (stack_size < PTHREAD_STACK_MIN ||
            ::pthread_attr_setstacksize(&attributes, stack_size) != 0) {
          return false;
        }
      }
    }

    _execution_data->m_low_priority = _attributes.low_priority;

    pthread_t pthread;
    const int creation_result = ::pthread_create(&pthread, &attributes,
        static_cast<void* (*)(void*)>(&wrapper),
        static_cast<void*>(_execution_data));

    if (creation_result != 0) {
      return false;
    }

    const int attribute_destroy_result = ::pthread_attr_destroy(&attributes);

    (void)attribute_destroy_result;

    WN_DEBUG_ASSERT(
        attribute_destroy_result == 0, "failed to cleanup thread attributes");

    _data->m_pthread = pthread;

    return true;
  }

  memory::intrusive_ptr<private_data> m_data;

private:
  static void* wrapper(void* _arguments) {
    private_execution_data* execution_data =
        static_cast<private_execution_data*>(_arguments);

    WN_RELEASE_ASSERT(execution_data, "invalid thread execution data");

    private_data* data = execution_data->m_data.get();

    WN_RELEASE_ASSERT(data, "invalid thread data");

    data->m_id = get_thread_id();

    if (execution_data->m_low_priority) {
      thread_adjust_priority(data->m_id);
    }

    execute(execution_data);

    memory::allocator* allocator = execution_data->m_allocator;

    allocator->destroy(execution_data);

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
  return thread_base::id_base(get_thread_id());
}

inline void yield() {
  const int result = thread_yield();

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
