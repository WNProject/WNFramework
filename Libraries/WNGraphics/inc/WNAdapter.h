// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef _WN_GRAPHICS_PHYSICAL_DEVICE_H__
#define _WN_GRAPHICS_PHYSICAL_DEVICE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNMemory/inc/WNUniquePtr.h"

#if _WN_GRAPHICS_DEVICE_TYPES_AVAILABLE > 1
#include "WNGraphics/inc/Internal/WNAdapter.h"
#elif defined _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/Vulkan/WNAdapter.h"
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/D3D12/WNAdapter.h"
#endif

namespace wn {
namespace graphics {

#if _WN_GRAPHICS_DEVICE_TYPES_AVAILABLE > 1
using adapter = internal::internal_adapter;
#elif defined _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
using adapter = internal::vulkan::vulkan_adapter;
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
using adapter = internal::d3d12::d3d12_adapter;
#endif

using adapter_ptr = memory::unique_ptr<adapter>;

}  // namespace graphics
}  // namespace wn

#endif  // _WN_GRAPHICS_PHYSICAL_DEVICE_H__