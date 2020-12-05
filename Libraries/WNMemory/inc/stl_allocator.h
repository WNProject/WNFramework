// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_MEMORY_STL_ALLOCATOR_H__
#define __WN_MEMORY_STL_ALLOCATOR_H__

#include "WNMemory/inc/allocator.h"
#include "core/inc/type_traits.h"
#include "core/inc/utilities.h"

#include <limits>

namespace wn {
namespace memory {
namespace internal {

template <typename DefaultAllocator>
class stl_allocator_base {
protected:
  static DefaultAllocator* get_default_allocator() {
    static DefaultAllocator allocator;

    return &allocator;
  }
};

}  // namespace internal

template <typename T, typename DefaultAllocator>
class stl_allocator : public internal::stl_allocator_base<DefaultAllocator> {
public:
  using value_type = T;
  using pointer = T*;
  using reference = T&;
  using const_pointer = const T*;
  using const_reference = const T&;
  using size_type = size_t;
  using difference_type = signed_t;
  using propagate_on_container_copy_assignment = core::false_type;
  using propagate_on_container_move_assignment = core::true_type;
  using propagate_on_container_swap = core::false_type;

  template <typename U>
  struct rebind {
    typedef stl_allocator<U, DefaultAllocator> other;
  };

  stl_allocator() : stl_allocator(base::get_default_allocator()) {}

  stl_allocator(allocator* _allocator) : m_allocator(_allocator) {}

  template <typename U, typename OtherDefaultAllocator>
  stl_allocator(const stl_allocator<U, OtherDefaultAllocator>& _other)
    : m_allocator(_other.m_allocator) {}

  template <typename U, typename OtherDefaultAllocator>
  bool operator==(const stl_allocator<U, OtherDefaultAllocator>& _other) const {
    return (m_allocator == _other.m_allocator);
  }

  pointer address(reference _reference) {
    return &_reference;
  }
  const_pointer address(const_reference _reference) {
    return &_reference;
  }

  template <typename DA = DefaultAllocator>
  pointer allocate(
      size_type _size, typename stl_allocator<void, DA>::const_pointer = 0) {
    return static_cast<pointer>(
        m_allocator->allocate(sizeof(value_type) * _size));
  }

  void deallocate(pointer _ptr, size_type _size) {
    (void)_size;

    return m_allocator->deallocate(_ptr);
  }

  template <typename U, typename... Args>
  void construct(U* _ptr, Args&&... _args) {
    m_allocator->construct_at(_ptr, core::forward<Args>(_args)...);
  }

  template <typename U>
  void destroy(U* _ptr) {
    (void)_ptr;

    _ptr->~U();
  }

  size_type max_size() const {
    return (std::numeric_limits<size_t>::max() / sizeof(value_type));
  }

  stl_allocator select_on_container_copy_construction() {
    return *this;
  }

private:
  template <typename U, typename OtherDefaultAllocator>
  friend class stl_allocator;

  using base = internal::stl_allocator_base<DefaultAllocator>;

  allocator* m_allocator;
};

template <typename DefaultAllocator>
class stl_allocator<void, DefaultAllocator>
  : public internal::stl_allocator_base<DefaultAllocator> {
public:
  using value_type = void;
  using pointer = void*;
  using const_pointer = const void*;
  using size_type = size_t;
  using difference_type = signed_t;
  using propagate_on_container_copy_assignment = core::false_type;
  using propagate_on_container_move_assignment = core::true_type;
  using propagate_on_container_swap = core::false_type;

  template <typename U>
  struct rebind {
    typedef stl_allocator<U, DefaultAllocator> other;
  };

  stl_allocator() : stl_allocator(base::get_default_allocator()) {}

  stl_allocator(allocator* _allocator) : m_allocator(_allocator) {}

  template <typename U, typename OtherDefaultAllocator>
  stl_allocator(const stl_allocator<U, OtherDefaultAllocator>& _other)
    : m_allocator(_other.m_allocator) {}

  template <typename U, typename OtherDefaultAllocator>
  bool operator==(const stl_allocator<U, OtherDefaultAllocator>& _other) const {
    return (m_allocator == _other.m_allocator);
  }

  template <typename DA = DefaultAllocator>
  pointer allocate(
      size_type _size, typename stl_allocator<void, DA>::const_pointer = 0) {
    return m_allocator->allocate(_size);
  }

  void deallocate(pointer _ptr, size_type _size) {
    (void)_size;

    return m_allocator->deallocate(_ptr);
  }

  template <typename U, typename... Args>
  void construct(U* _ptr, Args&&... _args) {
    m_allocator->construct_at(_ptr, core::forward<Args>(_args)...);
  }

  template <typename U>
  void destroy(U* _ptr) {
    (void)_ptr;

    _ptr->~U();
  }

  size_type max_size() const {
    return std::numeric_limits<size_t>::max();
  }

  stl_allocator select_on_container_copy_construction() {
    return *this;
  }

private:
  template <typename U, typename OtherDefaultAllocator>
  friend class stl_allocator;

  using base = internal::stl_allocator_base<DefaultAllocator>;

  allocator* m_allocator;
};

}  // namespace memory
}  // namespace wn

#endif  //__WN_MEMORY_STL_ALLOCATOR_H__
