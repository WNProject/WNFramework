// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/D3D12/WNAdapter.h"
#include "WNCore/inc/WNUtility.h"
#include "WNLogging/inc/WNLog.h"

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

device_ptr d3d12_adapter::make_device(
    memory::allocator* _allocator, logging::log* _log) const {
  Microsoft::WRL::ComPtr<ID3D12Device> device;
  HRESULT hr;

#ifdef _WN_GRAPHICS_ALLOW_DEBUG_MODE
  // Enable debug layer

  Microsoft::WRL::ComPtr<ID3D12Debug> debug_controller;
  hr = ::D3D12GetDebugInterface(__uuidof(ID3D12Debug), &debug_controller);

  if (FAILED(hr)) {
    _log->log_warning("Could not enable D3D12 debug layer, hr: ", hr);
  } else {
    debug_controller->EnableDebugLayer();
  }
#endif

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

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn
