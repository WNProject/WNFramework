// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_QUEUE_PROFILER_H__
#define __WN_GRAPHICS_QUEUE_PROFILER_H__

#ifdef TRACY_ENABLE
#include "Tracy.hpp"
#endif

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNMemory/inc/unique_ptr.h"
#include "core/inc/pair.h"

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/WNQueueProfilerIncludes.h"
#else
#include "core/inc/utilities.h"
#endif

#include <initializer_list>

#ifdef TRACY_ENABLE
#define PROFILE_GPU_NAMED_REGION(ctx, cmdbuf, name)                            \
  static constexpr tracy::SourceLocationData TracyConcat(                      \
      __gpu_source_location, __LINE__){                                        \
      name, __FUNCTION__, __FILE__, (uint32_t)__LINE__, 0};                    \
  memory::unique_ptr<wn::runtime::graphics::gpu_profile_scope> TracyConcat(    \
      __gpu_context, __LINE__) = ctx->profile_scope(cmdbuf,                    \
      &TracyConcat(__gpu_source_location, __LINE__));
#define PROFILE_GPU_NAMED_REGIONC(ctx, cmdbuf, name, color)                    \
  static constexpr tracy::SourceLocationData TracyConcat(                      \
      __gpu_source_location, __LINE__){                                        \
      name, __FUNCTION__, __FILE__, (uint32_t)__LINE__, color};                \
  memory::unique_ptr<wn::runtime::graphics::gpu_profile_scope> TracyConcat(    \
      __gpu_context, __LINE__) = ctx->profile_scope(cmdbuf,                    \
      &TracyConcat(__gpu_source_location, __LINE__));
#define ProfileGPUCollect(ctx) ctx->collect();
#define ProfileGPUNewFrame(ctx) ctx->new_frame();

#else  // TRACY_ENABLE
#define PROFILE_GPU_NAMED_REGION(ctx, cmdbuf, name)
#define PROFILE_GPU_NAMED_REGIONC(ctx, cmdbuf, name, color)
#define ProfileGPUCollect(ctx)
#define ProfileGPUNewFrame(ctx)
#endif
WN_GRAPHICS_FORWARD(device);

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using queue_profiler_base = _WN_GRAPHICS_DEVICE_TYPE::WN_GRAPHICS_PREFIXED_TYPE(
    queue_profiler);
#else
using queue_profiler_base = core::non_copyable;
#endif

}  // namespace internal

class command_list;
class device;
class fence;
class signal;

struct gpu_profile_scope {
  virtual ~gpu_profile_scope() {}
};

class queue_profiler : public internal::queue_profiler_base {
public:
  queue_profiler() : internal::queue_profiler_base() {}

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  virtual bool is_valid() const = 0;

  virtual ~queue_profiler() = default;

#ifdef TRACY_ENABLE
  virtual memory::unique_ptr<gpu_profile_scope> profile_scope(
      command_list* _command_list, const tracy::SourceLocationData*) = 0;
  virtual void collect() = 0;
  virtual void new_frame() = 0;
#endif
#else
  ~queue_profiler();
#endif
};

using queue_profiler_ptr = memory::unique_ptr<queue_profiler>;

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_QUEUE_PROFILER_H__
