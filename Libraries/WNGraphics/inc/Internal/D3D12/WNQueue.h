// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_H__
#define __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNQueue.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

#include <d3d12.h>
#include <wrl.h>

namespace wn {
namespace graphics {

class fence;
class command_list;

namespace internal {
namespace d3d12 {

class d3d12_device;

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

  void enqueue_signal(fence& _fence) WN_GRAPHICS_OVERRIDE_FINAL;

  void enqueue_command_list(
      command_list* _command_list) WN_GRAPHICS_OVERRIDE_FINAL;

  WN_FORCE_INLINE bool is_valid() const WN_GRAPHICS_OVERRIDE_FINAL {
    return (m_device != nullptr);
  }

protected:
  friend class d3d12_device;

  WN_FORCE_INLINE d3d12_queue() : d3d12_queue_base(), m_device(nullptr) {}

  WN_FORCE_INLINE void initialize(d3d12_device* _device,
      Microsoft::WRL::ComPtr<ID3D12CommandQueue>&& _queue) {
    m_device = _device;
    m_queue = core::move(_queue);
  }

  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_queue;
  d3d12_device* m_device;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_H__
