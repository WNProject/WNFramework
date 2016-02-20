// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_DEVICE_H__
#define __WN_GRAPHICS_DEVICE_H__

#include "WNMemory/inc/WNUniquePtr.h"

#if defined _ONLY_VULKAN || defined _WN_ANDROID
#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#elif defined _ONLY_D3D12
#include "WNGraphics/inc/Internal/D3D12/WNDevice.h"
#else
#include "WNGraphics/inc/Internal/WNDevice.h"
#endif

namespace wn {
namespace graphics {

#if defined _ONLY_VULKAN || defined _WN_ANDROID
using device = internal::vulkan::device;
#elif defined _ONLY_D3D12
using device = internal::d3d12::device;
#else
using device = internal::device;
#endif

using device_ptr = memory::unique_ptr<device>;

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_DEVICE_H__