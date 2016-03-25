// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_QUEUE_H__
#define __WN_GRAPHICS_QUEUE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNMemory/inc/WNUniquePtr.h"

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#ifdef _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/Vulkan/WNQueue.h"
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
#include "WNGraphics/inc/Internal/D3D12/WNQueue.h"
#endif
#else
#include "WNCore/inc/WNUtility.h"
#endif

namespace wn {
namespace graphics {
namespace internal {
namespace d3d12 {

class d3d12_device;

}  // namespace d3d12

namespace vulkan {

class vulkan_device;

}  // namespace vulkan

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#ifdef _WN_GRAPHICS_VULKAN_DEVICE_TYPE_AVAILABLE
using queue_base = internal::vulkan::vulkan_queue;
#elif defined _WN_GRAPHICS_D3D12_DEVICE_TYPE_AVAILABLE
using queue_base = internal::d3d12::d3d12_queue;
#endif
#else
using queue_base = core::non_copyable;
#endif

}  // namespace internal

class command_list;
class device;
class fence;

class queue : public internal::queue_base {
public:
  queue() : internal::queue_base() {}

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  virtual bool is_valid() const = 0;

  virtual void enqueue_signal(fence& _fence) = 0;
  virtual void enqueue_command_list(command_list* _command_list) = 0;

  // Note; You must call device->destroy_queue in your
  // deconstructor. There is a good reason why this
  // is not done here.
  virtual ~queue() = default;
#else
  ~queue();
#endif
};

using queue_ptr = memory::unique_ptr<queue>;

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_QUEUE_H__
