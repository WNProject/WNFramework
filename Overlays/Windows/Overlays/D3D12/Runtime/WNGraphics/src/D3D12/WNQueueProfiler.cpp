// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Inspired  by "TracyVulkan.hpp"

#include "WNGraphics/inc/Internal/D3D12/WNQueueProfiler.h"
#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#include "WNGraphics/inc/Internal/D3D12/WNQueue.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace d3d12 {

const uint32_t k_query_count = (64 * 1024);

d3d12_queue_profiler::~d3d12_queue_profiler() {
#ifdef TRACY_ENABLE
#endif

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  if (is_valid()) {
    m_device->destroy_queue_profiler(this);
  }
#endif
}

void d3d12_queue_profiler::initialize(memory::allocator* _allocator,
    containers::string_view _name, d3d12_device* _device, d3d12_queue* _queue) {
  (void)_allocator;
  (void)_name;
  (void)_device;
  (void)_queue;
#ifdef TRACY_ENABLE
  m_allocator = _allocator;
  m_query_limit = k_query_count;
  m_frame_queries = containers::deque<frame_query>(_allocator);
  m_qpc_to_ns = int64_t{1000000000 / tracy::GetFrequencyQpc()};
  m_queue = _queue;
  m_tracy_context =
      tracy::GetGpuCtxCounter().fetch_add(1, std::memory_order_relaxed);
  if (_queue->m_queue->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_COPY) {
    D3D12_FEATURE_DATA_D3D12_OPTIONS3 featureData{};

    if (FAILED(_device->m_device->CheckFeatureSupport(
            D3D12_FEATURE_D3D12_OPTIONS3, &featureData, sizeof(featureData)))) {
      return;
    }
  }

  uint64_t timestamp_frequency;
  WN_RELEASE_ASSERT(
      S_OK == _queue->m_queue->GetTimestampFrequency(&timestamp_frequency),
      "Failed to get timestamp frequency.");

  uint64_t cpu_timestamp;
  uint64_t gpu_timestamp;
  if (FAILED(_queue->m_queue->GetClockCalibration(
          &gpu_timestamp, &cpu_timestamp))) {
    assert(false && "Failed to get queue clock calibration.");
  }
  m_prev_calibration = cpu_timestamp * m_qpc_to_ns;
  cpu_timestamp = tracy::Profiler::GetTime();

  D3D12_QUERY_HEAP_DESC heap_desc{};
  heap_desc.Type =
      _queue->m_queue->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_COPY
          ? D3D12_QUERY_HEAP_TYPE_COPY_QUEUE_TIMESTAMP
          : D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
  heap_desc.Count = m_query_limit;
  heap_desc.NodeMask = 0;

  while (S_OK != _device->m_device->CreateQueryHeap(
                     &heap_desc, IID_PPV_ARGS(&m_query_heap))) {
    m_query_limit /= 2;
    heap_desc.Count = m_query_limit;
  }

  D3D12_RESOURCE_DESC readback_buffer_desc{};
  readback_buffer_desc.Alignment = 0;
  readback_buffer_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
  readback_buffer_desc.Width = m_query_limit * sizeof(uint64_t);
  readback_buffer_desc.Height = 1;
  readback_buffer_desc.DepthOrArraySize = 1;
  readback_buffer_desc.Format = DXGI_FORMAT_UNKNOWN;
  readback_buffer_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
  readback_buffer_desc.MipLevels = 1;
  readback_buffer_desc.SampleDesc.Count = 1;
  readback_buffer_desc.SampleDesc.Quality = 0;
  readback_buffer_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

  D3D12_HEAP_PROPERTIES readback_heap_props{};
  readback_heap_props.Type = D3D12_HEAP_TYPE_READBACK;
  readback_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
  readback_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
  readback_heap_props.CreationNodeMask = 0;
  readback_heap_props.VisibleNodeMask = 0;

  WN_RELEASE_ASSERT(
      S_OK == _device->m_device->CreateCommittedResource(&readback_heap_props,
                  D3D12_HEAP_FLAG_NONE, &readback_buffer_desc,
                  D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
                  IID_PPV_ARGS(&m_readback_buffer)),
      "Failed to create query readback buffer.");

  WN_RELEASE_ASSERT(
      S_OK == _device->m_device->CreateFence(
                  0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_payload_fence)),
      "Failed to create payload fence.");

  {
    auto* item = tracy::Profiler::QueueSerial();
    tracy::MemWrite(&item->hdr.type, tracy::QueueType::GpuNewContext);
    tracy::MemWrite(&item->gpuNewContext.cpuTime, cpu_timestamp);
    tracy::MemWrite(&item->gpuNewContext.gpuTime, gpu_timestamp);
    memory::memset(
        &item->gpuNewContext.thread, 0, sizeof(item->gpuNewContext.thread));
    tracy::MemWrite(&item->gpuNewContext.period,
        1E+09f / static_cast<float>(timestamp_frequency));
    tracy::MemWrite(&item->gpuNewContext.context, m_tracy_context);
    tracy::MemWrite(&item->gpuNewContext.flags, tracy::GpuContextCalibration);
    tracy::MemWrite(
        &item->gpuNewContext.type, tracy::GpuContextType::Direct3D12);

#ifdef TRACY_ON_DEMAND
    tracy::GetProfiler().DeferItem(*item);
#endif

    tracy::Profiler::QueueSerialFinish();
  }

  auto ptr = (char*)tracy::tracy_malloc(_name.size());
  memcpy(ptr, _name.data(), _name.size());
  {
    auto item = tracy::Profiler::QueueSerial();
    MemWrite(&item->hdr.type, tracy::QueueType::GpuContextName);
    tracy::MemWrite(&item->gpuContextNameFat.context, m_tracy_context);
    tracy::MemWrite(&item->gpuContextNameFat.ptr, (uint64_t)ptr);
    tracy::MemWrite(
        &item->gpuContextNameFat.size, static_cast<uint16_t>(_name.size()));
#ifdef TRACY_ON_DEMAND
    tracy::GetProfiler().DeferItem(*item);
#endif
    tracy::Profiler::QueueSerialFinish();
  }
  m_initialized = true;
#endif  // TRACY_ENABLE
}

uint32_t d3d12_queue_profiler::next_query_id() {
#ifdef TRACY_ENABLE
  WN_RELEASE_ASSERT(m_query_counter < m_query_limit,
      "Submitted too many GPU queries! Consider increasing MaxQueries.");
  const uint32_t id =
      (m_previous_query_counter + m_query_counter) % m_query_limit;
  m_query_counter += 2;
  return id;
#else
  return 0;
#endif
}

#ifdef TRACY_ENABLE

struct d3d12_gpu_profiler_scope : public gpu_profile_scope {
  ~d3d12_gpu_profiler_scope() {
    if (!m_active) {
      return;
    }
    m_command_list->EndQuery(m_profiler->m_query_heap.Get(),
        D3D12_QUERY_TYPE_TIMESTAMP, m_query_id + 1);

    auto item = tracy::Profiler::QueueSerial();
    tracy::MemWrite(&item->hdr.type, tracy::QueueType::GpuZoneEndSerial);
    tracy::MemWrite(&item->gpuZoneEnd.cpuTime, tracy::Profiler::GetTime());
    tracy::MemWrite(&item->gpuZoneEnd.thread, tracy::GetThreadHandle());
    tracy::MemWrite(
        &item->gpuZoneEnd.queryId, static_cast<uint16_t>(m_query_id + 1));
    tracy::MemWrite(&item->gpuZoneEnd.context, m_profiler->m_tracy_context);
    tracy::Profiler::QueueSerialFinish();

    m_command_list->ResolveQueryData(m_profiler->m_query_heap.Get(),
        D3D12_QUERY_TYPE_TIMESTAMP, m_query_id, 2,
        m_profiler->m_readback_buffer.Get(), m_query_id * sizeof(uint64_t));
  }

  bool m_active;
  d3d12_queue_profiler* m_profiler = nullptr;
  ID3D12GraphicsCommandList* m_command_list = nullptr;
  uint32_t m_query_id = 0;
};

memory::unique_ptr<gpu_profile_scope> d3d12_queue_profiler::profile_scope(
    command_list* _command_list, const tracy::SourceLocationData* loc) {
  static int x = 0;
  auto gps = memory::make_unique<d3d12_gpu_profiler_scope>(m_allocator);
#ifdef TRACY_ON_DEMAND
  gps->m_active = tracy::GetProfiler()::IsConnected();
#else
  gps->m_active = true;
#endif
  gps->m_profiler = this;

  auto cl = reinterpret_cast<d3d12_command_list*>(_command_list);

  gps->m_command_list = cl->m_command_list.Get();
  uint32_t id = next_query_id();
  gps->m_query_id = id;

  cl->m_command_list->EndQuery(
      m_query_heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, id);

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

void d3d12_queue_profiler::new_frame() {
  m_frame_queries.emplace_back(
      frame_query{m_previous_query_counter, m_query_counter});
  m_previous_query_counter += m_query_counter;
  m_query_counter = 0;

  if (m_previous_query_counter >= m_query_limit) {
    m_previous_query_counter -= m_query_limit;
  }

  m_queue->m_queue->Signal(m_payload_fence.Get(), ++m_active_payload);
}

void d3d12_queue_profiler::collect() {
#ifdef TRACY_ON_DEMAND
  if (!tracy::GetProfiler().IsConnected()) {
    m_query_counter = 0;
    return;
  }
#endif
  const auto newest_ready_payload = m_payload_fence->GetCompletedValue();
  const auto payload_count =
      m_frame_queries.size() - (m_active_payload - newest_ready_payload);
  if (0 == payload_count) {
    return;
  }
  D3D12_RANGE map_range{0, m_query_limit * sizeof(uint64_t)};

  void* mapping = nullptr;
  WN_RELEASE_ASSERT(S_OK == m_readback_buffer->Map(0, &map_range, &mapping),

      "Failed to map readback buffer.");

  auto* timestamp_data = static_cast<uint64_t*>(mapping);
  for (uint32_t i = 0; i < payload_count; ++i) {
    const auto& payload = m_frame_queries.front();

    for (uint32_t j = 0; j < payload.query_count; ++j) {
      const auto counter = (payload.query_id_start + j) % m_query_limit;
      const auto timestamp = timestamp_data[counter];
      const auto query_id = counter;

      auto* item = tracy::Profiler::QueueSerial();
      tracy::MemWrite(&item->hdr.type, tracy::QueueType::GpuTime);
      tracy::MemWrite(&item->gpuTime.gpuTime, timestamp);
      tracy::MemWrite(&item->gpuTime.queryId, static_cast<uint16_t>(query_id));
      tracy::MemWrite(&item->gpuTime.context, m_tracy_context);
      tracy::Profiler::QueueSerialFinish();
    }
    m_frame_queries.pop_front();
  }
  m_readback_buffer->Unmap(0, nullptr);

  uint64_t cpu_timestamp;
  uint64_t gpu_timestamp;

  WN_RELEASE_ASSERT(S_OK == m_queue->m_queue->GetClockCalibration(
                                &gpu_timestamp, &cpu_timestamp),
      "Failed to get queue clock calibration.");

  cpu_timestamp *= m_qpc_to_ns;

  const auto cpu_delta = cpu_timestamp - m_prev_calibration;
  if (cpu_delta > 0) {
    m_prev_calibration = cpu_timestamp;
    cpu_timestamp = tracy::Profiler::GetTime();

    auto* item = tracy::Profiler::QueueSerial();
    tracy::MemWrite(&item->hdr.type, tracy::QueueType::GpuCalibration);
    tracy::MemWrite(&item->gpuCalibration.gpuTime, gpu_timestamp);
    tracy::MemWrite(&item->gpuCalibration.cpuTime, cpu_timestamp);
    tracy::MemWrite(&item->gpuCalibration.cpuDelta, cpu_delta);
    tracy::MemWrite(&item->gpuCalibration.context, m_tracy_context);

    tracy::Profiler::QueueSerialFinish();
  }
}

#endif

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn
