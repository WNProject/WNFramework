// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNCore/inc/WNUtility.h"
#include "WNGraphics/inc/WNDeviceForward.h"
#include "WNGraphics/inc/WNQueueForward.h"
#include "WNMemory/inc/WNUniquePtr.h"

#pragma once

#ifndef __WN_GRAPHICS_QUEUE_H__
#define __WN_GRAPHICS_QUEUE_H__

namespace wn {
namespace graphics {

class fence;

namespace internal {

class internal_queue : public core::non_copyable {
public:
  // Note; You must call device->destroy_queue in your
  // deconstructor. There is a good reason why this
  // is not done here.
  virtual ~internal_queue() {}
  bool is_valid() {
    return m_device != nullptr;
  }

  virtual void enqueue_signal(fence& _fence) = 0;

protected:
  WN_FORCE_INLINE internal_queue(graphics::device* _device)
    : m_device(_device) {}

#include "WNGraphics/inc/Internal/WNSetFriendDevices.h"
  device* const m_device;
};

}  // namespace internal
}  // namespace graphics
}  // namespace wn

#if _WN_GRAPHICS_DEVICE_TYPES_AVAILABLE == 1
#if defined _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/Vulkan/WNQueue.h"
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/D3D12/WNQueue.h"
#endif

#endif

namespace wn {
namespace graphics {

using queue_ptr = memory::unique_ptr<queue>;

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_QUEUE_H__
