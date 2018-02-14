// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_SEMAPHORE_H__
#define __WN_MULTI_TASKING_SEMAPHORE_H__

#include "WNCore/inc/WNAssert.h"
#include "WNMultiTasking/inc/internal/semaphore_base.h"

namespace wn {
namespace multi_tasking {

class semaphore final : internal::semaphore_base {
public:
  semaphore() : semaphore(0u) {}

  semaphore(const uint16_t _count) : base(_count) {}

  semaphore(semaphore&& _other) : base(core::move(_other)) {}

  ~semaphore() = default;

  semaphore& operator=(semaphore&& _other) {
    semaphore(core::move(_other)).swap(*this);

    return *this;
  }

  void wait() {
    base::wait();
  }

  bool try_wait() {
    return base::try_wait();
  }

  void notify() {
    notify(1);
  }

  void notify(const uint16_t _count) {
    WN_DEBUG_ASSERT(
        _count != 0u, "notified a semaphore object with a count of 0");

    base::notify(_count);
  }

  void swap(semaphore& _other) {
    base::swap(_other);
  }

private:
  using base = internal::semaphore_base;
};

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_SEMAPHORE_H__
