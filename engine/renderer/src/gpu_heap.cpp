// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "renderer/inc/gpu_heap.h"

#include "WNGraphics/inc/WNArena.h"
#include "WNGraphics/inc/WNDevice.h"

namespace wn {
namespace engine {
namespace renderer {

gpu_heap::gpu_heap(memory::allocator* _allocator,
    runtime::graphics::device* _device, uint32_t _arena_index, uint64_t _size)
  : m_partition(_allocator, _size) {
  m_arena = memory::make_unique<runtime::graphics::arena>(
      _allocator, _device->create_arena(_arena_index, _size));
}

gpu_allocation gpu_heap::allocate_memory(uint64_t size, uint64_t alignment) {
  heap_range::token tok = m_partition.get_interval(size, alignment);
  if (!tok.is_valid()) {
    return gpu_allocation(nullptr, 0, heap_range::token(), nullptr);
  }
  uint64_t offset = tok.offset();
  return gpu_allocation(m_arena.get(), offset, core::move(tok), this);
}

gpu_heap::~gpu_heap() {}

gpu_allocation::gpu_allocation(runtime::graphics::arena* _arena,
    uint64_t _offset, heap_range::token _token, gpu_heap* _parent)
  : m_arena(_arena),
    m_offset(_offset),
    m_token(core::move(_token)),
    m_parent(_parent) {}

gpu_allocation::~gpu_allocation() {}

}  // namespace renderer
}  // namespace engine
}  // namespace wn
