// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MEMORY_BASIC_H__
#define __WN_MEMORY_BASIC_H__

#include "WNCore/inc/WNAssert.h"
#include "WNMemory/inc/Internal/WNBasicTraits.h"

#include <new>
#include <memory>
#include <cstdlib>

#ifdef _WN_WINDOWS
  #include <malloc.h>

  #define WN_STACK_ALLOC(size) ::_alloca(size)
#elif defined _WN_LINUX || defined _WN_ANDROID
  #include <alloca.h>

  #define WN_STACK_ALLOC(size) ::alloca(size)
#endif

namespace wn {
namespace memory {

WN_FORCE_INLINE wn_void* malloc(const wn_size_t size) {
  WN_RELEASE_ASSERT_DESC(size != 0, "attempting to allocate 0 bytes");

  return(std::malloc(size));
}

WN_FORCE_INLINE wn_void* realloc(wn_void* ptr, const wn_size_t new_size) {
  return(std::realloc(ptr, new_size));
}

WN_FORCE_INLINE wn_void free(wn_void* ptr) {
  std::free(ptr);
}

WN_FORCE_INLINE wn_void* aligned_malloc(const wn_size_t size,
                                        const wn_size_t alignment) {
  WN_RELEASE_ASSERT_DESC(size != 0, "attempting to allocate 0 bytes");
  WN_RELEASE_ASSERT_DESC(alignment > 0 && !(alignment & (alignment - 1)),
    "alignment is not a positive power of 2");

  #ifdef _WN_WINDOWS
    return(::_aligned_malloc(size, alignment));
  #else
    wn_void* real_ptr = malloc((size + alignment - 1) + 2 * sizeof(wn_void*));

    if (real_ptr) {
      wn_void *ptr =
          reinterpret_cast<wn_void*>(((reinterpret_cast<wn_size_t>(real_ptr) +
                                        sizeof(wn_void*) + alignment - 1) &
                                       ~(alignment - 1)));

      *(reinterpret_cast<wn_void**>(ptr) - 1) = real_ptr;
      *(reinterpret_cast<size_t*>(ptr) - 2) = size;

      return(ptr);
    }

    return(wn_nullptr);
  #endif
}

#ifndef _WN_WINDOWS
  wn_void aligned_free(wn_void*);
  wn_void* memcpy(wn_void*, const wn_void*, const wn_size_t);
#endif

WN_FORCE_INLINE wn_void* aligned_realloc(wn_void* ptr,
                                         const wn_size_t new_size,
                                         const wn_size_t alignment) {
  WN_RELEASE_ASSERT_DESC(new_size != 0, "attempting to allocate 0 bytes");
  WN_RELEASE_ASSERT_DESC(alignment > 0 && !(alignment & (alignment - 1)),
    "alignment is not a positive power of 2");

  #ifdef _WN_WINDOWS
    return(::_aligned_realloc(ptr, new_size, alignment));
  #else
    wn_void* temp_ptr = aligned_malloc(new_size, alignment);

    if (ptr) {
      const size_t old_size = *(reinterpret_cast<size_t*>(ptr) - 2);
      const size_t copy_size = old_size < new_size ? old_size : new_size;

      temp_ptr = memcpy(temp_ptr, ptr, copy_size);
    }

    aligned_free(ptr);

    return(temp_ptr);
  #endif
}

WN_FORCE_INLINE wn_void aligned_free(wn_void* ptr) {
  #ifdef _WN_WINDOWS
    ::_aligned_free(ptr);
  #else
    if (ptr) {
      free(*(reinterpret_cast<wn_void**>(ptr) - 1));
    }
  #endif
}

template <typename T>
WN_FORCE_INLINE T* heap_allocate() {
  static_assert(!std::is_void<T>::value,
    "you must specify a size in bytes when allocating memory of type void");

  return(static_cast<T*>(malloc(sizeof(T))));
}

template <typename T>
WN_FORCE_INLINE T* heap_allocate(const wn_size_t size) {
  return(static_cast<T*>(malloc(size * sizeof(T))));
}

template <>
WN_FORCE_INLINE wn_void* heap_allocate(const wn_size_t size) {
  return(malloc(size));
}

template <typename T>
WN_FORCE_INLINE T* heap_reallocate(T* ptr, const wn_size_t new_size) {
  return(static_cast<T*>(realloc(static_cast<wn_void*>(ptr),
                                 new_size * sizeof(T))));
}

template <>
WN_FORCE_INLINE wn_void* heap_reallocate(wn_void* ptr,
                                         const wn_size_t new_size) {
  WN_RELEASE_ASSERT_DESC(new_size != 0, "attempting to allocate 0 bytes");

  return(realloc(ptr, new_size));
}

template <typename T>
WN_FORCE_INLINE wn_void heap_free(T* ptr) {
  free(static_cast<wn_void*>(ptr));
}

template <>
WN_FORCE_INLINE wn_void heap_free(wn_void* ptr) {
  free(ptr);
}

template <typename T>
WN_FORCE_INLINE T* heap_aligned_allocate() {
  return(static_cast<T*>(aligned_malloc(sizeof(T),
                         std::alignment_of<T>::value)));
}

template <typename T>
WN_FORCE_INLINE T* heap_aligned_allocate(const wn_size_t alignment) {
  return(static_cast<T*>(aligned_malloc(sizeof(T), alignment)));
}

template <typename T>
WN_FORCE_INLINE T* heap_aligned_allocate(const wn_size_t size,
                                         const wn_size_t alignment) {
  return(static_cast<T*>(aligned_malloc(size * sizeof(T), alignment)));
}

template <>
WN_FORCE_INLINE wn_void* heap_aligned_allocate(const wn_size_t size,
                                               const wn_size_t alignment) {
  return(aligned_malloc(size, alignment));
}

template <typename T>
WN_FORCE_INLINE T* heap_aligned_reallocate(T* ptr, const wn_size_t new_size,
                                           const wn_size_t alignment) {
  return(static_cast<T*>(aligned_realloc(static_cast<wn_void*>(ptr),
                                         new_size * sizeof(T), alignment)));
}

template <>
WN_FORCE_INLINE wn_void* heap_aligned_reallocate(wn_void* ptr,
                                                 const wn_size_t new_size,
                                                 const wn_size_t alignment) {
  return(aligned_realloc(ptr, new_size, alignment));
}

template <typename T>
WN_FORCE_INLINE wn_void heap_aligned_free(T* ptr) {
  aligned_free(static_cast<wn_void*>(ptr));
}

template <>
WN_FORCE_INLINE wn_void heap_aligned_free(wn_void* ptr) {
  aligned_free(ptr);
}

template <typename T, typename... Args>
T* construct_at(T*, Args&&...);

template <typename T, typename... Args>
WN_FORCE_INLINE T* construct(Args&&... args) {
  static_assert(!std::is_void<T>::value,
    "you cannot construct an object of type void");

  return(construct_at<T, Args...>(heap_allocate<T>(),
                                  std::forward<Args>(args)...));
}

template <typename T>
WN_FORCE_INLINE T* construct_array(const size_t size) {
  static_assert(!std::is_void<T>::value,
    "you cannot construct an object of type void");

  return(new(std::nothrow) T[size]);
}

template <typename T, typename... Args>
WN_FORCE_INLINE T* construct_aligned(Args&&... args) {
  static_assert(!std::is_void<T>::value,
    "you cannot construct an object of type void");

  return(construct_at<T, Args...>(heap_aligned_allocate<T>(),
                                  std::forward<Args>(args)...));
}

template <typename T, typename... Args>
WN_FORCE_INLINE T* construct_at(T* ptr, Args&&... args) {
  static_assert(!std::is_void<T>::value,
    "you cannot construct an object of type void");

  return(new(ptr)T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
WN_FORCE_INLINE T* construct_at(wn_void* ptr, Args&&... args) {
  return(construct_at(static_cast<T*>(ptr), std::forward<Args>(args)...));
}

template <typename T>
WN_FORCE_INLINE wn_void destroy(T* ptr) {
  delete(ptr);
}

template <typename T>
WN_FORCE_INLINE wn_void destroy_array(T* ptr) {
  delete[](ptr);
}

template <typename T>
WN_FORCE_INLINE wn_void destroy_aligned(T* ptr) {
  ptr->~T();

  heap_aligned_free(ptr);
}

template <typename T>
struct default_destroyer {
  default_destroyer() = default;

  template <typename U>
  WN_FORCE_INLINE default_destroyer(const default_destroyer<U>&) {};

  WN_FORCE_INLINE wn_void operator () (T* ptr) const {
    destroy(ptr);
  }
};

template <typename T>
struct default_destroyer<T[]> {
  default_destroyer() = default;

  template <typename U>
  WN_FORCE_INLINE default_destroyer(const default_destroyer<U[]>&) {}

  template <typename U,
            typename = core::enable_if_t<
              std::is_convertible<U(*)[], T(*)[]>::value>>
  WN_FORCE_INLINE wn_void operator() (U* ptr) const {
    destroy_array(ptr);
  }
};

template <typename T>
struct default_aligned_destroyer {
  default_aligned_destroyer() = default;

  template <typename U>
  WN_FORCE_INLINE
  default_aligned_destroyer(const default_aligned_destroyer<U>&) {};

  WN_FORCE_INLINE wn_void operator () (T* ptr) const {
    destroy_aligned(ptr);
  }
};


WN_FORCE_INLINE wn_void* memzero(wn_void* dest, const wn_size_t count) {
  return(internal::basic_traits::memzero(dest, count));
}

WN_FORCE_INLINE wn_void* memset(wn_void* dest, const wn_uint8 value,
                                const wn_size_t count) {
  return(internal::basic_traits::memset(dest, value, count));
}

WN_FORCE_INLINE wn_void* memcpy(wn_void* dest, const wn_void* src,
                                const wn_size_t count) {
  return(internal::basic_traits::memcpy(dest, src, count));
}

WN_FORCE_INLINE wn_void* memmove(wn_void* dest, const wn_void* src,
                                 const wn_size_t count) {
  return(internal::basic_traits::memmove(dest, src, count));
}

WN_FORCE_INLINE wn_int32 memcmp(const wn_void* lhs, const wn_void* rhs,
                                const wn_size_t count) {
  return(internal::basic_traits::memcmp(lhs, rhs, count));
}

template <typename T>
WN_FORCE_INLINE T* memory_zero(T* dest) {
  static_assert(!std::is_void<T>::value,
    "you must specify a size in bytes when zeroing memory of type void");

  return(static_cast<T*>(memzero(dest, sizeof(T))));
}

template <typename T>
WN_FORCE_INLINE T* memory_zero(T* dest, const wn_size_t count) {
  return(static_cast<T*>(memzero(dest, count * sizeof(T))));
}

template <>
WN_FORCE_INLINE wn_void* memory_zero(wn_void* dest, const wn_size_t count) {
  return(memzero(dest, count));
}

template <typename T>
WN_FORCE_INLINE T* memory_set(T* dest, const wn_uint8 value) {
  static_assert(!std::is_void<T>::value,
    "you must specify a size in bytes when setting memory of type void");

  return(static_cast<T*>(memset(dest, value, sizeof(T))));
}

template <typename T>
WN_FORCE_INLINE T* memory_set(T* dest, const wn_uint8 value,
                              const wn_size_t count) {
  return(static_cast<T*>(memset(dest, value, count * sizeof(T))));
}

template <>
WN_FORCE_INLINE wn_void* memory_set(wn_void* dest, const wn_uint8 value,
                                    const wn_size_t count) {
  return(memset(dest, value, count));
}

template <typename T>
WN_FORCE_INLINE T* memory_copy(T* dest, const T* src) {
  static_assert(!std::is_void<T>::value,
    "you must specify a size in bytes when copying memory of type void");

  return(static_cast<T*>(memcpy(dest, src, sizeof(T))));
}

template <typename T>
WN_FORCE_INLINE T* memory_copy(T* dest, const T* src, const wn_size_t count) {
  return(static_cast<T*>(memcpy(dest, src, count * sizeof(T))));
}

template <>
WN_FORCE_INLINE wn_void* memory_copy(wn_void* dest, const wn_void* src,
                                     const wn_size_t count) {
  return(memcpy(dest, src, count));
}

template <typename T>
WN_FORCE_INLINE T* memory_move(T* dest, const T* src) {
  static_assert(!std::is_void<T>::value,
    "you must specify a size in bytes when moving memory of type void");

  return(static_cast<T*>(memmove(dest, src, sizeof(T))));
}

template <typename T>
WN_FORCE_INLINE T* memory_move(T* dest, const T* src, const wn_size_t count) {
  return(static_cast<T*>(memmove(dest, src, count * sizeof(T))));
}

template <>
WN_FORCE_INLINE wn_void* memory_move(wn_void* dest, const wn_void* src,
                                     const wn_size_t count) {
  return(memmove(dest, src, count));
}

template <typename T>
WN_FORCE_INLINE wn_int32 memory_compare(const T* lhs, const T* rhs) {
  static_assert(!std::is_void<T>::value,
    "you must specify a size in bytes when comparing memory of type void");

  return(memcmp(lhs, rhs, sizeof(T)));
}

template <typename T>
WN_FORCE_INLINE wn_int32 memory_compare(const T* lhs, const T* rhs,
                                        const wn_size_t count) {
  return(memcmp(lhs, rhs, count * sizeof(T)));
}

template <>
WN_FORCE_INLINE wn_int32 memory_compare(const wn_void* lhs, const wn_void* rhs,
                                        const wn_size_t count) {
  return(memcmp(lhs, rhs, count));
}

template <typename T>
WN_FORCE_INLINE wn_void prefetch(const T* ptr) {
  internal::basic_traits::prefetch(ptr);
}

template <typename T>
WN_FORCE_INLINE wn_void prefetch(const T* ptr, const wn_size_t offset) {
  prefetch(ptr + offset);
}

template <>
WN_FORCE_INLINE wn_void prefetch(const wn_void* ptr, const wn_size_t offset) {
  prefetch(static_cast<const wn_byte*>(ptr), offset);
}

} // namespace memory
} // namespace wn

#endif // __WN_MEMORY_BASIC_H__