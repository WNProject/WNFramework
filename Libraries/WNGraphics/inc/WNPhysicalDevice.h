// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef _WN_GRAPHICS_PHYSICAL_DEVICE_H__
#define _WN_GRAPHICS_PHYSICAL_DEVICE_H__

#include "WNCore/inc/WNBase.h"

#if defined _ONLY_VULKAN || defined _WN_ANDROID
#include "WNGraphics/inc/Internal/Vulkan/WNPhysicalDevice.h"
#elif defined _ONLY_D3D12
#include "WNGraphics/inc/Internal/D3D12/WNPhysicalDevice.h"
#else
#include "WNGraphics/inc/Internal/WNPhysicalDevice.h"
#endif

namespace wn {
namespace graphics {

#if defined _ONLY_VULKAN || defined _WN_ANDROID
using physical_device = internal::vulkan::physical_device;
#elif defined _ONLY_D3D12
using physical_device = internal::d3d12::physical_device;
#else
using physical_device = internal::physical_device;
#endif

using physical_device_ptr = memory::unique_ptr<physical_device>;

}  // namespace graphics
}  // namespace wn

#endif  // _WN_GRAPHICS_PHYSICAL_DEVICE_H__