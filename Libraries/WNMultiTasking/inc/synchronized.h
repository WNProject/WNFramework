// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MULTI_TASKING_SYNCHRONIZED_H__
#define __WN_MULTI_TASKING_SYNCHRONIZED_H__

#include "WNContainers/inc/WNArray.h"
#include "WNMultiTasking/inc/job_signal.h"
#include "core/inc/type_traits.h"
#include "core/inc/utilities.h"

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
  synchronized() : m_scheduled_for_destruction(false) {}
  virtual ~synchronized() {}

protected:
  bool scheduled_for_destruction() const {
    return m_scheduled_for_destruction;
  }

private:
  void sync_destruction() {}
  void schedule_for_destruction() {
    m_scheduled_for_destruction = true;
  }

  virtual void on_destroy(){};
  std::atomic<bool> m_scheduled_for_destruction;
  synchronization_data data;
  friend class synchronized_destroy_base;
};

template <typename D>
struct is_synchronized {
  using U =
      typename std::remove_cv<typename std::remove_reference<D>::type>::type;

  template <typename T>
  static auto is_same(synchronized<T>*) ->
      typename core::integral_constant<bool,
          !core::is_same<U, synchronized<T>>::value>;

  static core::false_type is_same(void*);

  using type = decltype(is_same(std::declval<U*>()));
};

template <typename T>
using is_synchronized_t = typename is_synchronized<T>::type;

class synchronized_destroy_base {
protected:
  void wait_for_destruction(synchronized<>* _sync);
};

template <typename T>
class synchronized_destroy : public synchronized_destroy_base {
public:
  template <typename... Args>
  synchronized_destroy(Args&&... args) : t(core::forward<Args>(args)...) {}
  ~synchronized_destroy() {
    wait_for_destruction(&t);
  }
  T* operator*() {
    return &t;
  }
  T* operator->() {
    return &t;
  }
  T* get() {
    return &t;
  }

private:
  T t;
};

template <typename T>
using sync_ptr = memory::unique_ptr<synchronized_destroy<T>>;

template <typename T, typename... Args>
sync_ptr<T> make_sync(memory::allocator* _allocator, Args&&... args) {
  return memory::make_unique<synchronized_destroy<T>>(
      _allocator, core::forward<Args>(args)...);
}

}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_SYNCHRONIZED_H__
