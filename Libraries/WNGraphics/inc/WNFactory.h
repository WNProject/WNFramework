// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_FACTORY_H__
#define __WN_GRAPHICS_FACTORY_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNGraphics/inc/WNPhysicalDevice.h"
#include "WNLogging/inc/WNLog.h"
#include <mutex>

namespace wn {
namespace graphics {

// This factory must outlive any objects created by any of the returned phsycial
// devices
class factory {
public:
  WN_FORCE_INLINE factory(memory::allocator* _allocator, WNLogging::WNLog* _log)
    : m_allocator(_allocator), m_log(_log) {}

  virtual ~factory() = default;

  virtual containers::contiguous_range<const physical_device_ptr>
  query_physical_devices() const;

private:
  void query_devices() const;

  mutable containers::dynamic_array<physical_device_ptr> m_physical_devices;
  mutable std::once_flag m_query_physical_device_once_flag;
  memory::allocator* m_allocator;
  WNLogging::WNLog* m_log;
};

}  // namespace graphics
}  // namespace wn

#endif  // _WN_GRAPHICS_FACTORY_H__s