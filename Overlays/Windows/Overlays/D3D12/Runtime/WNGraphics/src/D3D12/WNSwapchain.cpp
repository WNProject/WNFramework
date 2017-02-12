// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/D3D12/WNSwapchain.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNSwapchain.h"

namespace wn {
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
    image& im = m_images[i];
    Microsoft::WRL::ComPtr<ID3D12Resource>& resource =
        im.data_as<Microsoft::WRL::ComPtr<ID3D12Resource>>();
    im.m_resource_info = {_create_info.format, _width, _height, 1, 0, 0, 0};
    im.m_is_swapchain_image = true;
    m_swapchain->GetBuffer(
        static_cast<UINT>(i), __uuidof(ID3D12Resource), &resource);
  }
}

uint32_t d3d12_swapchain::get_backbuffer_index() const {
  return m_swapchain->GetCurrentBackBufferIndex();
}

image* d3d12_swapchain::get_image_for_index(uint32_t index) {
  WN_DEBUG_ASSERT_DESC(index < m_images.size(), "Invalid image index");
  return &m_images[index];
}

void d3d12_swapchain::present_internal(
    queue*, const swapchain_create_info& _info, uint32_t) const {
  uint32_t interval =
      (_info.mode == swap_mode::fifo || _info.mode == swap_mode::mailbox) ? 1
                                                                          : 0;
  HRESULT hr = m_swapchain->Present(interval, 0);
  (void)hr;
}

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn
