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

// The memory returned by this heap type may or may not be
// coherent. Make sure to flush any range before use. This will
// get optimized away if a flush was not strictly necessary.
class upload_heap {
public:
  ~upload_heap() {
    m_device->destroy_upload_heap(this);
  }

  template <typename T = uint8_t>
  WN_FORCE_INLINE containers::write_only_contiguous_range<T> get_range(
      size_t _offset_in_bytes, size_t _num_elements) {
    uint8_t* base_value = m_root_address + _offset_in_bytes;
    return containers::write_only_contiguous_range<T>(
        reinterpret_cast<T*>(base_value), _num_elements);
  }

  template <typename T = uint8_t>
  WN_FORCE_INLINE void flush_range(
      const containers::write_only_contiguous_range<T>& _range) {
    m_device->flush_mapped_range(this,
        reinterpret_cast<const uint8_t*>(_range.data()) - m_root_address,
        sizeof(T) * _range.size());
  }

  WN_FORCE_INLINE upload_heap(upload_heap&& _other) {
    ::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    m_root_address = _other.m_root_address;
    m_device = _other.m_device;

    _other.m_root_address = nullptr;
    memory::memset(&_other.m_data, 0x0, sizeof(opaque_data));
  }

  bool is_valid() const {
    return m_device != nullptr;
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
  // The opaque_data must be trivially copyable.
  // It also must be considered uninitialized when
  // memset to 0.
  struct opaque_data {
    uint64_t _dummy[2];
  } m_data;

  uint8_t* m_root_address;
  device* m_device;
};

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_HEAP_H__