// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INC_INTERNAL_VULKAN_QUEUE_PROFILER_H__
#define __WN_GRAPHICS_INC_INTERNAL_VULKAN_QUEUE_PROFILER_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNContainers/inc/WNDeque.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNHashSet.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanQueueProfilerContext.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNMultiTasking/inc/spin_lock.h"
#include "core/inc/pair.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNQueueProfiler.h"
#else
#include "core/inc/utilities.h"
#endif

#ifdef TRACY_ENABLE
#include "Tracy.hpp"
#endif

#include <initializer_list>

namespace wn {
namespace runtime {
namespace graphics {
struct gpu_profile_scope;
namespace internal {
namespace vulkan {

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using vulkan_queue_profiler_base = queue_profiler;
#else
using vulkan_queue_profiler_base = core::non_copyable;
#endif

class vulkan_device;
class vulkan_queue;

class vulkan_queue_profiler WN_GRAPHICS_FINAL
  : public vulkan_queue_profiler_base {
public:
  ~vulkan_queue_profiler() WN_GRAPHICS_OVERRIDE_FINAL;

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
  friend class vulkan_device;
  friend struct vulkan_gpu_profile_scope;
  void initialize(memory::allocator* _allocator, containers::string_view _name,
      vulkan_device* _device, vulkan_queue* _queue, float _timestamp_period,
      vulkan_queue_profiler_context* _queue_profiler_context);

  void calibrate(int64_t* cpu_time, int64_t* gpu_time);
  VkCommandBuffer get_command_buffer();
  VkFence get_fence();

  void release_command_buffer(VkCommandBuffer command_buffer);
  void release_fence(VkFence fence);
  uint32_t next_query_id();

  memory::allocator* m_allocator;

  vulkan_device* m_device;
  vulkan_queue* m_queue;
  vulkan_queue_profiler_context* m_context;
  float m_timestamp_period;

  VkQueryPool m_query_pool;
  VkCommandPool m_command_pool;
  VkTimeDomainEXT m_time_domain;
  containers::hash_map<VkFence, VkCommandBuffer> m_command_buffers;
  containers::deque<VkFence> m_fence_pool;
  containers::deque<VkCommandBuffer> m_command_buffer_pool;
  containers::dynamic_array<int64_t> m_queries;

  multi_tasking::spin_lock m_head_lock;
  uint32_t m_head;
  uint32_t m_tail;
  uint32_t m_old_count;
  uint32_t m_query_count;
  uint64_t m_deviation;
  int64_t m_convert_to_ns;
  int64_t m_prev_calibration;
  uint8_t m_tracy_context;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INC_INTERNAL_VULKAN_QUEUE_PROFILER_H__
