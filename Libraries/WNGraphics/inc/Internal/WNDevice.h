// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_DEVICE_H__
#define __WN_GRAPHICS_INTERNAL_DEVICE_H__

#include "WNCore/inc/WNUtility.h"

namespace WNLogging {

class WNLog;

}  // namespace WNLogging

namespace wn {
namespace memory {

class allocator;

}  // namespace memory

namespace graphics {
namespace internal {

class device : public core::non_copyable {
public:
  WN_FORCE_INLINE device(memory::allocator* _allocator, WNLogging::WNLog* _log)
    : m_allocator(_allocator), m_log(_log) {}

protected:
  memory::allocator* m_allocator;
  WNLogging::WNLog* m_log;
};

}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_DEVICE_H__