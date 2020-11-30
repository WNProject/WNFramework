// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_SRC_VULKAN_VULKAN_HELPERS_H__
#define __WN_GRAPHICS_SRC_VULKAN_VULKAN_HELPERS_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNGraphics/inc/WNAdapter.h"
#include "WNLogging/inc/WNLog.h"

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {

void enumerate_adapters(memory::allocator* _allocator, logging::log* _log,
    containers::dynamic_array<adapter_ptr>& _arr);

inline void cleanup(logging::log*) {}

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_ADAPTER_H__
