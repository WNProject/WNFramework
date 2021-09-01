// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_PROFILER_H__
#define __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_PROFILER_H__

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/WNQueueProfiler.h"
#else
#include "core/inc/utilities.h"
#endif

#ifdef TRACY_ENABLE
#include "Tracy.hpp"
#endif

#include <initializer_list>

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace d3d12 {

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using d3d12_queue_profiler_base = queue_profiler;
#else
using d3d12_queue_profiler_base = core::non_copyable;
#endif

class d3d12_device;
class d3d12_queue;

class d3d12_queue_profiler WN_GRAPHICS_FINAL
  : public d3d12_queue_profiler_base {
public:
  ~d3d12_queue_profiler() WN_GRAPHICS_OVERRIDE_FINAL{};

  inline bool is_valid() const WN_GRAPHICS_OVERRIDE_FINAL {
    return (m_device != 0);
  }

protected:
  memory::allocator* m_allocator;

  d3d12_device* m_device;
  d3d12_queue* m_queue;
};

}  // namespace d3d12
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INC_INTERNAL_D3D12_QUEUE_PROFILER_H__
