// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/D3D12/WNQueue.h"
#include "WNGraphics/inc/Internal/D3D12/WNCommandList.h"
#include "WNGraphics/inc/Internal/D3D12/WNDataTypes.h"
#include "WNGraphics/inc/WNFence.h"

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
