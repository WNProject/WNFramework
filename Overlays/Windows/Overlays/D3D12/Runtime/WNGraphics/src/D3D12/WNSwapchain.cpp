// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/D3D12/WNSwapchain.h"

#include "WNGraphics/inc/Internal/D3D12/WNDataTypes.h"
#include "WNGraphics/inc/Internal/D3D12/WNQueue.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNSignal.h"
#include "WNGraphics/inc/WNSwapchain.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace d3d12 {

void d3d12_swapchain::initialize(memory::allocator* _allocator,
    d3d12_device* _device, uint32_t _width, uint32_t _height,
    const swapchain_create_info& _create_info,
    Microsoft::WRL::ComPtr<IDXGISwapChain3>&& _swapchain) {
  m_allocator = _allocator;
  m_device = _device;
  m_swapchain = core::move(_swapchain);
  m_images = containers::dynamic_array<image>(m_allocator);
  m_images.reserve(_create_info.num_buffers);
  for (size_t i = 0; i < _create_info.num_buffers; ++i) {
    m_images.push_back(image(reinterpret_cast<device*>(m_device)));
    image* im = &m_images[i];
    auto& img = get_data(im);
    img = memory::make_unique<image_data>(m_allocator);
    im->m_resource_info =
        containers::dynamic_array<image::image_buffer_resource_info>(
            m_allocator, 1);
    im->m_resource_info[0] = {_create_info.format, _width, _height, 1, 0, 0, 0};
    im->m_is_swapchain_image = true;
    m_swapchain->GetBuffer(
        static_cast<UINT>(i), __uuidof(ID3D12Resource), &img->image);
  }
}

uint32_t d3d12_swapchain::get_next_backbuffer_index(
    fence* fence, signal* _signal) const {
  if (fence) {
    fence_data& dat = get_data(fence);
    dat.fence->Signal(1);
  }
  if (_signal) {
    signal_data& dat = get_data(_signal);
    dat.fence->Signal(1);
  }
  return m_swapchain->GetCurrentBackBufferIndex();
}

image* d3d12_swapchain::get_image_for_index(uint32_t index) {
  WN_DEBUG_ASSERT(index < m_images.size(), "Invalid image index");
  return &m_images[index];
}

void d3d12_swapchain::present_internal(queue* _queue,
    const swapchain_create_info& _info, signal* _signal, uint32_t) const {
  d3d12_queue* queue = reinterpret_cast<d3d12_queue*>(_queue);
  if (_signal) {
    signal_data& dat = get_data(_signal);
    queue->m_queue->Wait(dat.fence.Get(), 1);
    queue->m_queue->Signal(dat.fence.Get(), 0);
  }
  uint32_t interval =
      (_info.mode == swap_mode::fifo || _info.mode == swap_mode::mailbox) ? 1
                                                                          : 0;
  HRESULT hr = m_swapchain->Present(interval, 0);
  (void)hr;
}

template <typename T>
typename data_type<T>::value& d3d12_swapchain::get_data(T* t) const {
  return t->data_as<typename data_type<T>::value>();
}

template <typename T>
typename data_type<const T>::value& d3d12_swapchain::get_data(
    const T* const t) const {
  return t->data_as<typename data_type<const T>::value>();
}

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn
