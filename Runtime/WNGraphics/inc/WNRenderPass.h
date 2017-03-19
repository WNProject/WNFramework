// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_RENDER_PASS_H__
#define __WN_GRAPHICS_RENDER_PASS_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNMemory/inc/WNBasic.h"

WN_GRAPHICS_FORWARD(queue)
WN_GRAPHICS_FORWARD(device)

namespace wn {
namespace graphics {

struct render_pass_attachment {
  image_format format = image_format::r8g8b8a8_unorm;
  multisample_count num_samples = multisample_count::samples_1;
  load_op load_op = load_op::load;
  store_op store_op = store_op::store;
  resource_state initial_state = resource_state::render_target;
  resource_state final_state = resource_state::render_target;
};

struct attachment_reference {
  uint32_t attachment = 0;
  resource_state state = resource_state::render_target;
};

struct subpass_dependency {
  uint32_t src;
  uint32_t dst;
  resource_state src_state = resource_state::render_target;
  resource_state dst_state = resource_state::render_target;
  bool by_region = false;
}

// TODO(awoloszyn): Handle subpass dependencies.
struct subpass_description {
  // Must be size_0 if the device does not support input attachments.
  containers::contiguous_range<attachment_reference> input_attachments;
  containers::contiguous_range<attachment_reference> color_attachments;
  containers::contiguous_range<attachment_reference> resolve_attachments;
  bool depth_stencil_enabled = false;
  attachment_reference depth_stencil;
  // resource_state is unused for preserve attachments.
  containers::contiguous_range<attachment_reference> preserve_attachments;
};

class render_pass WN_FINAL : public core::non_copyable {
public:
  render_pass() = delete;

  WN_FORCE_INLINE render_pass(render_pass&& _other)
    : m_device(_other.m_device) {
    _other.m_device = nullptr;

    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }

  WN_FORCE_INLINE ~render_pass() {
    if (is_valid()) {
      m_device->destroy_render_pass(this);
    }
  }

  WN_FORCE_INLINE bool is_valid() const {
    return (m_device != nullptr);
  }

private:
  WN_GRAPHICS_ADD_FRIENDS(queue)
  WN_GRAPHICS_ADD_FRIENDS(device)

  WN_FORCE_INLINE render_pass(device* _device)
    : m_device(_device), m_data({0}) {}

  template <typename T>
  WN_FORCE_INLINE T& data_as() {
    static_assert(sizeof(opaque_data) >= sizeof(T),
        "invalid cast, target type size does not match opaque data size");

    return *reinterpret_cast<T*>(&m_data);
  }

  template <typename T>
  WN_FORCE_INLINE const T& data_as() const {
    static_assert(sizeof(opaque_data) >= sizeof(T),
        "invalid cast, target type size does not match opaque data size");

    return *reinterpret_cast<const T*>(&m_data);
  }

  // The opaque_data must be trivially copyable.
  // It also must be considered uninitialized when
  // memset to 0.
  struct opaque_data WN_FINAL {
    uint64_t _dummy[2];
  } m_data;

  device* m_device;
};

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_RENDER_PASS_H__
