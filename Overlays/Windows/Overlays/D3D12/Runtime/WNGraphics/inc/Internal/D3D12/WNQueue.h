// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_H__
#define __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNMemory/inc/allocator.h"
#include "core/inc/pair.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNQueue.h"
#else
#include "core/inc/utilities.h"
#endif

#include <d3d12.h>
#include <wrl.h>
#include <initializer_list>

namespace wn {
namespace runtime {
namespace graphics {

class fence;
class command_list;
class signal;

namespace internal {
namespace d3d12 {

template <typename T>
struct data_type;
class d3d12_device;
class d3d12_swapchain;

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using d3d12_queue_base = queue;
#else
using d3d12_queue_base = core::non_copyable;
#endif

class d3d12_queue WN_GRAPHICS_FINAL : public d3d12_queue_base {
public:
#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  ~d3d12_queue();
#else
  ~d3d12_queue() WN_GRAPHICS_OVERRIDE_FINAL = default;
#endif

  void enqueue_fence(fence& _fence) WN_GRAPHICS_OVERRIDE_FINAL;

  void enqueue_command_list(
      command_list* _command_list) WN_GRAPHICS_OVERRIDE_FINAL;

  inline bool is_valid() const WN_GRAPHICS_OVERRIDE_FINAL {
    return (m_device != nullptr);
  }

  void enqueue_commands(
      containers::contiguous_range<command_list*> _command_lists,
      containers::contiguous_range<core::pair<pipeline_stages, signal*>>
          _wait_signals,
      fence* _signal_fence,
      containers::contiguous_range<signal*> _signal_signals)
      WN_GRAPHICS_OVERRIDE_FINAL;

  void enqueue_command_lists(
      std::initializer_list<command_list*> _command_lists,
      std::initializer_list<core::pair<pipeline_stages, signal*>> _wait_signals,
      fence* _signal_fence, std::initializer_list<signal*> _signal_signals)
      WN_GRAPHICS_OVERRIDE_FINAL;

protected:
  friend class d3d12_device;
  friend class d3d12_swapchain;
  template <typename T>
  typename data_type<T>::value& get_data(T* t);

  template <typename T>
  typename data_type<const T>::value& get_data(const T* const t);

  inline d3d12_queue() : d3d12_queue_base(), m_device(nullptr) {}

  inline void initialize(d3d12_device* _device, memory::allocator* _allocator,
      Microsoft::WRL::ComPtr<ID3D12CommandQueue>&& _queue) {
    m_device = _device;
    m_allocator = _allocator;
    m_queue = core::move(_queue);
  }

  template <typename command_lists, typename wait_signals,
      typename signal_signals>
  void enqueue_command_lists_internal(command_lists& _command_lists,
      wait_signals& _wait_signals, fence* _signal_fence,
      signal_signals& _signal_signals);

  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_queue;
  d3d12_device* m_device;
  memory::allocator* m_allocator;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_H__
