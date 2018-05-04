// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_QUEUE_H__
#define __WN_GRAPHICS_QUEUE_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNCore/inc/pair.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNMemory/inc/unique_ptr.h"

#include <initializer_list>

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/WNQueueIncludes.h"
#else
#include "WNCore/inc/utilities.h"
#endif

WN_GRAPHICS_FORWARD(device);

namespace wn {
namespace runtime {
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
class signal;

class queue : public internal::queue_base {
public:
  queue() : internal::queue_base() {}

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  virtual bool is_valid() const = 0;

  virtual void enqueue_fence(fence& _fence) = 0;
  virtual void enqueue_command_list(command_list* _command_list) = 0;

  virtual void enqueue_commands(
      containers::contiguous_range<command_list*> _command_lists,
      containers::contiguous_range<core::pair<pipeline_stages, signal*>>
          _wait_signals,
      fence* _signal_fence,
      containers::contiguous_range<signal*> _signal_signals) = 0;

  virtual void enqueue_command_lists(
      std::initializer_list<command_list*> _command_lists,
      std::initializer_list<core::pair<pipeline_stages, signal*>> _wait_signals,
      fence* _signal_fence, std::initializer_list<signal*> _signal_signals) = 0;

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
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_QUEUE_H__
