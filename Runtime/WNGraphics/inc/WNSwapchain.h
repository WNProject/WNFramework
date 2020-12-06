// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_SWAPCHAIN_H__
#define __WN_GRAPHICS_SWAPCHAIN_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNMemory/inc/unique_ptr.h"

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
#include "WNGraphics/inc/Internal/WNSwapchainIncludes.h"
#else
#include "core/inc/utilities.h"
#endif

WN_GRAPHICS_FORWARD(device);

namespace wn {
namespace runtime {
namespace graphics {

class image;
class queue;
class fence;
class signal;

enum class swap_mode { immediate, mailbox, fifo, fifo_relaxed };

enum class discard_policy { keep, discard };

struct swapchain_create_info {
  data_format format;
  uint32_t num_buffers;
  swap_mode mode;
  discard_policy discard;
};

namespace internal {

#ifdef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using swapchain_base = _WN_GRAPHICS_DEVICE_TYPE::WN_GRAPHICS_PREFIXED_TYPE(
    swapchain);
#else
using swapchain_base = core::non_copyable;
#endif

}  // namespace internal

class swapchain : public internal::swapchain_base {
public:
  swapchain() : internal::swapchain_base(), m_create_info({}) {}

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  // Note; You must call device->destroy_swapchain in your
  // deconstructor. There is a good reason why this
  // is not done here.
  virtual ~swapchain() = default;

  virtual image* get_image_for_index(uint32_t index) = 0;
  virtual uint32_t get_next_backbuffer_index(
      fence* fence, signal* _signal) const = 0;

#else
  ~swapchain() {}
#endif
  WN_GRAPHICS_ADD_FRIENDS(device);
  const swapchain_create_info& info() const {
    return m_create_info;
  }

  void present(queue* q, signal* _signal, uint32_t image_index) const {
    return present_internal(q, m_create_info, _signal, image_index);
  }

private:
#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
  virtual void present_internal(queue* q, const swapchain_create_info& _info,
      signal* _signal, uint32_t image_index) const = 0;
#endif
  void set_create_info(const swapchain_create_info& _info) {
    m_create_info = _info;
  }
  swapchain_create_info m_create_info;
};

using swapchain_ptr = memory::unique_ptr<swapchain>;

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_SWAPCHAIN_H__
