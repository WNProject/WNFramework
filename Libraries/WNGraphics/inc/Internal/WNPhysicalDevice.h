// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_PHYSICAL_DEVICE_H__
#define __WN_GRAPHICS_INTERNAL_PHYSICAL_DEVICE_H__

#include "WNContainers/inc/WNStringView.h"
#include "WNGraphics/inc/WNDevice.h"

namespace wn {
namespace graphics {
namespace internal {

class physical_device {
public:
  enum class api_type { vulkan, d3d12 };

  virtual ~physical_device() = default;

  virtual device_ptr make_device() const = 0;

  WN_FORCE_INLINE containers::string_view name() const {
    return m_name;
  }

  WN_FORCE_INLINE uint32_t vendor_id() const {
    return m_vendor_id;
  }

  WN_FORCE_INLINE uint32_t device_id() const {
    return m_device_id;
  }

  WN_FORCE_INLINE api_type api() const {
    return m_api;
  }

private:
  containers::string m_name;
  uint32_t m_vendor_id;
  uint32_t m_device_id;
  api_type m_api;
};

}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_PHYSICAL_DEVICE_H__