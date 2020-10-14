// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "renderer/inc/texture.h"
#include "WNFileSystem/inc/WNFile.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNGraphics/inc/WNBuffer.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNScripting/inc/forward.h"
#include "renderer/inc/render_context.h"

#include <stb_image.h>
namespace wn {
namespace engine {
namespace renderer {
texture::texture(render_context* _render_context,
    file_system::mapping* _mapping,
    runtime::graphics::command_list* _setup_list,
    scripting::script_pointer<texture_desc> _texture_desc, logging::log* _log) {
  const char* path = _texture_desc.invoke(&texture_desc::get_name);
  int32_t width = _texture_desc.invoke(&texture_desc::get_width);
  int32_t height = _texture_desc.invoke(&texture_desc::get_height);
  int32_t fmt = _texture_desc.invoke(&texture_desc::get_format);
  WN_RELEASE_ASSERT(fmt == static_cast<int32_t>(
                               runtime::graphics::data_format::r8g8b8a8_unorm),
      "TODO: Support non r8g8b8a8 formats");

  file_system::result res;
  file_system::file_ptr file = _mapping->open_file(path, res);
  if (!file) {
    _log->log_error("Could not load texture source file ", path);
    return;
  }
  int x = 0;
  int y = 0;
  int chans = 4;

  stbi_uc* data = stbi_load_from_memory(file->typed_data<stbi_uc>(),
      static_cast<int>(file->typed_size<stbi_uc>()), &x, &y, &chans, 4);
  if (!data) {
    _log->log_error("Could not parse texture source file as png", path);
    _log->log_error("  We only support source png for now");
    return;
  }
  WN_RELEASE_ASSERT(chans == 4, "Could not convert to 4-channel png");
  bool needs_resizing = false;
  if (x != width || y != height) {
    _log->log_warning("Trying to load a texture of size [", x, ":", y,
        "] as a texture of size [", width, ":", height,
        "], runtime resizing will occur");
    needs_resizing = true;
  }

  // Create the image resource
  runtime::graphics::image_create_info create_info{static_cast<size_t>(width),
      static_cast<size_t>(height),
      static_cast<runtime::graphics::data_format>(fmt),
      runtime::graphics::resource_state::texture |
          runtime::graphics::resource_state::copy_dest,
      1};
  runtime::graphics::clear_value c{};
  m_image = _render_context->get_device()->create_image(create_info, c);
  runtime::graphics::image_memory_requirements reqs =
      m_image.get_memory_requirements();
  m_allocation =
      _render_context->get_allocation_for_texture(reqs.size, reqs.alignment);
  if (!m_allocation.is_valid()) {
    _log->log_error("Could not allocate memory for texture");
    return;
  }
  m_image.bind_memory(m_allocation.arena(), m_allocation.offset());

  runtime::graphics::image* upload_image = &m_image;
  runtime::graphics::image* temp_image = nullptr;
  if (needs_resizing) {
    create_info.m_valid_resource_states =
        runtime::graphics::resource_state::copy_source |
        runtime::graphics::resource_state::blit_dest;
    temp_image = _render_context->create_temporary_image(&create_info);
    if (!temp_image) {
      _log->log_error("Could not create temporary image, (oom?)");
      return;
    }
    create_info.m_width = x;
    create_info.m_height = y;
    create_info.m_valid_resource_states =
        runtime::graphics::resource_state::copy_dest |
        runtime::graphics::resource_state::blit_source;
    upload_image = _render_context->create_temporary_image(&create_info);
    if (!upload_image) {
      _log->log_error("Could not create temporary image, (oom?)");
      return;
    }
  }

  // Create the buffer
  runtime::graphics::image::image_buffer_resource_info buffer_reqs =
      upload_image->get_buffer_requirements(0);
  runtime::graphics::buffer* upload =
      _render_context->create_temporary_upload_buffer(
          buffer_reqs.total_memory_required);
  uint8_t* v = static_cast<uint8_t*>(upload->map());
  for (size_t i = 0; i < y; ++i) {
    memcpy(v, data + (i * chans * x), chans * x);
    v += buffer_reqs.row_pitch_in_bytes;
  }
  upload->unmap();
  stbi_image_free(data);

  _setup_list->transition_resource(*upload_image,
      static_cast<runtime::graphics::image_components>(
          runtime::graphics::image_component::color),
      0, 1, runtime::graphics::resource_state::initial,
      runtime::graphics::resource_state::copy_dest);
  _setup_list->transition_resource(*upload,
      runtime::graphics::resource_state::host_write,
      runtime::graphics::resource_state::copy_source);
  _setup_list->copy_buffer_to_image(*upload, 0, *upload_image, 0);

  if (needs_resizing) {
    _setup_list->transition_resource(*upload_image,
        static_cast<runtime::graphics::image_components>(
            runtime::graphics::image_component::color),
        0, 1, runtime::graphics::resource_state::copy_dest,
        runtime::graphics::resource_state::blit_source);
    _setup_list->transition_resource(*temp_image,
        static_cast<runtime::graphics::image_components>(
            runtime::graphics::image_component::color),
        0, 1, runtime::graphics::resource_state::initial,
        runtime::graphics::resource_state::blit_dest);
    _setup_list->blit_image(*upload_image, 0, *temp_image, 0);
    _setup_list->transition_resource(*temp_image,
        static_cast<runtime::graphics::image_components>(
            runtime::graphics::image_component::color),
        0, 1, runtime::graphics::resource_state::blit_dest,
        runtime::graphics::resource_state::copy_source);
    _setup_list->transition_resource(m_image,
        static_cast<runtime::graphics::image_components>(
            runtime::graphics::image_component::color),
        0, 1, runtime::graphics::resource_state::initial,
        runtime::graphics::resource_state::copy_dest);
    _setup_list->copy_image(*temp_image, 0, m_image, 0);
    _setup_list->transition_resource(m_image,
        static_cast<runtime::graphics::image_components>(
            runtime::graphics::image_component::color),
        0, 1, runtime::graphics::resource_state::copy_dest,
        runtime::graphics::resource_state::texture);
  } else {
    _setup_list->transition_resource(*upload_image,
        static_cast<runtime::graphics::image_components>(
            runtime::graphics::image_component::color),
        0, 1, runtime::graphics::resource_state::copy_dest,
        runtime::graphics::resource_state::texture);
  }

  m_image_view =
      _render_context->get_device()->create_image_view(&m_image, 0, 1,
          static_cast<runtime::graphics::image_components>(
              runtime::graphics::image_component::color));
  m_valid = true;
}
}  // namespace renderer
}  // namespace engine
}  // namespace wn