// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_DEVICE_H__
#define __WN_GRAPHICS_DEVICE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNMemory/inc/WNUniquePtr.h"

#if _WN_GRAPHICS_DEVICE_TYPES_AVAILABLE > 1
#include "WNGraphics/inc/Internal/WNDevice.h"
#elif defined _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#endif

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

class device;

}  // namespace d3d12

namespace vulkan {

class device;

}  // namespace vulkan
}  // namespace internal

#if _WN_GRAPHICS_DEVICE_TYPES_AVAILABLE > 1
using device = internal::device;
#elif defined _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
using device = internal::vulkan::device;
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
using device = internal::d3d12::device;
#endif

using device_ptr = memory::unique_ptr<device>;

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_DEVICE_H__