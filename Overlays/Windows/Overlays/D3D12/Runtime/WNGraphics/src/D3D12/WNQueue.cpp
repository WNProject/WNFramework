// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/D3D12/WNQueue.h"
#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
#include "WNGraphics/inc/Internal/D3D12/WNFenceData.h"
#include "WNGraphics/inc/WNFence.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#else
#include "WNGraphics/inc/WNDevice.h"
#endif

namespace wn {
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
  fence_data& data = _fence.data_as<fence_data>();

  m_queue->Signal(data.fence.Get(), 1);
}

void d3d12_queue::enqueue_command_list(command_list* _command) {
  d3d12_command_list* command_list =
      reinterpret_cast<d3d12_command_list*>(_command);
  ID3D12CommandList* list = command_list->command_list();

  m_queue->ExecuteCommandLists(1, &list);
}

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn
