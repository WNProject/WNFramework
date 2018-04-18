// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_IMAGE_H__
#define __WN_GRAPHICS_IMAGE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNGraphicsTypes.h"
#include "WNMemory/inc/manipulation.h"

WN_GRAPHICS_FORWARD(device);
WN_GRAPHICS_FORWARD(command_list);

namespace wn {
namespace runtime {
namespace graphics {

class arena;

struct image_memory_requirements {
  size_t size;
  size_t alignment;
};

class image final : public core::non_copyable {
public:
  // The offset for uploading/downloading data is valid
  // assuming that the base-alignment of the
  // write is m_device->get_image_upload_buffer_alignment
  // aligned.
  struct image_buffer_resource_info {
    data_format format;         // The format of the image
    size_t width;               // The width of the image
    size_t height;              // The height of the image
    size_t depth;               // The depth of the image
    size_t row_pitch_in_bytes;  // The required row_pitch of the image needed
                                // for upload
    size_t offset_in_bytes;  // The offset from the start of the upload/download
                             //  data needed for this image
    size_t total_memory_required;  // This is the total memory needed in the
                                   // upload/download heap
  };

  image() = delete;

  WN_FORCE_INLINE image(image&& _other)
    : m_device(_other.m_device), m_resource_info(_other.m_resource_info) {
    _other.m_device = nullptr;
    memory::memcpy(&m_resource_info, &_other.m_resource_info,
        sizeof(_other.m_resource_info));
    memory::memory_zero(&_other.m_resource_info);
    m_is_swapchain_image = _other.m_is_swapchain_image;
    memory::memcpy(&m_data, &_other.m_data, sizeof(opaque_data));
    memory::memzero(&_other.m_data, sizeof(opaque_data));
  }

  WN_FORCE_INLINE ~image() {
    if (is_valid() && !m_is_swapchain_image) {
      m_device->destroy_image(this);
    }
  }

  WN_FORCE_INLINE bool is_valid() const {
    return (m_device != nullptr);
  }

  WN_FORCE_INLINE bool is_swapchain_image() const {
    return m_is_swapchain_image;
  }

  const image_buffer_resource_info& get_buffer_requirements() const {
    return m_resource_info;
  }

  size_t get_width() const {
    return m_resource_info.width;
  }

  size_t get_height() const {
    return m_resource_info.height;
  }

  image_memory_requirements get_memory_requirements() const {
    return m_device->get_image_memory_requirements(this);
  }

  void bind_memory(arena* _arena, size_t _offset) {
    m_device->bind_image_memory(this, _arena, _offset);
  }

private:
  WN_GRAPHICS_ADD_FRIENDS(device);
  WN_GRAPHICS_ADD_FRIENDS(command_list);
  WN_GRAPHICS_ADD_FRIENDS(swapchain);

  WN_FORCE_INLINE image(device* _device)
    : m_data({0}), m_device(_device), m_is_swapchain_image(false) {}

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
  struct opaque_data final {
    uint64_t _dummy[2];
  } m_data;

  device* m_device;
  image_buffer_resource_info m_resource_info;
  bool m_is_swapchain_image;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_IMAGE_H__
