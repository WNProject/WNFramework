// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INC_INTERNAL_D3D12_SWAPCHIAN_H__
#define __WN_GRAPHICS_INC_INTERNAL_D3D12_SWAPCHIAN_H__

#include "WNGraphics/inc/Internal/WNConfig.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNSwapchain.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNGraphics/inc/Internal/D3D12/WNDataTypes.h"
#include "WNGraphics/inc/WNImage.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>

namespace wn {
namespace graphics {
struct swapchain_create_info;
namespace internal {
namespace d3d12 {

class d3d12_device;

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using d3d12_swapchain_base = swapchain;
#else
using d3d12_swapchain_base = core::non_copyable;
#endif

class d3d12_swapchain WN_GRAPHICS_FINAL : public d3d12_swapchain_base {
public:
#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  ~d3d12_swapchain() = default;
#else
  ~d3d12_swapchain() WN_GRAPHICS_OVERRIDE_FINAL = default;
#endif

  image* get_image_for_index(uint32_t index) WN_GRAPHICS_OVERRIDE_FINAL;
  uint32_t get_backbuffer_index() const WN_GRAPHICS_OVERRIDE_FINAL;

protected:
  friend class d3d12_device;

  template <typename T>
  typename data_type<T>::value& get_data(T* t);

  template <typename T>
  typename data_type<const T>::value& get_data(const T* const t);

  WN_FORCE_INLINE d3d12_swapchain()
    : d3d12_swapchain_base(), m_device(nullptr) {}

  void initialize(memory::allocator* _allocator, d3d12_device* _device,
      uint32_t _width, uint32_t _height,
      const swapchain_create_info& create_info,
      Microsoft::WRL::ComPtr<IDXGISwapChain3>&& _swapchain);

  void present_internal(queue* q, const swapchain_create_info& info,
      uint32_t image_index) const WN_GRAPHICS_OVERRIDE_FINAL;

  Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapchain;
  d3d12_device* m_device;
  memory::allocator* m_allocator;
  containers::dynamic_array<image> m_images;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_H__
