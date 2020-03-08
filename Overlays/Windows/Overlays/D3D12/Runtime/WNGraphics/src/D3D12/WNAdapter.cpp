// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/D3D12/WNAdapter.h"

#include "WNCore/inc/utilities.h"
#include "WNGraphics/inc/WNSurface.h"
#include "WNLogging/inc/WNLog.h"
#include "WNWindow/inc/WNWindowsWindow.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#else
#include "WNGraphics/inc/WNDevice.h"
#endif

#include <d3d12.h>

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace d3d12 {
namespace {

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using d3d12_device_constructable = d3d12_device;
#else
using d3d12_device_constructable = device;
#endif

}  // anonymous namespace

device_ptr d3d12_adapter::make_device(memory::allocator* _allocator,
    logging::log* _log, const adapter_features&) const {
  // Enabled features are unused in D3D, all features are turned on
  Microsoft::WRL::ComPtr<ID3D12Device> device;
  HRESULT hr;

  // This is set to D3D_FEATURE_LEVEL_11_0 because this is the lowest possible
  // d3d version d3d12 supports. This allows us to scale up on device
  // capabilities and work on older hardware
  hr = ::D3D12CreateDevice(
      m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), &device);

  if (FAILED(hr)) {
    _log->log_error("Could not create D3D12 device, hr: ", hr);

    return nullptr;
  }

  memory::unique_ptr<d3d12_device_constructable> ptr(
      memory::make_unique_delegated<d3d12_device_constructable>(
          _allocator, [](void* _memory) {
            return new (_memory) d3d12_device_constructable();
          }));

  if (ptr) {
    // if we fail to setup the device we clear the pointer (effectivly return
    // nullptr) due to failure
    if (!ptr->initialize(_allocator, _log, m_factory, core::move(device))) {
      ptr.reset();
    }
  }

  return core::move(ptr);
}

graphics_error d3d12_adapter::initialize_surface(
    surface* _surface, runtime::window::window* _window) {
  _surface->m_width = _window->get_width();
  _surface->m_height = _window->get_height();
  _surface->m_valid_formats.push_back(data_format::r8g8b8a8_unorm);
  _surface->m_valid_formats.push_back(data_format::b8g8r8a8_unorm);
  WN_RELEASE_ASSERT(_window->type() == runtime::window::window_type::system,
      "Unsupported: Null-windows");
  const runtime::window::native_handle* handle =
      reinterpret_cast<const runtime::window::native_handle*>(
          _window->get_native_handle());

  _surface->data_as<runtime::window::native_handle>() = *handle;
  return graphics_error::ok;
}

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn
