// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef _WN_GRAPHICS_PHYSICAL_DEVICE_H__
#define _WN_GRAPHICS_PHYSICAL_DEVICE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNMemory/inc/WNUniquePtr.h"

#if _WN_GRAPHICS_DEVICE_TYPES_AVAILABLE > 1
#include "WNGraphics/inc/Internal/WNPhysicalDevice.h"
#elif defined _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/Vulkan/WNPhysicalDevice.h"
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/D3D12/WNPhysicalDevice.h"
#endif

namespace wn {
namespace graphics {

#if _WN_GRAPHICS_DEVICE_TYPES_AVAILABLE > 1
using physical_device = internal::physical_device;
#elif defined _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
using physical_device = internal::vulkan::physical_device;
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
using physical_device = internal::d3d12::physical_device;
#endif

using physical_device_ptr = memory::unique_ptr<physical_device>;

}  // namespace graphics
}  // namespace wn

#endif  // _WN_GRAPHICS_PHYSICAL_DEVICE_H__