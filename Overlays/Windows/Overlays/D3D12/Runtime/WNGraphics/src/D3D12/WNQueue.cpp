// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/D3D12/WNQueue.h"

#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
#include "WNGraphics/inc/Internal/D3D12/WNDataTypes.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNSignal.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#else
#include "WNGraphics/inc/WNDevice.h"
#endif

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace d3d12 {

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
d3d12_queue::~d3d12_queue() {
  if (is_valid()) {
    m_device->destroy_queue(this);
  }
}
#endif

void d3d12_queue::enqueue_fence(fence& _fence) {
  fence_data& data = get_data(&_fence);

  m_queue->Signal(data.fence.Get(), 1);
}

void d3d12_queue::enqueue_command_list(command_list* _command) {
  d3d12_command_list* command_list =
      reinterpret_cast<d3d12_command_list*>(_command);
  ID3D12CommandList* list = command_list->command_list();

  m_queue->ExecuteCommandLists(1, &list);
}

void d3d12_queue::enqueue_commands(
    containers::contiguous_range<command_list*> _command_lists,
    containers::contiguous_range<core::pair<pipeline_stages, signal*>>
        _wait_signals,
    fence* _signal_fence,
    containers::contiguous_range<signal*> _signal_signals) {
  enqueue_command_lists_internal(
      _command_lists, _wait_signals, _signal_fence, _signal_signals);
}

void d3d12_queue::enqueue_command_lists(
    std::initializer_list<command_list*> _command_lists,
    std::initializer_list<core::pair<pipeline_stages, signal*>> _wait_signals,
    fence* _signal_fence, std::initializer_list<signal*> _signal_signals) {
  enqueue_command_lists_internal(
      _command_lists, _wait_signals, _signal_fence, _signal_signals);
}

template <typename command_lists, typename wait_signals,
    typename signal_signals>
void d3d12_queue::enqueue_command_lists_internal(command_lists& _command_lists,
    wait_signals& _wait_signals, fence* _signal_fence,
    signal_signals& _signal_signals) {
  containers::dynamic_array<ID3D12CommandList*> command_lists(m_allocator);
  command_lists.reserve(_command_lists.size());

  for (auto& list : _command_lists) {
    d3d12_command_list* command_list =
        reinterpret_cast<d3d12_command_list*>(list);
    command_lists.push_back(command_list->command_list());
  }

  for (auto& wait_signal : _wait_signals) {
    signal_data& data = get_data(wait_signal.second);
    m_queue->Wait(data.fence.Get(), 1);
    m_queue->Signal(data.fence.Get(), 0);
  }

  m_queue->ExecuteCommandLists(
      static_cast<UINT>(command_lists.size()), command_lists.data());

  for (auto& signal_signal : _signal_signals) {
    signal_data& data = get_data(signal_signal);
    m_queue->Signal(data.fence.Get(), 1);
  }

  if (_signal_fence) {
    fence_data& data = get_data(_signal_fence);
    m_queue->Signal(data.fence.Get(), 1);
  }
}

template <typename T>
typename data_type<T>::value& d3d12_queue::get_data(T* t) {
  return t->data_as<typename data_type<T>::value>();
}

template <typename T>
typename data_type<const T>::value& d3d12_queue::get_data(const T* const t) {
  return t->data_as<typename data_type<const T>::value>();
}

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn
