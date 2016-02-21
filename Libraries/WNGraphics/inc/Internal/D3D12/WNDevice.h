// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_DEVICE_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_DEVICE_H__

#include "WNCore/inc/WNUtility.h"
#include "WNGraphics/inc/Internal/WNDevice.h"

#include <D3D12.h>
#include <wrl.h>

namespace WNLogging {

class WNLog;

}  // namespace WNLogging

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace graphics {
namespace internal {
namespace d3d12 {

class device : public internal::device {
public:
  WN_FORCE_INLINE device(memory::allocator* _allocator, WNLogging::WNLog* _log,
      Microsoft::WRL::ComPtr<ID3D12Device>&& _d3d12_device,
      Microsoft::WRL::ComPtr<ID3D12CommandQueue>&& _d3d12_command_queue)
    : internal::device(_allocator, _log),
      m_d3d12_device(core::move(_d3d12_device)),
      m_d3d12_command_queue(core::move(_d3d12_command_queue)) {}

private:
  Microsoft::WRL::ComPtr<ID3D12Device> m_d3d12_device;
  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3d12_command_queue;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_DEVICE_H__