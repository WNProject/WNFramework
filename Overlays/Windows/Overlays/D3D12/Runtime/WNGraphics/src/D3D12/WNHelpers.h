// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_D3D12_HELPERS_H__
#define __WN_GRAPHICS_D3D12_HELPERS_H__

#include "WNContainers/inc/WNDynamicArray.h"
#include "WNGraphics/inc/WNAdapter.h"

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace runtime {
namespace graphics {
namespace internal {
namespace d3d12 {

void enumerate_adapters(memory::allocator* _allocator, logging::log* _log,
    containers::dynamic_array<adapter_ptr>& _physical_devices);

void cleanup(logging::log* _log);

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_D3D12_HELPERS_H__
