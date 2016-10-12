// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_THREADSAFE_CONTAINERS_OBJECT_POOL_H__
#define __WN_THREADSAFE_CONTAINERS_OBJECT_POOL_H__

#include "WNContainers/inc/WNFunction.h"
#include "WNContainers/inc/WNList.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNMultiTasking/inc/WNLockGuard.h"
#include "WNMultiTasking/inc/WNMutex.h"

namespace wn {
namespace threadsafe_containers {

template <typename T>
class object_pool;

template <typename T>
class pooled_object final {
public:
  pooled_object(pooled_object&& _other) {
    m_it = _other.m_it;
    _other.m_valid = false;
    m_pool = _other.m_pool;
  }

  ~pooled_object();

  T& operator*() {
    return *m_it;
  }
  T* operator->() {
    return &(*m_it);
  }

  const T& operator*() const {
    return *m_it;
  }

  const T* operator->() const {
    return &(*m_it);
  }

private:
  template <typename F>
  friend class object_pool;

  explicit pooled_object(
      typename containers::list<T>::iterator _it, object_pool<T>* _pool)
    : m_valid(true), m_it(_it), m_pool(_pool) {}

  bool m_valid;
  typename containers::list<T>::iterator m_it;
  object_pool<T>* m_pool;
};

template <typename T>
class object_pool {
private:
  struct copyable_object {
    copyable_object(const T& _template_object) : m_obj(_template_object) {}
    T operator()() {
      return T(m_obj);
    }

  private:
    T m_obj;
  };

  struct resettable_object {
    resettable_object(const T& _template_object) : m_obj(_template_object) {}
    void operator()(T* _val) {
      *_val = m_obj;
    }

  private:
    T m_obj;
  };

public:
  using unmanaged_object = typename containers::list<T>::iterator;
  object_pool(memory::allocator* _allocator,
      containers::function<T()> _create_function,
      containers::function<void(T*)> _reset_function)
    : m_allocator(_allocator),
      m_free_list(_allocator),
      m_used_list(_allocator),
      m_create_function(_create_function),
      m_reset_function(_reset_function) {}

  object_pool(memory::allocator* _allocator, const T& object_template)
    : object_pool(_allocator, copyable_object(object_template),
          resettable_object(object_template)) {}

  unmanaged_object get_unmanaged();
  void release_unamanged(unmanaged_object object);

  pooled_object<T> get_object();

private:
  memory::allocator* m_allocator;
  containers::list<T> m_free_list;
  containers::list<T> m_used_list;
  multi_tasking::mutex m_lock;
  containers::function<T()> m_create_function;
  containers::function<void(T*)> m_reset_function;
};

template <typename T>
typename object_pool<T>::unmanaged_object object_pool<T>::get_unmanaged() {
  multi_tasking::lock_guard<multi_tasking::mutex> guard(m_lock);
  if (!m_free_list.empty()) {
    auto val = m_free_list.begin();
    m_free_list.transfer_to(
        m_free_list.begin(), m_used_list.end(), m_used_list);
    auto obj_iter = m_used_list.end() - 1;
    m_reset_function(&(*obj_iter));
    return obj_iter;
  } else {
    m_used_list.push_back(m_create_function());
    return m_used_list.end() - 1;
  }
}

template <typename T>
void object_pool<T>::release_unamanged(
    typename object_pool<T>::unmanaged_object obj) {
  multi_tasking::lock_guard<multi_tasking::mutex> guard(m_lock);
  m_used_list.transfer_to(obj, m_free_list.end(), m_free_list);
}

template <typename T>
pooled_object<T> object_pool<T>::get_object() {
  return pooled_object<T>(get_unmanaged(), this);
}

template <typename T>
pooled_object<T>::~pooled_object() {
  if (m_valid) {
    m_pool->release_unamanged(m_it);
  }
}

}  // namespace threadsafe_containers
}  // namespace wn

#endif  // __WN_THREADSAFE_CONTAINERS_OBJECT_POOL_H__