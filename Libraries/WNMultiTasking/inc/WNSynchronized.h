// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_SYNCHRONIZED_H__
#define __WN_MULTI_TASKING_SYNCHRONIZED_H__

#include "WNContainers/inc/WNArray.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNUtility.h"
#include "WNMultiTasking/inc/WNJobSignal.h"

#include <atomic>

namespace wn {
namespace multi_tasking {

struct synchronization_data {
  synchronization_data() : signal(0), next_job(0) {}
  size_t increment_job() {
    return next_job.fetch_add(1, std::memory_order::memory_order_release);
  }

  job_signal signal;
  std::atomic<size_t> next_job;
};

struct synchronized_base {};

template <typename SUPER = synchronized_base>
class synchronized {
public:
  synchronization_data* get_synchronization_data() {
    return &data;
  }

private:
  synchronization_data data;
};

template <typename D>
struct is_synchronized {
  using U =
      typename std::remove_cv<typename std::remove_reference<D>::type>::type;

  template <typename T>
  static auto is_same(synchronized<T>*) -> typename core::integral_constant<bool,
      !core::is_same<U, synchronized<T>>::value>;

  static core::false_type is_same(void*);

  using type = decltype(is_same(std::declval<U*>()));
};

template <typename T>
using is_synchronized_t = typename is_synchronized<T>::type;

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_SYNCHRONIZED_H__
