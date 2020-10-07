// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "ui/inc/ui_resources.h"

namespace wn {
namespace engine {
namespace ui {

ui_buffer::ui_buffer(bool _upload, renderer::render_context* _context,
    size_t data_size, wn::runtime::graphics::resource_states _usage) {
  auto* device = _context->get_device();
  m_buffer = device->create_buffer(data_size, _usage);

  runtime::graphics::buffer_memory_requirements reqs =
      m_buffer.get_memory_requirements();
  m_gpu_allocation =
      _upload ? _context->get_allocation_for_upload(reqs.size, reqs.alignment)
              : _context->get_allocation_for_buffer(reqs.size, reqs.alignment);
  m_buffer.bind_memory(m_gpu_allocation.arena(), m_gpu_allocation.offset());
}

ui_texture::ui_texture(renderer::render_context* _context,
    wn::runtime::graphics::data_format _format, uint32_t _width,
    uint32_t _height, wn::runtime::graphics::resource_states _usage) {
  auto* device = _context->get_device();
  wn::runtime::graphics::image_create_info create_info = {
      _width, _height, _format, _usage, 1};
  wn::runtime::graphics::clear_value cc;
  m_texture = device->create_image(create_info, cc);

  runtime::graphics::image_memory_requirements reqs =
      m_texture.get_memory_requirements();

  m_gpu_allocation =
      _context->get_allocation_for_texture(reqs.size, reqs.alignment);
  m_texture.bind_memory(m_gpu_allocation.arena(), m_gpu_allocation.offset());

  m_image_view = device->create_image_view(&m_texture, 0, 1,
      static_cast<wn::runtime::graphics::image_components>(
          wn::runtime::graphics::image_component::color));
}

}  // namespace ui
}  // namespace engine
}  // namespace wn