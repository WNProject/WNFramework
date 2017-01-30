// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_PHYSICAL_DEVICE_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_PHYSICAL_DEVICE_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNMemory/inc/WNUniquePtr.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNAdapter.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

#include <DXGI.h>
#include <wrl.h>

namespace WNLogging {

class WNLog;

}  // namespace WNLogging

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace graphics {

class factory;
class adapter;
class device;

using adapter_ptr = memory::unique_ptr<adapter>;
using device_ptr = memory::unique_ptr<device>;

namespace internal {
namespace d3d12 {

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using d3d12_adapter_base = adapter;
#else
using d3d12_adapter_base = core::non_copyable;
#endif

class d3d12_adapter WN_GRAPHICS_FINAL : public d3d12_adapter_base {
public:
#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  enum class api_type{invalid, vulkan, d3d12, max};
#endif

  ~d3d12_adapter() WN_GRAPHICS_OVERRIDE_FINAL = default;

  device_ptr make_device(memory::allocator* _allocator,
      WNLogging::WNLog* _log) const WN_GRAPHICS_OVERRIDE_FINAL;

  WN_FORCE_INLINE containers::string_view name() const
      WN_GRAPHICS_OVERRIDE_FINAL {
    return m_name;
  }

  WN_FORCE_INLINE uint32_t vendor_id() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_vendor_id;
  }

  WN_FORCE_INLINE uint32_t device_id() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_device_id;
  }

  WN_FORCE_INLINE api_type api() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_api;
  }

protected:
  friend void enumerate_adapters(memory::allocator*, WNLogging::WNLog*,
      containers::dynamic_array<adapter_ptr>&);

  WN_FORCE_INLINE d3d12_adapter()
    : d3d12_adapter_base(),
      m_vendor_id(uint32_t(-1)),
      m_device_id(uint32_t(-1)),
      m_api(api_type::invalid) {}

  WN_FORCE_INLINE void initialize(
      Microsoft::WRL::ComPtr<IDXGIAdapter1>&& _dxgi_adapter,
      containers::string&& _name, const uint32_t _vendor_id,
      const uint32_t _device_id) {
    m_adapter = core::move(_dxgi_adapter);
    m_name = std::move(_name);
    m_vendor_id = _vendor_id;
    m_device_id = _device_id;
    m_api = api_type::d3d12;
  }

  Microsoft::WRL::ComPtr<IDXGIAdapter1> m_adapter;
  containers::string m_name;
  api_type m_api;
  uint32_t m_vendor_id;
  uint32_t m_device_id;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_PHYSICAL_DEVICE_H__
