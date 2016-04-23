// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_QUEUE_H__
#define __WN_GRAPHICS_QUEUE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNMemory/inc/WNUniquePtr.h"

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/WNQueueIncludes.h"
#else
#include "WNCore/inc/WNUtility.h"
#endif

WN_GRAPHICS_FORWARD(device);

namespace wn {
namespace graphics {
namespace internal {

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using queue_base = _WN_GRAPHICS_DEVICE_TYPE::WN_GRAPHICS_PREFIXED_TYPE(queue);
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
