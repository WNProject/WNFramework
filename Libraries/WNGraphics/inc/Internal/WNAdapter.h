// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_PHYSICAL_DEVICE_H__
#define __WN_GRAPHICS_INTERNAL_PHYSICAL_DEVICE_H__

#include "WNContainers/inc/WNStringView.h"
#include "WNCore/inc/WNUtility.h"
#include "WNGraphics/inc/WNDeviceForward.h"

namespace WNLogging {

class WNLog;

}  // namespace WNLogging

namespace wn {
namespace graphics {
namespace internal {

class internal_adapter : core::non_copyable {
public:
  enum class api_type { invalid, vulkan, d3d12, max };

  WN_FORCE_INLINE internal_adapter(containers::string&& _name,
      const uint32_t _vendor_id, const uint32_t _device_id,
      const api_type _api_type)
    : m_name(std::move(_name)),
      m_vendor_id(_vendor_id),
      m_device_id(_device_id),
      m_api(_api_type) {}

  virtual ~internal_adapter() = default;

  virtual device_ptr make_device(
      memory::allocator* _allocator, WNLogging::WNLog* _log) const = 0;

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

protected:
  const containers::string m_name;
  const uint32_t m_vendor_id;
  const uint32_t m_device_id;
  const api_type m_api;
};

}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_PHYSICAL_DEVICE_H__