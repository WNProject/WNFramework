// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FILE_SYSTEM_FILE_H__
#define __WN_FILE_SYSTEM_FILE_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNIntrusivePtr.h"

namespace wn {
namespace file_system {

class file : public memory::intrusive_ptr_base {
public:
  typedef uint8_t value_type;
  typedef size_t size_type;
  typedef std::add_pointer<value_type>::type pointer;
  typedef std::add_const<pointer>::type const_pointer;

  virtual ~file() = default;

  virtual pointer data() = 0;
  virtual const_pointer data() const = 0;

  template <typename T>
  WN_FORCE_INLINE T* typed_data() {
    return reinterpret_cast<T*>(data());
  }

  template <typename T>
  WN_FORCE_INLINE const T* typed_data() const {
    return reinterpret_cast<const T*>(data());
  }

  WN_FORCE_INLINE containers::contiguous_range<value_type> range() {
    return containers::contiguous_range<value_type>(data(), size());
  }

  WN_FORCE_INLINE containers::contiguous_range<const value_type> range() const {
    return containers::contiguous_range<const value_type>(data(), size());
  }

  template <typename T>
  WN_FORCE_INLINE containers::contiguous_range<T> typed_range() {
    return containers::contiguous_range<T>(typed_data<T>(), typed_size<T>());
  }

  template <typename T>
  WN_FORCE_INLINE containers::contiguous_range<const T> typed_range() const {
    return containers::contiguous_range<const T>(
        typed_data<const T>(), typed_size<const T>());
  }

  virtual size_type size() const = 0;

  template <typename T>
  WN_FORCE_INLINE size_t typed_size() const {
    return size() / sizeof(T);
  }

  WN_FORCE_INLINE bool empty() const {
    return (size() == 0);
  }

  virtual bool is_open() const = 0;

  virtual bool flush() = 0;
  virtual bool resize(const size_type _size) = 0;

  WN_FORCE_INLINE bool clear() {
    return resize(0);
  }

  virtual void close() = 0;

protected:
  WN_FORCE_INLINE file(memory::allocator* _allocator)
    : memory::intrusive_ptr_base(_allocator) {}
};

using file_ptr = memory::intrusive_ptr<file>;

}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_FILE_H__
