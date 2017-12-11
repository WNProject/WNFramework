// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_THREADSAFE_CONTAINERS_OBJECT_POOL_H__
#define __WN_THREADSAFE_CONTAINERS_OBJECT_POOL_H__

#include "WNContainers/inc/WNList.h"
#include "WNFunctional/inc/WNFunction.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNMultiTasking/inc/lock_guard.h"
#include "WNMultiTasking/inc/mutex.h"

namespace wn {
namespace threadsafe_containers {

template <typename T>
class object_pool;

template <typename T>
class unmanaged_object final {
public:
  unmanaged_object(unmanaged_object&& _other) {
    it = _other.it;
  }

  unmanaged_object(const unmanaged_object& _other) : it(_other.it) {}

  T& operator*() {
    return *it;
  }
  T* operator->() {
    return &(*it);
  }

  const T& operator*() const {
    return *it;
  }

  const T* operator->() const {
    return &(*it);
  }

private:
  template <typename F>
  friend class object_pool;

  explicit unmanaged_object(typename containers::list<T>::iterator _it)
    : it(_it) {}
  typename containers::list<T>::iterator it;
};

template <typename T>
class pooled_object final {
public:
  pooled_object(pooled_object&& _other) : m_val(core::move(_other.m_val)) {
    _other.m_valid = false;
    m_pool = _other.m_pool;
  }

  ~pooled_object();

  T& operator*() {
    return *m_val;
  }
  T* operator->() {
    return &(*m_val);
  }

  const T& operator*() const {
    return *m_val;
  }

  const T* operator->() const {
    return &(*m_val);
  }

private:
  template <typename F>
  friend class object_pool;

  explicit pooled_object(unmanaged_object<T>&& _val, object_pool<T>* _pool)
    : m_valid(true), m_val(core::move(_val)), m_pool(_pool) {}

  bool m_valid;
  unmanaged_object<T> m_val;
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
  using unmanaged_object_t = unmanaged_object<T>;

  object_pool(memory::allocator* _allocator,
      functional::function<T()> _create_function,
      functional::function<void(T*)> _reset_function)
    : m_allocator(_allocator),
      m_free_list(_allocator),
      m_used_list(_allocator),
      m_create_function(_create_function),
      m_reset_function(_reset_function) {}

  object_pool(memory::allocator* _allocator, const T& object_template)
    : object_pool(_allocator, copyable_object(object_template),
          resettable_object(object_template)) {}

  unmanaged_object_t get_unmanaged();
  void release_unmanaged(unmanaged_object_t& object);

  pooled_object<T> get_object();

private:
  memory::allocator* m_allocator;
  containers::list<T> m_free_list;
  containers::list<T> m_used_list;
  multi_tasking::mutex m_lock;
  functional::function<T()> m_create_function;
  functional::function<void(T*)> m_reset_function;
};

template <typename T>
typename object_pool<T>::unmanaged_object_t object_pool<T>::get_unmanaged() {
  multi_tasking::lock_guard<multi_tasking::mutex> guard(m_lock);
  auto ret_iter = m_free_list.begin();
  if (!m_free_list.empty()) {
    m_free_list.transfer_to(
        m_free_list.begin(), m_used_list.end(), m_used_list);
    ret_iter = m_used_list.end() - 1;
    m_reset_function(&(*ret_iter));
  } else {
    m_used_list.push_back(m_create_function());
    ret_iter = m_used_list.end() - 1;
  }
  return unmanaged_object_t(ret_iter);
}

// We will take advantage of the fact that we know
// that unmanaged_object is a T*. Furthermore it is
// a T* that lives within a linked-list node.
template <typename T>
void object_pool<T>::release_unmanaged(
    typename object_pool<T>::unmanaged_object_t& obj) {
  multi_tasking::lock_guard<multi_tasking::mutex> guard(m_lock);
  m_used_list.transfer_to(obj.it, m_free_list.end(), m_free_list);
}

template <typename T>
pooled_object<T> object_pool<T>::get_object() {
  return pooled_object<T>(get_unmanaged(), this);
}

template <typename T>
pooled_object<T>::~pooled_object() {
  if (m_valid) {
    m_pool->release_unmanaged(m_val);
  }
}

}  // namespace threadsafe_containers
}  // namespace wn

#endif  // __WN_THREADSAFE_CONTAINERS_OBJECT_POOL_H__
