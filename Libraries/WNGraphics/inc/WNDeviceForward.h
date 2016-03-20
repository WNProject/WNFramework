// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_DEVICE_FORWARD_H__
#define __WN_GRAPHICS_DEVICE_FORWARD_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNMemory/inc/WNUniquePtr.h"

namespace wn {
namespace graphics {
namespace internal {

class internal_device;

namespace d3d12 {

class d3d12_device;

}  // namespace d3d12

namespace vulkan {

class vulkan_device;

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#if _WN_GRAPHICS_DEVICE_TYPES_AVAILABLE > 1
namespace wn {
namespace graphics {

using device = internal::internal_device;

}  // namespace graphics
}  // namespace wn
#elif defined _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
namespace wn {
namespace graphics {

using device = internal::vulkan::vulkan_device;

}  // namespace graphics
}  // namespace wn
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
namespace wn {
namespace graphics {

using device = internal::d3d12::d3d12_device;

}  // namespace graphics
}  // namespace wn
#endif

namespace wn {
namespace graphics {

using device_ptr = memory::unique_ptr<device>;

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_DEVICE_FORWARD_H__
