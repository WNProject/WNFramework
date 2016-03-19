// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNCore/inc/WNUtility.h"
#include "WNGraphics/inc/Internal/D3D12/WNAdapter.h"
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNAllocator.h"

#include <D3D12.h>
#include <DXGI.h>

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

device_ptr adapter::make_device(
    memory::allocator* _allocator, WNLogging::WNLog* _log) const {
  Microsoft::WRL::ComPtr<ID3D12Device> d3d12_device;

  // This is set to D3D_FEATURE_LEVEL_11_0 because this is the lowest posible
  // d3d version d3d12 supports.  This allows us to scale up on device
  // capabilities and work on older hardware
  HRESULT hr = ::D3D12CreateDevice(m_dxgi_adapter.Get(), D3D_FEATURE_LEVEL_11_0,
      __uuidof(ID3D12Device), &d3d12_device);

  if (FAILED(hr)) {
    _log->Log(WNLogging::eError, 0, "Could not create D3D12 device, hr: ", hr);

    return nullptr;
  }

  return memory::make_unique<d3d12::device>(_allocator, _allocator, _log,
      core::move(d3d12_device));
}

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn