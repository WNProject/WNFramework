#ifndef __WN_MEMORY_STL_COMPATIBLE_ALLOCATOR_H__
#define __WN_MEMORY_STL_COMPATIBLE_ALLOCATOR_H__

#include "WNAllocator.h"

namespace wn {
namespace memory {

template<typename DefaultAlloc>
struct passthrough_allocator_base {
  static DefaultAlloc s_default_allocator;
};

template <typename T, typename DefaultAlloc>
struct passthrough_stl_allocator
    : public passthrough_allocator_base<DefaultAlloc> {
  typedef T value_type;
  typedef T* pointer;
  typedef T& reference;
  typedef const T* const_pointer;
  typedef const T& const_reference;
  typedef wn_size_t size_type;
  typedef wn_size_t difference_type;
  typedef std::false_type propagate_on_container_copy_assignment;
  typedef std::true_type propagate_on_container_move_assignment;
  typedef std::false_type propagate_on_container_swap;

  passthrough_stl_allocator select_on_container_copy_construction() {
    return *this;
  }

  passthrough_stl_allocator()
      : passthrough_stl_allocator(
            &passthrough_allocator_base<DefaultAlloc>::s_default_allocator) {}
  passthrough_stl_allocator(wn::memory::allocator* _allocator)
      : m_allocator(_allocator) {}
  template <typename T0, typename T1>
  passthrough_stl_allocator(const passthrough_stl_allocator<T0, T1>& _other)
      : m_allocator(_other.m_allocator) {}

  template <class Type>
  struct rebind {
    typedef passthrough_stl_allocator<Type, DefaultAlloc> other;
  };

  template<typename T0, typename T1>
  inline bool operator==(const passthrough_stl_allocator<T0, T1>& _other) const {
    return m_allocator == _other.m_allocator;
  }

  pointer address(reference _r) { return &_r; }
  const_pointer address(const_reference _r) { return &_r; }

  template <typename DA = DefaultAlloc>
  pointer allocate(
      size_type _n,
      typename passthrough_stl_allocator<void, DA>::const_pointer = 0) {
    return static_cast<pointer>(
        m_allocator->allocate(sizeof(value_type), _n).m_location);
  }

  void deallocate(pointer _p, size_type) { return m_allocator->deallocate(_p); }
  size_type max_size() const {
    return std::numeric_limits<wn_size_t>::max() / sizeof(value_type);
  }
  template <class U, class... Args>
  void construct(U* _p, Args&&... _args) {
    ::new ((void*)_p) U(std::forward<Args>(_args)...);
  }
  template <class U>
  void destroy(U* _p) {
    WN_UNUSED_ARGUMENT(_p);
    _p->~U();
  }

  wn::memory::allocator* m_allocator;
};

template <typename DefaultAlloc>
struct passthrough_stl_allocator<void, DefaultAlloc>
    : public passthrough_allocator_base<DefaultAlloc> {
  typedef void value_type;
  typedef void* pointer;
  typedef const void* const_pointer;
  typedef wn_size_t size_type;
  typedef wn_size_t difference_type;
  typedef std::false_type propagate_on_container_copy_assignment;
  typedef std::true_type propagate_on_container_move_assignment;
  typedef std::false_type propagate_on_container_swap;

  passthrough_stl_allocator select_on_container_copy_construction() {
    return *this;
  }

  passthrough_stl_allocator()
      : passthrough_stl_allocator(
            &passthrough_allocator_base<DefaultAlloc>::s_default_allocator) {}
  passthrough_stl_allocator(wn::memory::allocator* _allocator)
      : m_allocator(_allocator) {}
  template <typename T0, typename T1>
  passthrough_stl_allocator(const passthrough_stl_allocator<T0, T1>& _other)
      : m_allocator(_other.m_allocator) {}

  template <class Type>
  struct rebind {
    typedef passthrough_stl_allocator<Type, DefaultAlloc> other;
  };

  template<typename T0, typename T1>
  inline bool operator==(const passthrough_stl_allocator<T0, T1>& _other) const {
    return m_allocator == _other.m_allocator;
  }

  template <typename DA = DefaultAlloc>
  pointer allocate(
      size_type _n,
      typename passthrough_stl_allocator<void, DA>::const_pointer = 0) {
    return wn_nullptr;
  }

  void deallocate(pointer _p, size_type) { return m_allocator->deallocate(_p); }
  size_type max_size() const {
    return std::numeric_limits<wn_size_t>::max();
  }
  template <class U, class... Args>
  void construct(U* _p, Args&&... _args) {
    ::new ((void*)_p) U(std::forward<Args>(_args)...);
  }
  template <class U>
  void destroy(U* _p) {
    WN_UNUSED_ARGUMENT(_p);
    _p->~U();
  }

  wn::memory::allocator* m_allocator;
};

template <typename DefaultAlloc>
DefaultAlloc passthrough_allocator_base<DefaultAlloc>::s_default_allocator;
}
}

#endif  //__WN_MEMORY_STL_COMPATIBLE_ALLOCATOR_H__