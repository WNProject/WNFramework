// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Inspired  by "TracyVulkan.hpp"

#include "WNGraphics/inc/Internal/Vulkan/WNQueueProfiler.h"
#include "WNGraphics/inc/Internal/Vulkan/WNCommandList.h"
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#include "WNGraphics/inc/Internal/Vulkan/WNQueue.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {
#ifdef TRACY_ENABLE

const uint32_t k_query_count = (64 * 1024);

const VkCommandBufferAllocateInfo k_command_buffer_allocate_info{
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,  // sType
    nullptr,                                         // pNext
    0,                                               // commandPool
    VK_COMMAND_BUFFER_LEVEL_PRIMARY,                 // level
    1                                                // commandBufferCount
};

const VkCommandBufferBeginInfo k_command_buffer_begin{
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // sType
    nullptr,                                      // pNext
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT   // flags
};

const VkSubmitInfo k_submit_info{
    VK_STRUCTURE_TYPE_SUBMIT_INFO,  // sType
    nullptr,                        // pNext
    0,                              // waitSemaphoreCount
    nullptr,                        // pWaitSemaphores
    nullptr,                        // pWaitDstStageMask
    1,                              // commandBufferCount
    nullptr,                        // pCommandBuffers
    0,                              // signalSemaphoreCount
    nullptr                         // pSignalSemaphores
};

const VkFenceCreateInfo k_fence_info{
    VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,  // sType
    nullptr,                              // pNext
    0,                                    // flags
};
#endif  // TRACY_ENABLE

vulkan_queue_profiler::~vulkan_queue_profiler() {
#ifdef TRACY_ENABLE
  for (auto it : m_command_buffers) {
    m_context->vkWaitForFences(
        m_device->m_device, 1, &it.first, VK_TRUE, 0xFFFFFFFFFFFFFFFF);
    m_context->vkDestroyFence(m_device->m_device, it.first, nullptr);
    m_context->vkFreeCommandBuffers(
        m_device->m_device, m_command_pool, 1, &it.second);
  }

  for (auto& it : m_command_buffer_pool) {
    m_context->vkFreeCommandBuffers(m_device->m_device, m_command_pool, 1, &it);
  }

  for (auto& it : m_fence_pool) {
    m_context->vkDestroyFence(m_device->m_device, it, nullptr);
  }

  m_context->vkDestroyCommandPool(m_device->m_device, m_command_pool, nullptr);
  m_context->vkDestroyQueryPool(m_device->m_device, m_query_pool, nullptr);
#endif

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  if (is_valid()) {
    m_device->destroy_queue_profiler(this);
  }
#endif
}

void vulkan_queue_profiler::initialize(memory::allocator* _allocator,
    containers::string_view _name, vulkan_device* _device, vulkan_queue* _queue,
    float _timestamp_period, vulkan_queue_profiler_context* _context) {
  (void)_allocator;
  (void)_name;
  (void)_device;
  (void)_queue;
  (void)_timestamp_period;
  (void)_context;
#ifdef TRACY_ENABLE
  m_head = 0;
  m_tail = 0;
  m_old_count = 0;
  m_query_count = 0;
  m_time_domain = VK_TIME_DOMAIN_DEVICE_EXT;
  m_allocator = _allocator;
  m_device = _device;
  m_context = _context;
  m_queue = _queue;
  m_tracy_context =
      tracy::GetGpuCtxCounter().fetch_add(1, std::memory_order_relaxed);
  m_timestamp_period = _timestamp_period;
  m_command_buffers =
      containers::hash_map<VkFence, VkCommandBuffer>(_allocator);
  m_fence_pool = containers::deque<VkFence>(_allocator);
  m_command_buffer_pool = containers::deque<VkCommandBuffer>(_allocator);
  m_queries = containers::dynamic_array<int64_t>(_allocator);

  {
    const VkCommandPoolCreateInfo command_pool_create_info{
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,       // sType
        nullptr,                                          // pNExt
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,  // flags
        m_queue->m_queue_index                            // queueFamilyIndex
    };

    WN_RELEASE_ASSERT(
        VK_SUCCESS == m_context->vkCreateCommandPool(m_device->m_device,
                          &command_pool_create_info, nullptr, &m_command_pool),
        "Cannot create a command pool");
  }
  {
    VkQueryPoolCreateInfo query_pool_create_info = {
        VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,  // sType
        nullptr,                                   // pNext
        0,                                         // flags
        VK_QUERY_TYPE_TIMESTAMP,                   // queryType
        k_query_count,                             // queryCount
        0,                                         // pipelineStatistics
    };
    while (VK_SUCCESS != m_context->vkCreateQueryPool(m_device->m_device,
                             &query_pool_create_info, nullptr, &m_query_pool)) {
      query_pool_create_info.queryCount >>= 1;
      WN_RELEASE_ASSERT(query_pool_create_info.queryCount,
          "Could not allocate ANY query pool");
    }
    m_query_count = query_pool_create_info.queryCount;
  }

  if (m_context->vkGetCalibratedTimestampsEXT &&
      m_context->vkGetPhysicalDeviceCalibrateableTimeDomainsEXT) {
    uint32_t num_domains;
    WN_RELEASE_ASSERT(
        VK_SUCCESS == m_context->vkGetPhysicalDeviceCalibrateableTimeDomainsEXT(
                          m_context->physical_device, &num_domains, nullptr),
        "Could not calibrate timestamps");
    containers::dynamic_array<VkTimeDomainEXT> time_domains(
        m_allocator, num_domains);
    WN_RELEASE_ASSERT(
        VK_SUCCESS ==
            m_context->vkGetPhysicalDeviceCalibrateableTimeDomainsEXT(
                m_context->physical_device, &num_domains, time_domains.data()),
        "Could not calibrate timestamps");

    VkTimeDomainEXT supported_domain = VK_TIME_DOMAIN_MAX_ENUM_EXT;
#if defined _WIN32 || defined __CYGWIN__
    supported_domain = VK_TIME_DOMAIN_QUERY_PERFORMANCE_COUNTER_EXT;
#elif defined __linux__ && defined CLOCK_MONOTONIC_RAW
    supported_domain = VK_TIME_DOMAIN_CLOCK_MONOTONIC_RAW_EXT;
#endif

    for (size_t i = 0; i < num_domains; ++i) {
      if (time_domains[i] == supported_domain) {
        m_time_domain = time_domains[i];
        break;
      }
    }
  }

  VkCommandBuffer command_buffer = get_command_buffer();

  m_context->vkBeginCommandBuffer(command_buffer, &k_command_buffer_begin);
  m_context->vkCmdResetQueryPool(
      command_buffer, m_query_pool, 0, m_query_count);
  m_context->vkEndCommandBuffer(command_buffer);

  auto submit_info = k_submit_info;
  submit_info.pCommandBuffers = &command_buffer;
  m_context->vkQueueSubmit(m_queue->m_queue, 1, &submit_info, VK_NULL_HANDLE);
  m_context->vkQueueWaitIdle(m_queue->m_queue);

  int64_t cpu_time;
  int64_t gpu_time;

  if (m_time_domain == VK_TIME_DOMAIN_DEVICE_EXT) {
    m_context->vkBeginCommandBuffer(command_buffer, &k_command_buffer_begin);
    m_context->vkCmdWriteTimestamp(
        command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_query_pool, 0);
    m_context->vkEndCommandBuffer(command_buffer);
    m_context->vkQueueSubmit(m_queue->m_queue, 1, &submit_info, VK_NULL_HANDLE);
    m_context->vkQueueWaitIdle(m_queue->m_queue);

    cpu_time = tracy::Profiler::GetTime();
    m_context->vkGetQueryPoolResults(m_device->m_device, m_query_pool, 0, 1,
        sizeof(int64_t), &gpu_time, sizeof(int64_t),
        VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);

    m_context->vkBeginCommandBuffer(command_buffer, &k_command_buffer_begin);
    m_context->vkCmdResetQueryPool(
        command_buffer, m_query_pool, 0, m_query_count);
    m_context->vkEndCommandBuffer(command_buffer);
    m_context->vkQueueSubmit(m_queue->m_queue, 1, &submit_info, VK_NULL_HANDLE);
    m_context->vkQueueWaitIdle(m_queue->m_queue);
  } else {
    const uint32_t probe_count = 32;
    VkCalibratedTimestampInfoEXT spec[2] = {
        {VK_STRUCTURE_TYPE_CALIBRATED_TIMESTAMP_INFO_EXT, nullptr,
            VK_TIME_DOMAIN_DEVICE_EXT},
        {VK_STRUCTURE_TYPE_CALIBRATED_TIMESTAMP_INFO_EXT, nullptr,
            m_time_domain},
    };

    uint64_t _scratch[2];
    uint64_t deviation[probe_count];
    for (size_t i = 0; i < probe_count; ++i) {
      m_context->vkGetCalibratedTimestampsEXT(
          m_device->m_device, 2, spec, _scratch, deviation + i);
    }

    uint64_t min_dev = deviation[0];
    for (size_t i = 1; i < probe_count; ++i) {
      if (deviation[i] < min_dev) {
        min_dev = deviation[i];
      }
    }

    m_deviation = min_dev * 3 / 2;

    int64_t fq = tracy::GetFrequencyQpc();
    m_convert_to_ns = fq ? static_cast<int64_t>(1000000000. / fq) : 1;

    calibrate(&m_prev_calibration, &gpu_time);
    cpu_time = tracy::Profiler::GetTime();
  }
  m_command_buffer_pool.push_back(command_buffer);

  uint8_t flags = m_time_domain == VK_TIME_DOMAIN_DEVICE_EXT
                      ? 0
                      : tracy::GpuContextCalibration;
  {
    auto item = tracy::Profiler::QueueSerial();
    tracy::MemWrite(&item->hdr.type, tracy::QueueType::GpuNewContext);
    tracy::MemWrite(&item->gpuNewContext.cpuTime, cpu_time);
    tracy::MemWrite(&item->gpuNewContext.gpuTime, gpu_time);
    memory::memset(
        &item->gpuNewContext.thread, 0, sizeof(item->gpuNewContext.thread));
    tracy::MemWrite(&item->gpuNewContext.period, m_timestamp_period);
    tracy::MemWrite(&item->gpuNewContext.context, m_tracy_context);
    tracy::MemWrite(&item->gpuNewContext.flags, flags);
    tracy::MemWrite(&item->gpuNewContext.type, tracy::GpuContextType::Vulkan);

#ifdef TRACY_ON_DEMAND
    tracy::GetProfiler().DeferItem(*item);
#endif
    tracy::Profiler::QueueSerialFinish();
  }
  void* nm = tracy::tracy_malloc(_name.size());
  memory::memcpy(nm, _name.data(), _name.size());
  {
    auto* item = tracy::Profiler::QueueSerial();
    tracy::MemWrite(&item->hdr.type, tracy::QueueType::GpuContextName);
    tracy::MemWrite(&item->gpuContextNameFat.context, m_tracy_context);
    tracy::MemWrite(&item->gpuContextNameFat.ptr, (uint64_t)nm);
    tracy::MemWrite(&item->gpuContextNameFat.size, _name.size());
#ifdef TRACY_ON_DEMAND
    tracy::GetProfiler().DeferItem(*item);
#endif
    tracy::Profiler::QueueSerialFinish();
  }
  m_queries.resize(m_query_count);
#endif
}

uint32_t vulkan_queue_profiler::next_query_id() {
  multi_tasking::spin_lock_guard lg(m_head_lock);
  const auto id = m_head;
  m_head = (m_head + 1) % m_query_count;
  assert(m_head != m_tail);
  return id;
}

void vulkan_queue_profiler::calibrate(int64_t* cpu_time, int64_t* gpu_time) {
  (void)cpu_time;
  (void)gpu_time;
#ifdef TRACY_ENABLE
  WN_RELEASE_ASSERT(m_time_domain != VK_TIME_DOMAIN_DEVICE_EXT,
      "Cannot calibrate with unknown time domain");
  VkCalibratedTimestampInfoEXT spec[2] = {
      {VK_STRUCTURE_TYPE_CALIBRATED_TIMESTAMP_INFO_EXT, nullptr,
          VK_TIME_DOMAIN_DEVICE_EXT},
      {VK_STRUCTURE_TYPE_CALIBRATED_TIMESTAMP_INFO_EXT, nullptr, m_time_domain},
  };
  uint64_t ts[2];
  uint64_t deviation;
  do {
    m_context->vkGetCalibratedTimestampsEXT(
        m_device->m_device, 2, spec, ts, &deviation);
  } while (deviation > m_deviation);

  *gpu_time = ts[0];
  *cpu_time = ts[1] * m_convert_to_ns;
#endif
}

VkCommandBuffer vulkan_queue_profiler::get_command_buffer() {
#ifdef TRACY_ENABLE
  if (m_command_buffer_pool.empty()) {
    VkCommandBuffer cb;
    auto cbai = k_command_buffer_allocate_info;
    cbai.commandPool = m_command_pool;

    m_context->vkAllocateCommandBuffers(m_device->m_device, &cbai, &cb);
    return cb;
  }
  auto v = m_command_buffer_pool.back();
  m_command_buffer_pool.pop_back();
  m_context->vkResetCommandBuffer(
      v, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
  return v;
#else
  return VK_NULL_HANDLE;
#endif
}

VkFence vulkan_queue_profiler::get_fence() {
#ifdef TRACY_ENABLE
  if (m_fence_pool.empty()) {
    VkFence fence;
    auto fci = k_fence_info;
    m_context->vkCreateFence(m_device->m_device, &fci, nullptr, &fence);
    return fence;
  }
  auto v = m_fence_pool.back();
  m_fence_pool.pop_back();
  m_context->vkResetFences(m_device->m_device, 1, &v);
  return v;
#else
  return VK_NULL_HANDLE;
#endif
}

void vulkan_queue_profiler::release_command_buffer(
    VkCommandBuffer command_buffer) {
  (void)command_buffer;
#ifdef TRACY_ENABLE
  m_command_buffer_pool.push_back(command_buffer);
#endif
}
void vulkan_queue_profiler::release_fence(VkFence fence) {
  (void)fence;
#ifdef TRACY_ENABLE
  m_fence_pool.push_back(fence);
#endif
}

#ifdef TRACY_ENABLE

struct vulkan_gpu_profile_scope : public gpu_profile_scope {
  ~vulkan_gpu_profile_scope() {
    if (!m_active) {
      return;
    }
    const auto id = m_profiler->next_query_id();
    m_profiler->m_context->vkCmdWriteTimestamp(m_command_buffer,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_profiler->m_query_pool, id);

    auto item = tracy::Profiler::QueueSerial();
    tracy::MemWrite(&item->hdr.type, tracy::QueueType::GpuZoneEndSerial);
    tracy::MemWrite(&item->gpuZoneEnd.cpuTime, tracy::Profiler::GetTime());
    tracy::MemWrite(&item->gpuZoneEnd.thread, tracy::GetThreadHandle());
    tracy::MemWrite(&item->gpuZoneEnd.queryId, static_cast<uint16_t>(id));
    tracy::MemWrite(&item->gpuZoneEnd.context, m_profiler->m_tracy_context);
    tracy::Profiler::QueueSerialFinish();
  }

  bool m_active;
  VkCommandBuffer m_command_buffer;
  vulkan_queue_profiler* m_profiler;
};

void vulkan_queue_profiler::new_frame() {}

memory::unique_ptr<gpu_profile_scope> vulkan_queue_profiler::profile_scope(
    command_list* _command_list, const tracy::SourceLocationData* loc) {
  static int x = 0;
  auto gps = memory::make_unique<vulkan_gpu_profile_scope>(m_allocator);
#ifdef TRACY_ON_DEMAND
  gps->m_active = tracy::GetProfiler()::IsConnected();
#else
  gps->m_active = true;
#endif
  gps->m_profiler = this;

  auto cl = reinterpret_cast<vulkan_command_list*>(_command_list);

  gps->m_command_buffer = cl->m_command_buffer;
  uint32_t id = next_query_id();
  m_context->vkCmdWriteTimestamp(gps->m_command_buffer,
      VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_query_pool, id);

#if WN_TRACY_CALLBACK_DEPTH > 0
  auto item = tracy::Profiler::QueueSerialCallstack(
      tracy::Callstack(WN_TRACY_CALLBACK_DEPTH + 1));
#else
  auto item = tracy::Profiler::QueueSerial();
#endif
  tracy::MemWrite(&item->hdr.type, tracy::QueueType::GpuZoneBeginSerial);
  tracy::MemWrite(&item->gpuZoneBegin.cpuTime, tracy::Profiler::GetTime());
  tracy::MemWrite(&item->gpuZoneBegin.srcloc, (uint64_t)loc);
  tracy::MemWrite(&item->gpuZoneBegin.thread, tracy::GetThreadHandle());
  tracy::MemWrite(&item->gpuZoneBegin.queryId, static_cast<uint16_t>(id));
  tracy::MemWrite(&item->gpuZoneBegin.context, m_tracy_context);
  tracy::Profiler::QueueSerialFinish();
  return gps;
}

void vulkan_queue_profiler::collect() {
  for (auto it = m_command_buffers.begin(); it != m_command_buffers.end();) {
    if (VK_SUCCESS ==
        m_context->vkGetFenceStatus(m_device->m_device, it->first)) {
      m_command_buffer_pool.push_back(it->second);
      m_fence_pool.push_back(it->first);
      it = m_command_buffers.erase(it);
      continue;
    }
    ++it;
  }
  if (m_tail == m_head) {
    return;
  }

#ifdef TRACY_ON_DEMAND
  if (!GetProfiler().IsConnected()) {
    VkCommandBuffer cb = get_command_buffer();
    VkFence fence = get_fence();
    m_context->vkBeginCommandBuffer(cb, &k_command_buffer_begin);
    m_context->vkCmdResetQueryPool(cb, m_query_pool, 0, m_query_count);
    m_context->vkEndCommandBuffer(cb);

    auto submit = k_submit_info;
    submit.pCommandBuffers = &cb;
    m_context->vkQueueSubmit(m_queue->m_queue, 1, &submit, fence);
    m_command_buffers[fence] = cb;

    m_head = m_tail = m_oldCnt = 0;
    int64_t tgpu;
    if (m_time_domain != VK_TIME_DOMAIN_DEVICE_EXT) {
      int64_t _scratch;
      calibrate(m_prev_calibration, &_scratch);
    }
    return;
  }
#endif

  int32_t count = 0;
  if (m_old_count != 0) {
    count = m_old_count;
    m_old_count = 0;
  } else {
    count = m_head < m_tail ? m_query_count - m_tail : m_head - m_tail;
  }

  if (m_context->vkGetQueryPoolResults(m_device->m_device, m_query_pool, m_tail,
          count, sizeof(int64_t) * m_queries.size(), m_queries.data(),
          sizeof(int64_t), VK_QUERY_RESULT_64_BIT) == VK_NOT_READY) {
    m_old_count = count;
    return;
  }

  for (int32_t idx = 0; idx < count; idx++) {
    auto item = tracy::Profiler::QueueSerial();
    tracy::MemWrite(&item->hdr.type, tracy::QueueType::GpuTime);
    tracy::MemWrite(&item->gpuTime.gpuTime, m_queries[idx]);
    tracy::MemWrite(&item->gpuTime.queryId, uint16_t(m_tail + idx));
    tracy::MemWrite(&item->gpuTime.context, m_tracy_context);
    tracy::Profiler::QueueSerialFinish();
  }

  if (m_time_domain != VK_TIME_DOMAIN_DEVICE_EXT) {
    int64_t cpu_time;
    int64_t gpu_time;
    calibrate(&cpu_time, &gpu_time);
    auto time = tracy::Profiler::GetTime();
    auto delta = cpu_time - m_prev_calibration;
    if (delta > 0) {
      m_prev_calibration = cpu_time;
      auto item = tracy::Profiler::QueueSerial();
      tracy::MemWrite(&item->hdr.type, tracy::QueueType::GpuCalibration);
      tracy::MemWrite(&item->gpuCalibration.gpuTime, gpu_time);
      tracy::MemWrite(&item->gpuCalibration.cpuTime, time);
      tracy::MemWrite(&item->gpuCalibration.cpuDelta, delta);
      tracy::MemWrite(&item->gpuCalibration.context, m_tracy_context);
      tracy::Profiler::QueueSerialFinish();
    }
  }

  VkCommandBuffer cb = get_command_buffer();
  VkFence fence = get_fence();
  m_context->vkBeginCommandBuffer(cb, &k_command_buffer_begin);
  m_context->vkCmdResetQueryPool(cb, m_query_pool, 0, m_query_count);
  m_context->vkEndCommandBuffer(cb);

  auto submit = k_submit_info;
  submit.pCommandBuffers = &cb;
  m_context->vkQueueSubmit(m_queue->m_queue, 1, &submit, fence);
  m_command_buffers[fence] = cb;

  m_tail += count;
  if (m_tail == m_query_count) {
    m_tail = 0;
  }
}

#endif

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn
