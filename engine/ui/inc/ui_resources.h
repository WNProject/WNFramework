// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_UI_RESOURCES_H__
#define __WN_ENGINE_UI_RESOURCES_H__

#include "Rocket/Core.h"
#include "WNCore/inc/pair.h"
#include "WNGraphics/inc/WNBuffer.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNMemory/inc/unique_ptr.h"
#include "renderer/inc/gpu_heap.h"
#include "renderer/inc/render_context.h"

namespace wn {
namespace engine {
namespace ui {

class ui_resource {
public:
  // returns true if this resource is still in use
  bool remove_from_frame(size_t _frame_parity) {
    frame_parity &= ~_frame_parity;
    return frame_parity == 0;
  }

  void use_in_frame(size_t _frame_parity) {
    frame_parity |= _frame_parity;
  }

private:
  size_t frame_parity = 0;
};

// This is specifically an inline texture,
// not a texture from a file.
class ui_texture : public ui_resource {
public:
  ui_texture(renderer::render_context* _context,
      runtime::graphics::data_format _format, uint32_t _width, uint32_t _height,
      runtime::graphics::resource_states _usage);

  const runtime::graphics::image::image_buffer_resource_info&
  get_buffer_info() {
    return m_texture.get_buffer_requirements(0);
  }
  runtime::graphics::image& texture() {
    return m_texture;
  }
  runtime::graphics::image_view& view() {
    return m_image_view;
  }

private:
  renderer::gpu_allocation m_gpu_allocation;
  runtime::graphics::image m_texture;
  runtime::graphics::image_view m_image_view;
};

class ui_buffer : public ui_resource {
public:
  ui_buffer(bool upload, renderer::render_context* _context, size_t data_size,
      runtime::graphics::resource_states _usage);
  runtime::graphics::buffer& buffer() {
    return m_buffer;
  }

private:
  renderer::gpu_allocation m_gpu_allocation;
  runtime::graphics::buffer m_buffer;
};

class ui_geometry : public ui_resource {
public:
  ui_geometry(memory::unique_ptr<ui_buffer> _vertex,
      memory::unique_ptr<ui_buffer> _index, ui_texture* _texture, size_t _verts,
      size_t _inds)
    : m_vertex_buffer(core::move(_vertex)),
      m_index_buffer(core::move(_index)),
      m_texture(_texture),
      m_num_vertices(_verts),
      m_num_indices(_inds) {}

  runtime::graphics::buffer& vertex_buffer() {
    return m_vertex_buffer->buffer();
  }
  runtime::graphics::buffer& index_buffer() {
    return m_index_buffer->buffer();
  }
  size_t num_verts() {
    return m_num_vertices;
  }
  size_t num_inds() {
    return m_num_indices;
  }
  ui_texture* get_texture() {
    return m_texture;
  }
  runtime::graphics::descriptor_set& set() {
    return m_set;
  }

private:
  memory::unique_ptr<ui_buffer> m_vertex_buffer;
  memory::unique_ptr<ui_buffer> m_index_buffer;
  runtime::graphics::descriptor_set m_set;
  ui_texture* m_texture;
  size_t m_num_vertices;
  size_t m_num_indices;
};

template <typename T>
inline core::pair<memory::unique_ptr<ui_buffer>, memory::unique_ptr<ui_buffer>>
create_and_initialize_gpu_buffer(memory::allocator* _allocator,
    renderer::render_context* _context,
    runtime::graphics::command_list* _upload_list, const T* _data,
    uint32_t _count, runtime::graphics::resource_state _usage) {
  const size_t uint32_t = _count * sizeof(T);
  memory::unique_ptr<ui_buffer> upload_buffer =
      memory::make_unique<ui_buffer>(m_allocator, true, _context, size,
          runtime::graphics::resource_state::host_write |
              runtime::graphics::resource_state::copy_source);
  memory::unique_ptr<ui_buffer> gpu_buffer =
      memory::make_unique<ui_buffer>(m_allocator, false, _context, size,
          _usage | runtime::graphics::resource_state::copy_dest);
  void* data = upload_buffer->buffer().map();
  wn::memory::memcpy(data, _data, size);
  upload_buffer->buffer().unmap();
  _upload_list->transition_resource(ui_buffer->get_buffer(),
      runtime::graphics::resource_state::host_write,
      runtime::graphics::resource_state::copy_source);
  _upload_list->transition_resource(gpu_buffer->get_buffer(),
      runtime::graphics::resource_state::initial,
      runtime::graphics::resource_state::copy_dest);
  _upload_list->copy_buffer(upload_buffer->buffer(), 0, gpu_buffer, 0, size);
  _upload_list->transition_resource(gpu_buffer->get_buffer(),
      runtime::graphics::resource_state::copy_dest, _usage);
  return core::make_pair(core::move(upload_buffer), core::move(gpu_buffer));
}

inline core::pair<memory::unique_ptr<ui_buffer>, memory::unique_ptr<ui_texture>>
create_and_initialize_texture(memory::allocator* _allocator,
    renderer::render_context* _context,
    runtime::graphics::command_list* _upload_list, const void* _data,
    uint32_t count, uint32_t _width, uint32_t _height,
    runtime::graphics::data_format _format,
    runtime::graphics::resource_state _usage) {
  memory::unique_ptr<ui_texture> gpu_texture =
      memory::make_unique<ui_texture>(_allocator, _context, _format, _width,
          _height, _usage | runtime::graphics::resource_state::copy_dest);
  auto buffer_reqs = gpu_texture->get_buffer_info();

  memory::unique_ptr<ui_buffer> upload_buffer = memory::make_unique<ui_buffer>(
      _allocator, true, _context, buffer_reqs.total_memory_required,
      runtime::graphics::resource_state::host_write |
          runtime::graphics::resource_state::copy_source);

  // This was just a quick and dirty test to make sure that the data size
  // seemed reasonable. This is likely wrong and needs to be fixed for any
  // block_compressed textures. Especially if block_width/height are not
  // multiples of width/height
  WN_RELEASE_ASSERT((_width / buffer_reqs.block_width) *
                            (_height / buffer_reqs.block_height) *
                            buffer_reqs.block_size ==
                        count,
      "Invalid data count");
  const char* src = static_cast<const char*>(_data);
  char* dst = static_cast<char*>(upload_buffer->buffer().map());
  const uint32_t row_size = static_cast<uint32_t>(
      (_width / buffer_reqs.block_width) * buffer_reqs.block_size);
  for (size_t i = 0; i < _height / buffer_reqs.block_height; ++i) {
    wn::memory::memcpy(dst, src, row_size);
    src += row_size;
    dst += buffer_reqs.row_pitch_in_bytes;
  }
  upload_buffer->buffer().unmap();
  _upload_list->transition_resource(upload_buffer->buffer(),
      runtime::graphics::resource_state::host_write,
      runtime::graphics::resource_state::copy_source);
  _upload_list->transition_resource(gpu_texture->texture(),
      static_cast<runtime::graphics::image_components>(
          runtime::graphics::image_component::color),
      0, 1, runtime::graphics::resource_state::initial,
      runtime::graphics::resource_state::copy_dest);
  _upload_list->copy_buffer_to_image(
      upload_buffer->buffer(), 0, gpu_texture->texture(), 0);
  _upload_list->transition_resource(gpu_texture->texture(),
      static_cast<wn::runtime::graphics::image_components>(
          runtime::graphics::image_component::color),
      0, 1, runtime::graphics::resource_state::copy_dest, _usage);
  return core::make_pair(core::move(upload_buffer), core::move(gpu_texture));
}

struct geometry_data {
  memory::unique_ptr<ui_buffer> m_setup;
  memory::unique_ptr<ui_geometry> m_data;
};

inline geometry_data create_and_initialize_geometry(
    memory::allocator* _allocator, renderer::render_context* _context,
    runtime::graphics::command_list* _upload_list,
    Rocket::Core::Vertex* _vertices, int _num_vertices, int* _indices,
    int _num_indices, ui_texture* _texture) {
  size_t vertex_size = _num_vertices * sizeof(Rocket::Core::Vertex);
  size_t index_size = _num_indices * sizeof(int);

  memory::unique_ptr<ui_buffer> vertex_buffer =
      memory::make_unique<ui_buffer>(_allocator, false, _context, vertex_size,
          runtime::graphics::resource_state::copy_dest |
              runtime::graphics::resource_state::vertex_buffer);
  memory::unique_ptr<ui_buffer> index_buffer =
      memory::make_unique<ui_buffer>(_allocator, false, _context, index_size,
          runtime::graphics::resource_state::copy_dest |
              runtime::graphics::resource_state::index_buffer);

  memory::unique_ptr<ui_buffer> upload_buffer = memory::make_unique<ui_buffer>(
      _allocator, true, _context, vertex_size + index_size,
      runtime::graphics::resource_state::host_write |
          runtime::graphics::resource_state::copy_source);

  char* vd = static_cast<char*>(upload_buffer->buffer().map());
  wn::memory::memcpy(vd, _vertices, vertex_size);
  wn::memory::memcpy(vd + vertex_size, _indices, index_size);
  upload_buffer->buffer().unmap();

  _upload_list->transition_resource(upload_buffer->buffer(),
      runtime::graphics::resource_state::host_write,
      runtime::graphics::resource_state::copy_source);
  _upload_list->transition_resource(vertex_buffer->buffer(),
      runtime::graphics::resource_state::initial,
      runtime::graphics::resource_state::copy_dest);
  _upload_list->transition_resource(index_buffer->buffer(),
      runtime::graphics::resource_state::initial,
      runtime::graphics::resource_state::copy_dest);

  _upload_list->copy_buffer(
      upload_buffer->buffer(), 0, vertex_buffer->buffer(), 0, vertex_size);

  _upload_list->copy_buffer(upload_buffer->buffer(), vertex_size,
      index_buffer->buffer(), 0, index_size);

  _upload_list->transition_resource(vertex_buffer->buffer(),
      runtime::graphics::resource_state::copy_dest,
      runtime::graphics::resource_state::vertex_buffer);
  _upload_list->transition_resource(index_buffer->buffer(),
      runtime::graphics::resource_state::copy_dest,
      runtime::graphics::resource_state::index_buffer);

  return geometry_data{
      core::move(upload_buffer),
      memory::make_unique<ui_geometry>(_allocator, core::move(vertex_buffer),
          core::move(index_buffer), _texture, _num_vertices, _num_indices),
  };
}

}  // namespace ui
}  // namespace engine
}  // namespace wn
#endif  // __WN_ENGINE_UI_RESOURCES_H__