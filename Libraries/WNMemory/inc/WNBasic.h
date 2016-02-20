// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_BASIC_H__
#define __WN_MEMORY_BASIC_H__

#include "WNCore/inc/WNAssert.h"
#include "WNMemory/inc/Internal/WNBasicTraits.h"

#include <cstdlib>
#include <memory>
#include <new>

#ifdef _WN_WINDOWS
#include <malloc.h>

#define WN_STACK_ALLOC(size) ::_alloca(size)
#elif defined _WN_LINUX || defined _WN_ANDROID
#include <alloca.h>

#define WN_STACK_ALLOC(size) ::alloca(size)
#endif

// If you want allocators to track the amount of allocated memory
// define WN_TRACK_ALLOCATIONS

namespace wn {
namespace memory {

WN_FORCE_INLINE void* malloc(const size_t size) {
  WN_RELEASE_ASSERT_DESC(size != 0, "attempting to allocate 0 bytes");

  return (std::malloc(size));
}

WN_FORCE_INLINE void* realloc(void* ptr, const size_t new_size) {
  return (std::realloc(ptr, new_size));
}

WN_FORCE_INLINE void free(void* ptr) {
  std::free(ptr);
}

WN_FORCE_INLINE void* aligned_malloc(
    const size_t size, const size_t alignment) {
  WN_RELEASE_ASSERT_DESC(size != 0, "attempting to allocate 0 bytes");
  WN_RELEASE_ASSERT_DESC(alignment > 0 && !(alignment & (alignment - 1)),
      "alignment is not a positive power of 2");

#ifdef _WN_WINDOWS
  return (::_aligned_malloc(size, alignment));
#else
  void* real_ptr = malloc((size + alignment - 1) + 2 * sizeof(void*));

  if (real_ptr) {
    void* ptr = reinterpret_cast<void*>(
        ((reinterpret_cast<size_t>(real_ptr) + sizeof(void*) + alignment - 1) &
            ~(alignment - 1)));

    *(reinterpret_cast<void**>(ptr) - 1) = real_ptr;
    *(reinterpret_cast<size_t*>(ptr) - 2) = size;

    return (ptr);
  }

  return (nullptr);
#endif
}

#ifndef _WN_WINDOWS
void aligned_free(void*);
void* memcpy(void*, const void*, const size_t);
#endif

WN_FORCE_INLINE void* aligned_realloc(
    void* ptr, const size_t new_size, const size_t alignment) {
  WN_RELEASE_ASSERT_DESC(new_size != 0, "attempting to allocate 0 bytes");
  WN_RELEASE_ASSERT_DESC(alignment > 0 && !(alignment & (alignment - 1)),
      "alignment is not a positive power of 2");

#ifdef _WN_WINDOWS
  return (::_aligned_realloc(ptr, new_size, alignment));
#else
  void* temp_ptr = aligned_malloc(new_size, alignment);

  if (ptr) {
    const size_t old_size = *(reinterpret_cast<size_t*>(ptr) - 2);
    const size_t copy_size = old_size < new_size ? old_size : new_size;

    temp_ptr = memcpy(temp_ptr, ptr, copy_size);
  }

  aligned_free(ptr);

  return (temp_ptr);
#endif
}

WN_FORCE_INLINE void aligned_free(void* ptr) {
#ifdef _WN_WINDOWS
  ::_aligned_free(ptr);
#else
  if (ptr) {
    free(*(reinterpret_cast<void**>(ptr) - 1));
  }
#endif
}

template <typename T>
WN_FORCE_INLINE T* heap_allocate() {
  static_assert(!std::is_void<T>::value,
      "you must specify a size in bytes when allocating memory of type void");

  return (static_cast<T*>(malloc(sizeof(T))));
}

template <typename T>
WN_FORCE_INLINE T* heap_allocate(const size_t size) {
  return (static_cast<T*>(malloc(size * sizeof(T))));
}

template <>
WN_FORCE_INLINE void* heap_allocate(const size_t size) {
  return (malloc(size));
}

template <typename T>
WN_FORCE_INLINE T* heap_reallocate(T* ptr, const size_t new_size) {
  return (
      static_cast<T*>(realloc(static_cast<void*>(ptr), new_size * sizeof(T))));
}

template <>
WN_FORCE_INLINE void* heap_reallocate(void* ptr, const size_t new_size) {
  WN_RELEASE_ASSERT_DESC(new_size != 0, "attempting to allocate 0 bytes");

  return (realloc(ptr, new_size));
}

template <typename T>
WN_FORCE_INLINE void heap_free(T* ptr) {
  free(static_cast<void*>(ptr));
}

template <>
WN_FORCE_INLINE void heap_free(void* ptr) {
  free(ptr);
}

template <typename T>
WN_FORCE_INLINE T* heap_aligned_allocate() {
  return (
      static_cast<T*>(aligned_malloc(sizeof(T), std::alignment_of<T>::value)));
}

template <typename T>
WN_FORCE_INLINE T* heap_aligned_allocate(const size_t alignment) {
  return (static_cast<T*>(aligned_malloc(sizeof(T), alignment)));
}

template <typename T>
WN_FORCE_INLINE T* heap_aligned_allocate(
    const size_t size, const size_t alignment) {
  return (static_cast<T*>(aligned_malloc(size * sizeof(T), alignment)));
}

template <>
WN_FORCE_INLINE void* heap_aligned_allocate(
    const size_t size, const size_t alignment) {
  return (aligned_malloc(size, alignment));
}

template <typename T>
WN_FORCE_INLINE T* heap_aligned_reallocate(
    T* ptr, const size_t new_size, const size_t alignment) {
  return (static_cast<T*>(aligned_realloc(
      static_cast<void*>(ptr), new_size * sizeof(T), alignment)));
}

template <>
WN_FORCE_INLINE void* heap_aligned_reallocate(
    void* ptr, const size_t new_size, const size_t alignment) {
  return (aligned_realloc(ptr, new_size, alignment));
}

template <typename T>
WN_FORCE_INLINE void heap_aligned_free(T* ptr) {
  aligned_free(static_cast<void*>(ptr));
}

template <>
WN_FORCE_INLINE void heap_aligned_free(void* ptr) {
  aligned_free(ptr);
}

template <typename T, typename... Args>
T* construct_at(T*, Args&&...);

template <typename T, typename... Args>
WN_FORCE_INLINE T* construct(Args&&... args) {
  static_assert(
      !std::is_void<T>::value, "you cannot construct an object of type void");
  return (construct_at<T, Args...>(
      heap_allocate<T>(), std::forward<Args>(args)...));
}

template <typename T>
WN_FORCE_INLINE T* construct_array(const size_t size) {
  static_assert(
      !std::is_void<T>::value, "you cannot construct an object of type void");

  return (new (std::nothrow) T[size]);
}

template <typename T, typename... Args>
WN_FORCE_INLINE T* construct_aligned(Args&&... args) {
  static_assert(
      !std::is_void<T>::value, "you cannot construct an object of type void");

  return (construct_at<T, Args...>(
      heap_aligned_allocate<T>(), std::forward<Args>(args)...));
}

template <typename T, typename... Args>
WN_FORCE_INLINE T* construct_at(T* ptr, Args&&... args) {
  static_assert(
      !std::is_void<T>::value, "you cannot construct an object of type void");

  return (new (ptr) T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
WN_FORCE_INLINE T* construct_at(void* ptr, Args&&... args) {
  return (construct_at(static_cast<T*>(ptr), std::forward<Args>(args)...));
}

template <typename T>
WN_FORCE_INLINE void destroy(T* ptr) {
  delete (ptr);
}

template <typename T>
WN_FORCE_INLINE void destroy_array(T* ptr) {
  delete[](ptr);
}

template <typename T>
WN_FORCE_INLINE void destroy_aligned(T* ptr) {
  ptr->~T();

  heap_aligned_free(ptr);
}

template <typename T>
struct default_destroyer {
  default_destroyer() = default;

  template <typename U>
  WN_FORCE_INLINE default_destroyer(const default_destroyer<U>&){};

  WN_FORCE_INLINE void operator()(T* ptr) const {
    destroy(ptr);
  }
};

template <typename T>
struct default_destroyer<T[]> {
  default_destroyer() = default;

  template <typename U>
  WN_FORCE_INLINE default_destroyer(const default_destroyer<U[]>&) {}

  template <typename U, typename = core::enable_if_t<
                            std::is_convertible<U (*)[], T (*)[]>::value>>
  WN_FORCE_INLINE void operator()(U* ptr) const {
    destroy_array(ptr);
  }
};

template <typename T>
struct default_aligned_destroyer {
  default_aligned_destroyer() = default;

  template <typename U>
  WN_FORCE_INLINE default_aligned_destroyer(
      const default_aligned_destroyer<U>&){};

  WN_FORCE_INLINE void operator()(T* ptr) const {
    destroy_aligned(ptr);
  }
};

WN_FORCE_INLINE void* memzero(void* dest, const size_t count) {
  return (internal::basic_traits::memzero(dest, count));
}

WN_FORCE_INLINE void* memset(
    void* dest, const uint8_t value, const size_t count) {
  return (internal::basic_traits::memset(dest, value, count));
}

WN_FORCE_INLINE void* memcpy(void* dest, const void* src, const size_t count) {
  return (internal::basic_traits::memcpy(dest, src, count));
}

WN_FORCE_INLINE void* memmove(void* dest, const void* src, const size_t count) {
  return (internal::basic_traits::memmove(dest, src, count));
}

WN_FORCE_INLINE int32_t memcmp(
    const void* lhs, const void* rhs, const size_t count) {
  return (internal::basic_traits::memcmp(lhs, rhs, count));
}

template <typename T>
WN_FORCE_INLINE T* memory_zero(T* dest) {
  static_assert(!std::is_void<T>::value,
      "you must specify a size in bytes when zeroing memory of type void");

  return (static_cast<T*>(memzero(dest, sizeof(T))));
}

template <typename T>
WN_FORCE_INLINE T* memory_zero(T* dest, const size_t count) {
  return (static_cast<T*>(memzero(dest, count * sizeof(T))));
}

template <>
WN_FORCE_INLINE void* memory_zero(void* dest, const size_t count) {
  return (memzero(dest, count));
}

template <typename T>
WN_FORCE_INLINE T* memory_set(T* dest, const uint8_t value) {
  static_assert(!std::is_void<T>::value,
      "you must specify a size in bytes when setting memory of type void");

  return (static_cast<T*>(memset(dest, value, sizeof(T))));
}

template <typename T>
WN_FORCE_INLINE T* memory_set(
    T* dest, const uint8_t value, const size_t count) {
  return (static_cast<T*>(memset(dest, value, count * sizeof(T))));
}

template <>
WN_FORCE_INLINE void* memory_set(
    void* dest, const uint8_t value, const size_t count) {
  return (memset(dest, value, count));
}

template <typename T>
WN_FORCE_INLINE T* memory_copy(T* dest, const T* src) {
  static_assert(!std::is_void<T>::value,
      "you must specify a size in bytes when copying memory of type void");

  return (static_cast<T*>(memcpy(dest, src, sizeof(T))));
}

template <typename T>
WN_FORCE_INLINE T* memory_copy(T* dest, const T* src, const size_t count) {
  return (static_cast<T*>(memcpy(dest, src, count * sizeof(T))));
}

template <>
WN_FORCE_INLINE void* memory_copy(
    void* dest, const void* src, const size_t count) {
  return (memcpy(dest, src, count));
}

template <typename T>
WN_FORCE_INLINE T* memory_move(T* dest, const T* src) {
  static_assert(!std::is_void<T>::value,
      "you must specify a size in bytes when moving memory of type void");

  return (static_cast<T*>(memmove(dest, src, sizeof(T))));
}

template <typename T>
WN_FORCE_INLINE T* memory_move(T* dest, const T* src, const size_t count) {
  return (static_cast<T*>(wn::memory::memmove(dest, src, count * sizeof(T))));
}

template <>
WN_FORCE_INLINE void* memory_move(
    void* dest, const void* src, const size_t count) {
  return (wn::memory::memmove(dest, src, count));
}

template <typename T>
WN_FORCE_INLINE int32_t memory_compare(const T* lhs, const T* rhs) {
  static_assert(!std::is_void<T>::value,
      "you must specify a size in bytes when comparing memory of type void");

  return (wn::memory::memcmp(lhs, rhs, sizeof(T)));
}

template <typename T>
WN_FORCE_INLINE int32_t memory_compare(
    const T* lhs, const T* rhs, const size_t count) {
  return (wn::memory::memcmp(lhs, rhs, count * sizeof(T)));
}

template <>
WN_FORCE_INLINE int32_t memory_compare(
    const void* lhs, const void* rhs, const size_t count) {
  return (memcmp(lhs, rhs, count));
}

template <typename T>
WN_FORCE_INLINE void prefetch(const T* ptr) {
  internal::basic_traits::prefetch(ptr);
}

template <typename T>
WN_FORCE_INLINE void prefetch(const T* ptr, const size_t offset) {
  prefetch(ptr + offset);
}

template <>
WN_FORCE_INLINE void prefetch(const void* ptr, const size_t offset) {
  prefetch(static_cast<const uint8_t*>(ptr), offset);
}

}  // namespace memory
}  // namespace wn

#endif  // __WN_MEMORY_BASIC_H__
