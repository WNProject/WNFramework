// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MEMORY_MANIPULATION_H__
#define __WN_MEMORY_MANIPULATION_H__

#include "core/inc/type_traits.h"
#include "core/inc/types.h"

#include <cstring>

namespace wn {
namespace memory {

inline void* memset(void* _dest, uint8_t _value, size_t _count) {
  return std::memset(_dest, _value, _count);
}

inline void* memzero(void* _dest, size_t _count) {
  return memset(_dest, 0, _count);
}

inline void* memcpy(void* _dest, const void* _src, size_t _count) {
  return std::memcpy(_dest, _src, _count);
}

inline void* memmove(void* _dest, const void* _src, size_t _count) {
  return std::memmove(_dest, _src, _count);
}

inline int32_t memcmp(const void* _lhs, const void* _rhs, size_t _count) {
  return std::memcmp(_lhs, _rhs, _count);
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
      "you must specify a size in bytes when moving memory of type void");

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
      "you must specify a size in bytes when comparing memory of type void");

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
  (void)_ptr;  // currently does nothing
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

#endif  // __WN_MEMORY_MANIPULATION_H__
