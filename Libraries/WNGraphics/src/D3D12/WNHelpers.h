// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_D3D12_HELPERS_H__
#define __WN_GRAPHICS_D3D12_HELPERS_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNGraphics/inc/WNPhysicalDevice.h"

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

void enumerate_physical_devices(memory::allocator* _allocator,
    WNLogging::WNLog* _log,
    containers::dynamic_array<physical_device_ptr>& _physical_devices);

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_D3D12_HELPERS_H__