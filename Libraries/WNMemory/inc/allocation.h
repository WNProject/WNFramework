// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_ALLOCATION_H__
#define __WN_MEMORY_ALLOCATION_H__

#include "WNMemory/inc/internal/allocation.h"
#include "core/inc/assert.h"
#include "core/inc/type_traits.h"
#include "core/inc/types.h"
#include "core/inc/utilities.h"

#include <cstdlib>
#include <new>

#define WN_STACK_ALLOC(_size) __WN_INTERNAL_STACK_ALLOC(_size)

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

  return internal::aligned_malloc(_size, _alignment);
}

inline void* aligned_realloc(void* _ptr, size_t _new_size, size_t _alignment) {
  WN_RELEASE_ASSERT(_new_size != 0, "attempting to allocate 0 bytes");
  WN_RELEASE_ASSERT(_alignment > 0 && !(_alignment & (_alignment - 1)),
      "alignment is not a positive power of 2");

  return internal::aligned_realloc(_ptr, _new_size, _alignment);
}

inline void aligned_free(void* _ptr) {
  internal::aligned_free(_ptr);
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
      aligned_malloc(sizeof(T), core::alignment_of<T>::value));
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
  return construct_at(static_cast<T*>(_ptr), core::forward<Args>(_args)...);
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

}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_ALLOCATION_H__
