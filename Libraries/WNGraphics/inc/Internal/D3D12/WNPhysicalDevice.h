// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_PHYSICAL_DEVICE_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_PHYSICAL_DEVICE_H__

#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#include "WNGraphics/inc/Internal/WNPhysicalDevice.h"

#include <DXGI.h>
#include <wrl.h>

namespace wn {
namespace graphics {

class factory;

namespace internal {
namespace d3d12 {

class physical_device : public internal::physical_device {
public:
  WN_FORCE_INLINE physical_device(
      Microsoft::WRL::ComPtr<IDXGIAdapter1>&& _adapter,
      containers::string&& _name, const uint32_t _vendor_id,
      const uint32_t _device_id)
    : internal::physical_device(
          std::move(_name), _vendor_id, _device_id, api_type::d3d12),
      m_adapter(_adapter) {}

  virtual ~physical_device() override = default;

  WN_FORCE_INLINE virtual device_ptr make_device(
      memory::allocator*, WNLogging::WNLog*) const override {
    return nullptr;
  }

private:
  Microsoft::WRL::ComPtr<IDXGIAdapter1> m_adapter;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_PHYSICAL_DEVICE_H__