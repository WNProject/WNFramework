// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_PROFILER_H__
#define __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_PROFILER_H__

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNQueueProfiler.h"
#else
#include "core/inc/utilities.h"
#endif

#ifdef TRACY_ENABLE
#include "Tracy.hpp"
#endif

#include <d3d12.h>
#include <wrl.h>

#include <initializer_list>
#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNStringView.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace d3d12 {

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using d3d12_queue_profiler_base = queue_profiler;
#else
using d3d12_queue_profiler_base = core::non_copyable;
#endif

class d3d12_device;
class d3d12_queue;

class d3d12_queue_profiler WN_GRAPHICS_FINAL
  : public d3d12_queue_profiler_base {
public:
  ~d3d12_queue_profiler() WN_GRAPHICS_OVERRIDE_FINAL;

  inline bool is_valid() const WN_GRAPHICS_OVERRIDE_FINAL {
    return (m_device != 0);
  }

#ifdef TRACY_ENABLE
  memory::unique_ptr<gpu_profile_scope> profile_scope(
      command_list* _command_list,
      const tracy::SourceLocationData*) WN_GRAPHICS_OVERRIDE_FINAL;
  void collect() WN_GRAPHICS_OVERRIDE_FINAL;
  void new_frame() WN_GRAPHICS_OVERRIDE_FINAL;
#endif

protected:
  friend struct d3d12_gpu_profiler_scope;
  friend class d3d12_device;
  void initialize(memory::allocator* _allocator, containers::string_view _name,
      d3d12_device* _device, d3d12_queue* _queue);
  uint32_t next_query_id();
  memory::allocator* m_allocator;

  bool m_initialized = false;
  d3d12_device* m_device;
  d3d12_queue* m_queue;
  uint8_t m_tracy_context;
  Microsoft::WRL::ComPtr<ID3D12QueryHeap> m_query_heap;
  Microsoft::WRL::ComPtr<ID3D12Resource> m_readback_buffer;

  uint32_t m_query_limit = 0;
  uint32_t m_query_counter = 0;
  uint32_t m_previous_query_counter = 0;

  uint32_t m_active_payload = 0;
  Microsoft::WRL::ComPtr<ID3D12Fence> m_payload_fence;
  struct frame_query {
    uint32_t query_id_start;
    uint32_t query_count;
  };
  containers::deque<frame_query> m_frame_queries;

  int64_t m_prev_calibration = 0;
  int64_t m_qpc_to_ns = 0;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_PROFILER_H__
