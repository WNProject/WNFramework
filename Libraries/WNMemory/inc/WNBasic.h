// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_BASIC_H__
#define __WN_MEMORY_BASIC_H__

#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNMemory/inc/Internal/WNBasicTraits.h"

#include <cstdlib>
#include <memory>
#include <new>

#ifdef _WN_WINDOWS
#include <malloc.h>

#define WN_STACK_ALLOC(_size) ::_alloca(_size)
#elif defined _WN_LINUX || defined _WN_ANDROID
#include <alloca.h>

#define WN_STACK_ALLOC(_size) ::alloca(_size)
#endif

namespace wn {
namespace memory {

inline void* malloc(size_t _size) {
  WN_RELEASE_ASSERT(_size != 0, "attempting to allocate 0 bytes");

  return std::malloc(_size);
}

inline void* realloc(void* _ptr, size_t _new_size) {
  return std::realloc(_ptr, _new_size);
}

inline void free(void* _ptr) {
  std::free(_ptr);
}

inline void* aligned_malloc(size_t _size, size_t _alignment) {
  WN_RELEASE_ASSERT(_size != 0, "attempting to allocate 0 bytes");
  WN_RELEASE_ASSERT(_alignment > 0 && !(_alignment & (_alignment - 1)),
      "alignment is not a positive power of 2");

#ifdef _WN_WINDOWS
  return ::_aligned_malloc(_size, _alignment);
#else
  void* real_ptr = malloc((_size + _alignment - 1) + 2 * sizeof(void*));

  if (real_ptr) {
    void* adjusted_ptr = reinterpret_cast<void*>(
        ((reinterpret_cast<size_t>(real_ptr) + sizeof(void*) + _alignment - 1) &
            ~(_alignment - 1)));

    *(reinterpret_cast<void**>(adjusted_ptr) - 1) = real_ptr;
    *(reinterpret_cast<size_t*>(adjusted_ptr) - 2) = _size;

    return adjusted_ptr;
  }

  return nullptr;
#endif
}

#ifndef _WN_WINDOWS
void aligned_free(void*);
void* memcpy(void*, const void*, size_t);
#endif

inline void* aligned_realloc(void* _ptr, size_t _new_size, size_t _alignment) {
  WN_RELEASE_ASSERT(_new_size != 0, "attempting to allocate 0 bytes");
  WN_RELEASE_ASSERT(_alignment > 0 && !(_alignment & (_alignment - 1)),
      "alignment is not a positive power of 2");

#ifdef _WN_WINDOWS
  return ::_aligned_realloc(_ptr, _new_size, _alignment);
#else
  void* temp_ptr = aligned_malloc(_new_size, _alignment);

  if (_ptr) {
    const size_t old_size = *(reinterpret_cast<size_t*>(_ptr) - 2);
    const size_t copy_size = old_size < _new_size ? old_size : _new_size;

    temp_ptr = memcpy(temp_ptr, _ptr, copy_size);
  }

  aligned_free(_ptr);

  return temp_ptr;
#endif
}

inline void aligned_free(void* _ptr) {
#ifdef _WN_WINDOWS
  ::_aligned_free(_ptr);
#else
  if (_ptr) {
    free(*(reinterpret_cast<void**>(_ptr) - 1));
  }
#endif
}

template <typename T>
inline T* heap_allocate() {
  static_assert(!core::is_void<T>::value,
      "you must specify a size in bytes when allocating memory of type void");

  return static_cast<T*>(malloc(sizeof(T)));
}

template <typename T>
inline T* heap_allocate(size_t _size) {
  return static_cast<T*>(malloc(_size * sizeof(T)));
}

template <>
inline void* heap_allocate(size_t _size) {
  return malloc(_size);
}

template <typename T>
inline T* heap_reallocate(T* _ptr, size_t _new_size) {
  return static_cast<T*>(
      realloc(static_cast<void*>(_ptr), _new_size * sizeof(T)));
}

template <>
inline void* heap_reallocate(void* _ptr, size_t _new_size) {
  WN_RELEASE_ASSERT(_new_size != 0, "attempting to allocate 0 bytes");

  return realloc(_ptr, _new_size);
}

template <typename T>
inline void heap_free(T* _ptr) {
  free(static_cast<void*>(_ptr));
}

template <>
inline void heap_free(void* _ptr) {
  free(_ptr);
}

template <typename T>
inline T* heap_aligned_allocate() {
  return static_cast<T*>(
      aligned_malloc(sizeof(T), std::alignment_of<T>::value));
}

template <typename T>
inline T* heap_aligned_allocate(size_t _alignment) {
  return static_cast<T*>(aligned_malloc(sizeof(T), _alignment));
}

template <typename T>
inline T* heap_aligned_allocate(size_t _size, size_t _alignment) {
  return static_cast<T*>(aligned_malloc(_size * sizeof(T), _alignment));
}

template <>
inline void* heap_aligned_allocate(size_t _size, size_t _alignment) {
  return aligned_malloc(_size, _alignment);
}

template <typename T>
inline T* heap_aligned_reallocate(
    T* _ptr, size_t _new_size, size_t _alignment) {
  return static_cast<T*>(aligned_realloc(
      static_cast<void*>(_ptr), _new_size * sizeof(T), _alignment));
}

template <>
inline void* heap_aligned_reallocate(
    void* _ptr, size_t _new_size, size_t _alignment) {
  return aligned_realloc(_ptr, _new_size, _alignment);
}

template <typename T>
inline void heap_aligned_free(T* _ptr) {
  aligned_free(static_cast<void*>(_ptr));
}

template <>
inline void heap_aligned_free(void* _ptr) {
  aligned_free(_ptr);
}

template <typename T, typename... Args>
T* construct_at(T*, Args&&...);

template <typename T, typename... Args>
inline T* construct(Args&&... _args) {
  static_assert(
      !core::is_void<T>::value, "you cannot construct an object of type void");
  return construct_at<T, Args...>(
      heap_allocate<T>(), core::forward<Args>(_args)...);
}

template <typename T>
inline T* construct_array(size_t _count) {
  static_assert(
      !core::is_void<T>::value, "you cannot construct an object of type void");

  return new (std::nothrow) T[_count];
}

template <typename T, typename... Args>
inline T* construct_aligned(Args&&... _args) {
  static_assert(
      !core::is_void<T>::value, "you cannot construct an object of type void");

  return construct_at<T, Args...>(
      heap_aligned_allocate<T>(), core::forward<Args>(_args)...);
}

template <typename T, typename... Args>
inline T* construct_at(T* _ptr, Args&&... _args) {
  static_assert(
      !core::is_void<T>::value, "you cannot construct an object of type void");

  return new (_ptr) T(core::forward<Args>(_args)...);
}

template <typename T, typename... Args>
inline T* construct_at(void* _ptr, Args&&... _args) {
  return (construct_at(static_cast<T*>(_ptr), core::forward<Args>(_args)...));
}

template <typename T>
inline void destroy(T* _ptr) {
  delete _ptr;
}

template <typename T>
inline void destroy_array(T* _ptr) {
  delete[] _ptr;
}

template <typename T>
inline void destroy_aligned(T* _ptr) {
  _ptr->~T();

  heap_aligned_free(_ptr);
}

inline void* memzero(void* _dest, size_t _count) {
  return internal::basic_traits::memzero(_dest, _count);
}

inline void* memset(void* _dest, uint8_t _value, size_t _count) {
  return internal::basic_traits::memset(_dest, _value, _count);
}

inline void* memcpy(void* _dest, const void* _src, size_t _count) {
  return internal::basic_traits::memcpy(_dest, _src, _count);
}

inline void* memmove(void* _dest, const void* _src, size_t _count) {
  return internal::basic_traits::memmove(_dest, _src, _count);
}

inline int32_t memcmp(const void* _lhs, const void* _rhs, size_t _count) {
  return internal::basic_traits::memcmp(_lhs, _rhs, _count);
}

template <typename T>
inline T* memory_zero(T* _dest) {
  static_assert(!core::is_void<T>::value,
      "you must specify a size in bytes when zeroing memory of type void");

  return static_cast<T*>(memzero(_dest, sizeof(T)));
}

template <typename T>
inline T* memory_zero(T* _dest, size_t _count) {
  return static_cast<T*>(memzero(_dest, _count * sizeof(T)));
}

template <>
inline void* memory_zero(void* _dest, size_t _count) {
  return memzero(_dest, _count);
}

template <typename T>
inline T* memory_set(T* _dest, uint8_t _value) {
  static_assert(!core::is_void<T>::value,
      "you must specify a size in bytes when setting memory of type void");

  return static_cast<T*>(memory::memset(_dest, _value, sizeof(T)));
}

template <typename T>
inline T* memory_set(T* _dest, uint8_t _value, size_t _count) {
  return static_cast<T*>(memory::memset(_dest, _value, _count * sizeof(T)));
}

template <>
inline void* memory_set(void* _dest, uint8_t _value, size_t _count) {
  return memory::memset(_dest, _value, _count);
}

template <typename T>
inline T* memory_copy(T* _dest, const T* _src) {
  static_assert(!core::is_void<T>::value,
      "you must specify a size in bytes when copying memory of type void");

  return static_cast<T*>(memory::memcpy(_dest, _src, sizeof(T)));
}

template <typename T>
inline T* memory_copy(T* _dest, const T* _src, size_t _count) {
  return static_cast<T*>(memory::memcpy(_dest, _src, _count * sizeof(T)));
}

template <>
inline void* memory_copy(void* _dest, const void* _src, size_t _count) {
  return memory::memcpy(_dest, _src, _count);
}

template <typename T>
inline T* memory_move(T* _dest, const T* _src) {
  static_assert(!core::is_void<T>::value,
      "you must specify a _size in bytes when moving memory of type void");

  return static_cast<T*>(memory::memmove(_dest, _src, sizeof(T)));
}

template <typename T>
inline T* memory_move(T* _dest, const T* _src, size_t _count) {
  return static_cast<T*>(memory::memmove(_dest, _src, _count * sizeof(T)));
}

template <>
inline void* memory_move(void* _dest, const void* _src, size_t _count) {
  return memory::memmove(_dest, _src, _count);
}

template <typename T>
inline int32_t memory_compare(const T* _lhs, const T* _rhs) {
  static_assert(!core::is_void<T>::value,
      "you must specify a _size in bytes when comparing memory of type void");

  return memory::memcmp(_lhs, _rhs, sizeof(T));
}

template <typename T>
inline int32_t memory_compare(const T* _lhs, const T* _rhs, size_t _count) {
  return memory::memcmp(_lhs, _rhs, _count * sizeof(T));
}

template <>
inline int32_t memory_compare(
    const void* _lhs, const void* _rhs, size_t _count) {
  return memory::memcmp(_lhs, _rhs, _count);
}

template <typename T>
inline void prefetch(const T* _ptr) {
  internal::basic_traits::prefetch(_ptr);
}

template <typename T>
inline void prefetch(const T* _ptr, size_t _offset) {
  prefetch(_ptr + _offset);
}

template <>
inline void prefetch(const void* _ptr, size_t _offset) {
  prefetch(static_cast<const uint8_t*>(_ptr), _offset);
}

}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_BASIC_H__
