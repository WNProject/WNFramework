// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_POSIX_INTERNAL_SEMAPHORE_BASE_H__
#define __WN_MULTI_TASKING_POSIX_INTERNAL_SEMAPHORE_BASE_H__

#include "WNCore/inc/assert.h"
#include "WNCore/inc/utilities.h"

#include <errno.h>
#include <semaphore.h>

namespace wn {
namespace multi_tasking {
namespace internal {

class semaphore_base : core::non_copyable {
protected:
  semaphore_base() = delete;

  semaphore_base(uint32_t _count) {
    const int result =
        ::sem_init(&m_semaphore, 0, static_cast<unsigned int>(_count));

    WN_RELEASE_ASSERT(result == 0, "failed to create semaphore object");

    m_valid = true;
  }

  semaphore_base(semaphore_base&& _other) {
    m_semaphore = _other.m_semaphore;
    m_valid = _other.m_valid;
    _other.m_valid = false;
  }

  ~semaphore_base() {
    if (m_valid) {
      const int result = ::sem_destroy(&m_semaphore);

      WN_DEBUG_ASSERT(result == 0, "failed to destroy semaphore object");

      (void)result;
    }
  }

  void wait() {
    int result;

    while ((result = ::sem_wait(&m_semaphore)) == -1) {
      WN_RELEASE_ASSERT(errno == EINTR, "failed to wait on semaphore object");
    }
  }

  bool try_wait() {
    int result;

    while ((result = ::sem_trywait(&m_semaphore)) == -1 && errno != EAGAIN) {
      WN_RELEASE_ASSERT(
          errno == EINTR, "failed to try to wait on semaphore object");
    }

    return (result == 0);
  }

  void notify(uint32_t _count) {
    while (_count-- != 0) {
      const int result = ::sem_post(&m_semaphore);

      WN_RELEASE_ASSERT(
          result == 0, "failed to post desired count to semaphore object");
    }
  }

  void swap(semaphore_base& _other) {
    core::swap(m_semaphore, _other.m_semaphore);
    core::swap(m_valid, _other.m_valid);
  }

private:
  sem_t m_semaphore;
  bool m_valid;
};

}  // namespace internal
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_POSIX_INTERNAL_SEMAPHORE_BASE_H__
