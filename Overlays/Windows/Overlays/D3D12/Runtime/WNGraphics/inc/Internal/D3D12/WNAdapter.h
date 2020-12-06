// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_ADAPTER_H__
#define __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_ADAPTER_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNAdapterFeatures.h"
#include "WNGraphics/inc/WNErrors.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNWindow/inc/WNWindow.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNAdapter.h"
#else
#include "core/inc/utilities.h"
#endif

#include <DXGI1_4.h>
#include <wrl.h>

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace runtime {
namespace graphics {

class factory;
class adapter;
class device;
class surface;
struct surface_capabilities;

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

  device_ptr make_device(memory::allocator* _allocator, logging::log* _log,
      const adapter_features&) const WN_GRAPHICS_OVERRIDE_FINAL;

  graphics_error initialize_surface(surface* _surface,
      runtime::window::window* window) WN_GRAPHICS_OVERRIDE_FINAL;

  void destroy_surface(surface*) WN_GRAPHICS_OVERRIDE_FINAL {}

  graphics_error get_surface_capabilities(surface* _surface,
      surface_capabilities* _capabilities) WN_GRAPHICS_OVERRIDE_FINAL;

  const adapter_features& get_features() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_adapter_features;
  }

  const adapter_formats& get_formats() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_adapter_formats;
  }

  inline containers::string_view name() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_name;
  }

  inline uint32_t vendor_id() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_vendor_id;
  }

  inline uint32_t device_id() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_device_id;
  }

  inline api_type api() const WN_GRAPHICS_OVERRIDE_FINAL {
    return m_api;
  }

protected:
  friend void enumerate_adapters(memory::allocator*, logging::log*,
      containers::dynamic_array<adapter_ptr>&);

  inline d3d12_adapter()
    : d3d12_adapter_base(),
      m_vendor_id(uint32_t(-1)),
      m_device_id(uint32_t(-1)),
      m_api(api_type::invalid) {}

  inline void initialize(Microsoft::WRL::ComPtr<IDXGIAdapter1>&& _dxgi_adapter,
      Microsoft::WRL::ComPtr<IDXGIFactory4> _dxgi_factory,
      containers::string&& _name, logging::log* _log, const uint32_t _vendor_id,
      const uint32_t _device_id) {
    m_adapter = core::move(_dxgi_adapter);
    m_name = std::move(_name);
    m_vendor_id = _vendor_id;
    m_device_id = _device_id;
    m_factory = _dxgi_factory;
    m_log = _log;
    m_api = api_type::d3d12;
    m_adapter_features.bc_textures = true;
    m_adapter_features.geometry = true;
    m_adapter_features.tessellation = true;

    m_adapter_formats.has_d16_unorm = true;
    m_adapter_formats.has_d32_float = true;
    m_adapter_formats.has_d24_unorm_s8_unorm = true;
    m_adapter_formats.has_d32_float_s8_unorm = true;
  }

  Microsoft::WRL::ComPtr<IDXGIAdapter1> m_adapter;
  Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory;
  adapter_features m_adapter_features;
  adapter_formats m_adapter_formats;
  containers::string m_name;
  logging::log* m_log;
  api_type m_api;
  uint32_t m_vendor_id;
  uint32_t m_device_id;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_GRAPHICS_INTERNAL_D3D12_ADAPTER_H__
