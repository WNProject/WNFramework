// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#pragma once

#ifndef __WN_ENGINE_TEMPORARY_BUFFER_H__
#define __WN_ENGINE_TEMPORARY_BUFFER_H__
#include "WNGraphics/inc/WNBuffer.h"
#include "WNGraphics/inc/WNImage.h"
#include "renderer/inc/gpu_heap.h"

namespace wn {
namespace engine {
namespace renderer {

class temporary_buffer {
public:
  temporary_buffer(
      runtime::graphics::buffer _buffer, gpu_allocation _gpu_allocation)
    : m_buffer(core::move(_buffer)),
      m_buffer_allocation(core::move(_gpu_allocation)) {}
  temporary_buffer(temporary_buffer&& _other)
    : m_buffer(core::move(_other.m_buffer)),
      m_buffer_allocation(core::move(_other.m_buffer_allocation)) {}
  runtime::graphics::buffer* buffer() {
    return &m_buffer;
  }

private:
  runtime::graphics::buffer m_buffer;
  gpu_allocation m_buffer_allocation;
};

class temporary_image {
public:
  temporary_image(
      runtime::graphics::image _image, gpu_allocation _gpu_allocation)
    : m_image(core::move(_image)),
      m_image_allocation(core::move(_gpu_allocation)) {}
  temporary_image(temporary_image&& _other)
    : m_image(core::move(_other.m_image)),
      m_image_allocation(core::move(_other.m_image_allocation)) {}
  runtime::graphics::image* image() {
    return &m_image;
  }

private:
  runtime::graphics::image m_image;
  gpu_allocation m_image_allocation;
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_TEMPORARY_BUFFER_H__
