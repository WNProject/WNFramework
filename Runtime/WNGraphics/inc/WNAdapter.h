// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_GRAPHICS_ADAPTER_H__
#define __WN_RUNTIME_GRAPHICS_ADAPTER_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNUniquePtr.h"

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/WNAdapterIncludes.h"
#else
#include "WNContainers/inc/WNString.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNCore/inc/WNUtility.h"
#endif

namespace wn {
namespace runtime {
namespace graphics {

struct adapter_features {
  bool non_solid_fill = false;
  bool tessellation = false;
  bool geometry = false;
  bool depth_clamp = false;
  bool depth_bias_clamp = false;
  bool dual_src_blending = false;
  bool sample_rate_shading = false;
  bool input_attachments = false;

  bool etc2_textures = false;
  bool astc_ldr_textures = false;
  bool bc_textures = false;

  bool clip_distance = false;
  bool cull_distance = false;
  uint32_t num_viewports = 1;
  uint32_t num_scissors = 1;
};

namespace internal {

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using adapter_base = _WN_GRAPHICS_DEVICE_TYPE::WN_GRAPHICS_PREFIXED_TYPE(
    adapter);
#else
using adapter_base = core::non_copyable;
#endif

}  // namespace internal

class device;

using device_ptr = memory::unique_ptr<device>;

class adapter : public internal::adapter_base {
public:
#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  enum class api_type{invalid, vulkan, d3d12, max};

  virtual ~adapter() = default;

  virtual device_ptr make_device(
      memory::allocator* _allocator, logging::log* _log) const = 0;

  virtual containers::string_view name() const = 0;

  virtual uint32_t vendor_id() const = 0;

  virtual uint32_t device_id() const = 0;

  virtual api_type api() const = 0;
#endif
};

using adapter_ptr = memory::unique_ptr<adapter>;

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_GRAPHICS_ADAPTER_H__
