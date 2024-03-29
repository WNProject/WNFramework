// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_RUNTIME_GRAPHICS_BUFFER_H__
#define __WN_RUNTIME_GRAPHICS_BUFFER_H__

#include "WNGraphics/inc/WNArena.h"
#include "WNGraphics/inc/WNGraphicsObjectBase.h"
#include "WNMemory/inc/string_utility.h"
#include "core/inc/types.h"

WN_GRAPHICS_FORWARD(device);
WN_GRAPHICS_FORWARD(command_list);

namespace wn {
namespace runtime {
namespace graphics {

struct buffer_memory_requirements {
  uint64_t size;
  uint64_t alignment;
};

class arena;

class buffer final : base_object<2> {
private:
  using base = base_object<2>;

public:
  inline buffer(buffer&& _other)
    : m_device(core::move(_other.m_device)),
      m_size(_other.m_size),
      m_memory_size(_other.m_memory_size),
      m_memory_alignment(_other.m_memory_alignment) {
    _other.m_device = nullptr;
    _other.m_size = 0;
    _other.m_memory_size = 0;
    _other.m_memory_alignment = 0;

    memory::memory_copy(&m_data, &_other.m_data);
    memory::memory_zero(&_other.m_data);
  }

  inline ~buffer() {
    if (is_valid()) {
      m_device->destroy_buffer(this);
    }
  }

  inline buffer& operator=(buffer&& _other) {
    m_device = core::move(_other.m_device);
    m_size = _other.m_size;
    m_memory_size = _other.m_memory_size;
    m_memory_alignment = _other.m_memory_alignment;

    _other.m_device = nullptr;
    _other.m_size = 0;
    _other.m_memory_size = 0;
    _other.m_memory_alignment = 0;

    memory::memory_copy(&m_data, &_other.m_data);
    memory::memory_zero(&_other.m_data);
    return *this;
  }

  inline bool bind_memory(arena* _arena, const uint64_t _offset) {
    WN_DEBUG_ASSERT(_offset + size() <= _arena->size(),
        "binding is out of bounds of arena");

    m_bound = m_device->bind_buffer(this, _arena, _offset);

    return m_bound;
  }

  inline bool bind_memory(arena* _arena) {
    return bind_memory(_arena, 0);
  }

  inline void* map() {
    return m_device->map_buffer(this);
  }

  inline void unmap() {
    m_device->unmap_buffer(this);
  }

  inline bool is_valid() const {
    return (m_device != nullptr && size() != 0);
  }

  inline bool is_bound() const {
    return (is_valid() && m_bound);
  }

  inline uint64_t size() const {
    return m_size;
  }

  inline uint64_t memory_size() const {
    return m_memory_size;
  }

  buffer_memory_requirements get_memory_requirements() const {
    return buffer_memory_requirements{m_memory_size, m_memory_alignment};
  }

  inline buffer() : base(), m_device(nullptr), m_size(0), m_bound(false) {}

private:
  WN_GRAPHICS_ADD_FRIENDS(device);
  WN_GRAPHICS_ADD_FRIENDS(command_list);

  inline buffer(device* _device, const size_t _size)
    : base(), m_device(_device), m_size(_size), m_bound(false) {}

  device* m_device;
  uint64_t m_size;
  uint64_t m_memory_size;
  uint64_t m_memory_alignment;
  bool m_bound;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  //  __WN_RUNTIME_GRAPHICS_BUFFER_H__
