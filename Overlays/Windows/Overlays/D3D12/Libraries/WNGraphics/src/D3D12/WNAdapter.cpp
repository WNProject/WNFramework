// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNCore/inc/WNUtility.h"
#include "WNGraphics/inc/Internal/D3D12/WNAdapter.h"
#include "WNLogging/inc/WNLog.h"

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#else
#include "WNGraphics/inc/WNDevice.h"
#endif

#include <D3D12.h>
#include <DXGI.h>

namespace wn {
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
    memory::allocator* _allocator, WNLogging::WNLog* _log) const {
  Microsoft::WRL::ComPtr<ID3D12Device> device;

  // This is set to D3D_FEATURE_LEVEL_11_0 because this is the lowest possible
  // d3d version d3d12 supports.  This allows us to scale up on device
  // capabilities and work on older hardware
  const HRESULT hr = ::D3D12CreateDevice(
      m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), &device);

  if (FAILED(hr)) {
    _log->Log(WNLogging::eError, 0, "Could not create D3D12 device, hr: ", hr);

    return nullptr;
  }

  memory::unique_ptr<d3d12_device_constructable> ptr(
      memory::make_unique_delegated<d3d12_device_constructable>(
          _allocator, [](void* _memory) {
            return new (_memory) d3d12_device_constructable();
          }));

  if (ptr) {
    ptr->initialize(_allocator, _log, core::move(device));
  }

  return core::move(ptr);
}

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn
