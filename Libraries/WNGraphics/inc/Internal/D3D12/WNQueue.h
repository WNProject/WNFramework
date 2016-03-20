// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_H__
#define __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_H__

#include "WNCore/inc/WNUtility.h"
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNQueue.h"

#include <d3d12.h>
#include <wrl.h>

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

class d3d12_queue : public internal_queue {
public:
  ~d3d12_queue();
  void enqueue_signal(fence& _fence) final;
  void enqueue_command_list(command_list* _command) final;

private:
  WN_FORCE_INLINE d3d12_queue(d3d12_device* _device,
      Microsoft::WRL::ComPtr<ID3D12CommandQueue>&& _queue)
    : internal_queue(_device), m_queue(core::move(_queue)) {}

  friend class d3d12_device;

  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_queue;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_H__
