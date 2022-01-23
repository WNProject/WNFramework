// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_FILE_H__
#define __WN_FILE_SYSTEM_FILE_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNFileSystem/inc/object_info.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/intrusive_ptr.h"
#include "core/inc/type_traits.h"

namespace wn {
namespace file_system {

class file : public memory::intrusive_ptr_base {
public:
  using value_type = uint8_t;
  using size_type = object_info::size_type;
  using pointer = core::add_pointer<value_type>::type;
  using const_pointer = core::add_const<pointer>::type;

  virtual ~file() = default;

  virtual pointer data() = 0;
  virtual const_pointer data() const = 0;

  template <typename T>
  T* typed_data() {
    return reinterpret_cast<T*>(data());
  }

  template <typename T>
  const T* typed_data() const {
    return reinterpret_cast<const T*>(data());
  }

  containers::contiguous_range<value_type> range() {
    return containers::contiguous_range<value_type>(data(), size());
  }

  containers::contiguous_range<const value_type> range() const {
    return containers::contiguous_range<const value_type>(data(), size());
  }

  template <typename T>
  containers::contiguous_range<T> typed_range() {
    return containers::contiguous_range<T>(typed_data<T>(), typed_size<T>());
  }

  template <typename T>
  containers::contiguous_range<const T> typed_range() const {
    return containers::contiguous_range<const T>(
        typed_data<const T>(), typed_size<const T>());
  }

  virtual object_info info() const = 0;

  size_type size() const {
    return info().size;
  }

  template <typename T>
  size_type typed_size() const {
    return size() / sizeof(T);
  }

  bool empty() const {
    return (size() == 0);
  }

  virtual bool is_open() const = 0;

  virtual bool flush() = 0;
  virtual bool resize(const size_type _size) = 0;

  bool clear() {
    return resize(0);
  }

  virtual void close() = 0;

protected:
  file(memory::allocator* _allocator)
    : memory::intrusive_ptr_base(_allocator) {}
};

using file_ptr = memory::intrusive_ptr<file>;

}  // namespace file_system
}  // namespace wn

#endif  // __WN_FILE_SYSTEM_FILE_H__
