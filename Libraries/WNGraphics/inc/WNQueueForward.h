// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once
#ifndef __WN_GRAPHICS_QUEUE_FORWARD_H__
#define __WN_GRAPHICS_QUEUE_FORWARD_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNMemory/inc/WNUniquePtr.h"

namespace wn {
namespace graphics {
namespace internal {

class queue;

namespace d3d12 {

class queue;

}  // namespace d3d12

namespace vulkan {

class queue;

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#if _WN_GRAPHICS_DEVICE_TYPES_AVAILABLE > 1
namespace wn {
namespace graphics {

using queue = wn::graphics::internal::queue;

}  // namespace graphics
}  // namespace wn
#elif defined _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
namespace wn {
namespace graphics {

using queue = internal::vulkan::queue;

}  // namespace graphics
}  // namespace wn
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
namespace wn {
namespace graphics {

using queue = internal::d3d12::queue;

}  // namespace graphics
}  // namespace wn
#endif

namespace wn {
namespace graphics {

using queue_ptr = memory::unique_ptr<queue>;

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_QUEUE_FORWARD_H__
