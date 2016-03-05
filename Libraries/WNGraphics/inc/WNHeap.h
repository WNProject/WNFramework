// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_HEAP_H__
#define __WN_GRAPHICS_HEAP_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNMemory/inc/WNUniquePtr.h"

namespace wn {
namespace graphics {

template <typename T>
class write_only_value {
public:
  WN_FORCE_INLINE write_only_value& operator=(const T& _other) {
    m_t = _other;
    return *this;
  }

  write_only_value(const write_only_value& _other) = delete;
  write_only_value(write_only_value&& _other) = delete;

private:
  template <typename F>
  friend class write_only_range;
  volatile T m_t;
};

template <typename T>
class write_only_range
    : public containers::contiguous_range<write_only_value<T>> {
  // Make sure that the types are the same size, and that
  // they will line up when put in an array.
  static_assert(sizeof(write_only_value<T>[2]) == sizeof(T[2]),
      "The size of the write-only value should be the same as the"
      "size of the underlying type");
  using range_type = containers::contiguous_range<write_only_value<T>>;

public:
  using value_type = typename range_type::value_type;
  using size_type = typename range_type::size_type;
  using difference_type = typename range_type::difference_type;
  using pointer = typename range_type::pointer;
  using const_pointer = typename range_type::const_pointer;
  using reference = typename range_type::reference;
  using const_reference = typename range_type::const_reference;
  using iterator = typename range_type::iterator;
  using const_iterator = typename range_type::const_iterator;
  using reverse_iterator = typename range_type::reverse_iterator;
  using const_reverse_iterator = typename range_type::const_reverse_iterator;

  ~write_only_range() {}

  WN_FORCE_INLINE void memcpy_in(
      size_t _offset, T* _source, size_t num_elements) {
    memory::memory_copy(
        reinterpret_cast<T*>(range_type::data()), _source, num_elements);
  }

private:
  write_only_range(size_t _offset, T* _value, size_t _num_elements)
    : range_type(reinterpret_cast<write_only_value<T>*>(_value), _num_elements),
      m_offset(_offset) {}

  size_t m_offset;
  friend class upload_heap;
};

// TODO(awoloszyn): Investigate if we really want to keep the explicit flush.
// It is not strictly necessary for the current implementation of our DX12/Vk
// upload heaps, but if we change the coherency properties we may get better
// performance (but need more flushing).
class upload_heap {
public:
  ~upload_heap() {
    m_device->destroy_upload_heap(this);
  }

  template <typename T = uint8_t>
  WN_FORCE_INLINE write_only_range<T> get_range(
      size_t _offset_in_bytes, size_t _num_elements) {
    uint8_t* base_value =
        static_cast<uint8_t*>(m_root_address) + _offset_in_bytes;
    return write_only_range<T>(_offset_in_bytes, base_value, _num_elements);
  }

  template <typename T = uint8_t>
  WN_FORCE_INLINE void flush_range(const write_only_range<T>& _range) {
    m_device->flush_mapped_range(
        *this, _range.m_offset, sizeof(T) * _range.size());
  }

protected:
  WN_FORCE_INLINE upload_heap(device* _device)
    : m_device(_device), m_root_address(0), m_data({0, 0}) {}

  template <typename T>
  T& data_as() {
    static_assert(sizeof(opaque_data) >= sizeof(T), "Invalid cast");
    return *reinterpret_cast<T*>(&m_data);
  }

#include "WNGraphics/inc/Internal/WNSetFriendDevices.h"
  struct opaque_data {
    uint64_t _dummy[2];
  } m_data;

  void* m_root_address;
  device* m_device;
};

using upload_heap_ptr = memory::unique_ptr<upload_heap>;
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_HEAP_H__