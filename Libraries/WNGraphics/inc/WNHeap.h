// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_HEAP_H__
#define __WN_GRAPHICS_HEAP_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNHeapTraits.h"
#include "WNMemory/inc/WNBasic.h"

namespace wn {
namespace graphics {

template <typename T, typename HeapTraits>
class heap_buffer {
public:
  using range_type = typename HeapTraits::template range_type<T>;

  ~heap_buffer();

  heap_buffer(heap_buffer&& _other)
    : m_heap(core::move(_other.m_heap)), m_range(core::move(_other.m_range)) {
    _other.m_heap = nullptr;
  }

  range_type& range() {
    return m_range;
  }

private:
  heap_buffer(heap<HeapTraits>* _heap, size_t _offset,
      typename HeapTraits::template range_type<T>&& _range);

  heap<HeapTraits>* m_heap;
  size_t m_offset;
  typename HeapTraits::template range_type<T> m_range;
  template <typename F>
  friend class heap;
};

// The memory returned by this heap type may or may not be
// coherent. Make sure to flush any range before use. This will
// get optimized away if a flush was not strictly necessary.
template <typename HeapTraits>
class heap {
public:
  ~heap() {
    m_device->destroy_heap(this);
  }

  template <typename T = uint8_t>
  WN_FORCE_INLINE heap_buffer<T, HeapTraits> get_range(
      size_t _offset_in_bytes, size_t _num_elements) {
    uint8_t* root = m_device->acquire_range(
        this, _offset_in_bytes, sizeof(T) * _num_elements);
    typename HeapTraits::template range_type<T> range(
        reinterpret_cast<T*>(root), _num_elements);

    return heap_buffer<T, HeapTraits>(
        this, _offset_in_bytes, core::move(range));
  }

  bool is_valid() const {
    return m_device != nullptr;
  }

  WN_FORCE_INLINE heap(heap&& _other) {
    ::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    m_root_address = _other.m_root_address;
    m_device = _other.m_device;

    _other.m_root_address = nullptr;
    memory::memset(&_other.m_data, 0x0, sizeof(opaque_data));
  }

private:
  WN_FORCE_INLINE heap(device* _device)
    : m_device(_device), m_root_address(0), m_data({0, 0}) {}

  template <typename T = uint8_t>
  void release_range(heap_buffer<T, HeapTraits>& _range) {
    m_device->release_range(
        this, _range.m_offset, sizeof(T) * _range.range().size());
  }

  template <typename T>
  T& data_as() {
    static_assert(sizeof(opaque_data) >= sizeof(T), "Invalid cast");
    return *reinterpret_cast<T*>(&m_data);
  }

  template <typename T, typename HT>
  friend class heap_buffer;

#include "WNGraphics/inc/Internal/WNSetFriendDevices.h"
  // The opaque_data must be trivially copyable.
  // It also must be considered uninitialized when
  // memset to 0.
  struct opaque_data {
    uint64_t _dummy[2];
  } m_data;

  uint8_t* m_root_address;
  device* m_device;
};

template <typename T, typename HeapTraits>
heap_buffer<T, HeapTraits>::heap_buffer(
    heap<HeapTraits>* _heap, size_t _offset, range_type&& _range)
  : m_heap(_heap), m_offset(_offset), m_range(core::move(_range)) {}

template <typename T, typename HeapTraits>
heap_buffer<T, HeapTraits>::~heap_buffer() {
  if (m_heap) {
    m_heap->release_range(*this);
  }
}

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_HEAP_H__