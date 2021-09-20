// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_RENDERER_GPU_ALLOCATOR_H__
#define __WN_ENGINE_RENDERER_GPU_ALLOCATOR_H__

#include "WNContainers/inc/WNRangePartition.h"
#include "WNGraphics/inc/WNDevice.h"

namespace wn {
namespace engine {
namespace renderer {

using heap_range =
    containers::range_partition<containers::default_node_allocator<uint64_t>,
        uint64_t>;

class gpu_heap;

class gpu_allocation final {
public:
  gpu_allocation() : m_parent(nullptr) {}
  ~gpu_allocation();

  runtime::graphics::arena* arena() {
    return m_arena;
  }
  uint64_t offset() const {
    return m_offset;
  }

  bool is_valid() const {
    return m_parent != nullptr;
  }

  gpu_allocation(gpu_allocation&& _other)
    : m_arena(_other.m_arena),
      m_offset(_other.m_offset),
      m_token(core::move(_other.m_token)),
      m_parent(_other.m_parent) {
    _other.m_parent = nullptr;
  }

  gpu_allocation& operator=(gpu_allocation&& _other) {
    m_arena = _other.m_arena;
    m_offset = _other.m_offset;
    m_token = core::move(_other.m_token);
    m_parent = _other.m_parent;
    _other.m_parent = nullptr;
    return *this;
  }

private:
  friend class gpu_heap;
  gpu_allocation(runtime::graphics::arena* _arena, uint64_t _offset,
      heap_range::token _token, gpu_heap* _parent);
  runtime::graphics::arena* m_arena = nullptr;
  uint64_t m_offset = 0;
  heap_range::token m_token;
  gpu_heap* m_parent = nullptr;
};

class gpu_heap final {
public:
  gpu_heap(memory::allocator* _allocator, runtime::graphics::device* _device,
      uint32_t arena_index, uint64_t size);
  gpu_allocation allocate_memory(uint64_t size, uint64_t alignment);
  ~gpu_heap();

private:
  memory::unique_ptr<runtime::graphics::arena> m_arena;
  heap_range m_partition;
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_RENDERER_GPU_ALLOCATOR_H__
