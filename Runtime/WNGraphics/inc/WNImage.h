// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_IMAGE_H__
#define __WN_GRAPHICS_IMAGE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNImageFormats.h"
#include "WNGraphics/inc/WNResourceStates.h"
#include "WNMemory/inc/WNBasic.h"

WN_GRAPHICS_FORWARD(device);
WN_GRAPHICS_FORWARD(command_list);

namespace wn {
namespace graphics {

struct image_create_info {
  static image_create_info default_texture(size_t _width, size_t _height) {
    return image_create_info{_width, _height, image_format::r8g8b8a8_unorm};
  }

  size_t m_width;
  size_t m_height;
  image_format m_format;
  // TODO(awoloszyn): Add mip-levels
  // TODO(awoloszyn): Add Depth/Array Size
  // TODO(awoloszyn): Add Multisample information
};

class image WN_FINAL : public core::non_copyable {
public:
  // The offset for uploading/downloading data is valid
  // assuming that the base-alignment of the
  // write is m_device->get_image_upload_buffer_alignment
  // aligned.
  struct image_buffer_resource_info {
    image_format format;        // The format of the image
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
    memory::memory_zero(&_other.m_resource_info);
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

  const image_buffer_resource_info& get_resource_info() const {
    return m_resource_info;
  }

  size_t get_width() const {
    return m_resource_info.width;
  }

  size_t get_height() const {
    return m_resource_info.height;
  }

private:
  WN_GRAPHICS_ADD_FRIENDS(device);
  WN_GRAPHICS_ADD_FRIENDS(command_list);
  WN_GRAPHICS_ADD_FRIENDS(swapchain);

  WN_FORCE_INLINE image(device* _device)
    : m_device(_device), m_data({0}), m_is_swapchain_image(false) {}

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

  image_buffer_resource_info m_resource_info;
  bool m_is_swapchain_image;
  device* m_device;
};

}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_IMAGE_H__