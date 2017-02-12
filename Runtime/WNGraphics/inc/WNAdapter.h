// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef _WN_GRAPHICS_PHYSICAL_DEVICE_H__
#define _WN_GRAPHICS_PHYSICAL_DEVICE_H__

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
namespace graphics {
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
}  // namespace wn

#endif  // _WN_GRAPHICS_PHYSICAL_DEVICE_H__