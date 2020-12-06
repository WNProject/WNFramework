// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_APPLICATIONS_TEXTURE_H__
#define __WN_APPLICATIONS_TEXTURE_H__

#include "WNGraphics/inc/WNBuffer.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNQueue.h"

#include "texture_file.h"

using namespace wn;
using namespace wn::runtime;

// Texture uploads, for now are really dumb.
// For each texture to upload, allocate memory, allocate
// a staging buffer, fill the staging buffer, do the copies.
// submit to queue, wait on completion, delete buffer.
// We should really make this smarter, but that can be done
// later.
class texture {
public:
  texture(memory::allocator* _allocator, logging::log* _log,
      const containers::string_view& _texture_name, texture_file* _texture,
      uint32_t _arena_index, uint32_t _upload_arena_index,
      graphics::device* _device, graphics::queue* _staging_queue,
      graphics::command_allocator* _command_allocator) {
    _log->log_debug("Uploading data for image ", _texture_name);
    graphics::fence completion_fence = _device->create_fence();

    graphics::image_create_info info = {_texture->width(), _texture->height(),
        _texture->format(),
        graphics::resource_states(
            static_cast<uint32_t>(graphics::resource_state::copy_dest) |
            static_cast<uint32_t>(graphics::resource_state::texture)),
        static_cast<uint32_t>(_texture->data().size())};
    m_texture = memory::make_unique<graphics::image>(
        _allocator, _device->create_image(info, graphics::clear_value{}));

    auto reqs = m_texture->get_memory_requirements();
    m_texture_memory = memory::make_unique<graphics::arena>(
        _allocator, _device->create_arena(_arena_index, reqs.size));
    m_texture->bind_memory(m_texture_memory.get(), 0);

    const size_t alignment = _device->get_image_upload_buffer_alignment();
    WN_DEBUG_ASSERT((alignment & (alignment - 1)) == 0,
        "Make sure alignment is power of 2");
    size_t total_size = 0;

    for (uint32_t i = 0; i < _texture->data().size(); ++i) {
      const auto& mip_req = m_texture->get_buffer_requirements(i);
      auto pitch = mip_req.row_pitch_in_bytes;
      auto height_in_blocks = (mip_req.height + _texture->block_height() - 1) /
                              _texture->block_height();
      total_size += pitch * height_in_blocks;
      total_size = (total_size + alignment - 1) & ~(alignment - 1);
    }

    graphics::buffer upload_buffer = _device->create_buffer(total_size,
        static_cast<graphics::resource_states>(
            static_cast<uint32_t>(graphics::resource_state::host_write) |
            static_cast<uint32_t>(graphics::resource_state::copy_source)));

    graphics::buffer_memory_requirements buffer_reqs =
        upload_buffer.get_memory_requirements();

    graphics::arena upload_arena =
        _device->create_arena(_upload_arena_index, buffer_reqs.size);

    upload_buffer.bind_memory(&upload_arena, 0);

    uint8_t* v = static_cast<uint8_t*>(upload_buffer.map());
    size_t write_offset = 0;
    containers::dynamic_array<size_t> write_offsets(
        _allocator, _texture->data().size());

    for (uint32_t i = 0; i < _texture->data().size(); ++i) {
      write_offsets[i] = write_offset;
      const auto& dat = _texture->data()[i];
      const auto& mip_req = m_texture->get_buffer_requirements(i);
      auto pitch = mip_req.row_pitch_in_bytes;
      auto stride_width =
          (mip_req.width + mip_req.block_width - 1) / mip_req.block_width;
      auto height_in_blocks =
          (mip_req.height + mip_req.block_height - 1) / mip_req.block_height;
      auto required_size = pitch * height_in_blocks;

      // Make sure the file is of the expected size.
      WN_DEBUG_ASSERT(
          stride_width * height_in_blocks * mip_req.block_size == dat.size(),
          "The file does not have the expected dimensions");
      if (dat.size() == required_size) {
        memcpy(v + write_offset, dat.data(), dat.size());
        write_offset += dat.size();
      } else {
        const uint8_t* base = dat.data();
        uint8_t* write_base = v + write_offset;
        for (size_t j = 0; j < height_in_blocks; ++j) {
          memcpy(write_base, base, stride_width * mip_req.block_size);
          write_base += pitch;
          base += stride_width * mip_req.block_size;
          write_offset += pitch;
        }
      }
      write_offset = (write_offset + alignment - 1) & ~(alignment - 1);
    }
    upload_buffer.unmap();

    graphics::command_list_ptr list = _command_allocator->create_command_list();
    list->transition_resource(upload_buffer,
        graphics::resource_state::host_write,
        graphics::resource_state::copy_source);
    list->transition_resource(*m_texture,
        static_cast<wn::runtime::graphics::image_components>(
            wn::runtime::graphics::image_component::color),
        0, static_cast<uint32_t>(_texture->data().size()),
        graphics::resource_state::initial, graphics::resource_state::copy_dest);
    uint32_t mip_level = 0;
    for (auto dat : _texture->data()) {
      (void)dat;
      list->copy_buffer_to_image(
          upload_buffer, write_offsets[mip_level], *m_texture, mip_level);
      ++mip_level;
    }

    list->transition_resource(*m_texture,
        static_cast<wn::runtime::graphics::image_components>(
            wn::runtime::graphics::image_component::color),
        0, static_cast<uint32_t>(_texture->data().size()),
        graphics::resource_state::copy_dest, graphics::resource_state::texture);
    list->finalize();

    _staging_queue->enqueue_command_list(list.get());
    _staging_queue->enqueue_fence(completion_fence);

    completion_fence.wait();
    _log->log_debug("Done uploading data for image ", _texture_name);
    _log->flush();

    m_image_view = memory::make_unique<graphics::image_view>(
        _allocator, _device->create_image_view(m_texture.get(), 0,
                        static_cast<uint32_t>(_texture->data().size()),
                        static_cast<graphics::image_components>(
                            graphics::image_component::color)));
  }

  graphics::image_view* get_image_view() {
    return m_image_view.get();
  }

private:
  memory::unique_ptr<graphics::image> m_texture;
  memory::unique_ptr<graphics::arena> m_texture_memory;
  memory::unique_ptr<graphics::image_view> m_image_view;
};

#endif  // __WN_APPLICATIONS_TEXTURE_H__
