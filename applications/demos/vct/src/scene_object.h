// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_APPLICATIONS_DEMOS_SCENE_OBJECT_H__
#define __WN_APPLICATIONS_DEMOS_SCENE_OBJECT_H__

class texture_file;

#include "WNContainers/inc/WNArray.h"
#include "WNGraphics/inc/WNBuffer.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNCommandList.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNGraphicsPipeline.h"
#include "WNGraphics/inc/WNGraphicsPipelineDescription.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/inc/WNShaderModule.h"

#include "object.h"
#include "texture.h"

using namespace wn;
using namespace wn::runtime;

class entity {
public:
  entity(memory::allocator* _allocator, logging::log* _log,
      graphics::device* _device,
      graphics::command_allocator* _command_allocator,
      graphics::queue* _staging_queue, uint32_t _device_buffer_arena,
      uint32_t _staging_buffer_arena, const entity_in_file* _entity,
      const containers::hash_map<const texture_file*,
          memory::unique_ptr<texture>>* _textures)
    : m_allocator(_allocator) {
    _log->log_debug("Started uploading data for ", _entity->get_name());

    graphics::fence completion_fence = _device->create_fence();

    uint32_t vb_size = static_cast<uint32_t>(
        _entity->get_vertices().size() * sizeof(packed_vertex));

    graphics::buffer vertex_staging = _device->create_buffer(vb_size,
        static_cast<graphics::resource_states>(
            static_cast<uint32_t>(graphics::resource_state::copy_source) |
            static_cast<uint32_t>(graphics::resource_state::host_write)));

    m_vertex_buffer = memory::make_unique<graphics::buffer>(_allocator,
        _device->create_buffer(vb_size,
            static_cast<graphics::resource_states>(
                static_cast<uint32_t>(graphics::resource_state::vertex_buffer) |
                static_cast<uint32_t>(graphics::resource_state::copy_dest))));

    m_vertex_buffer_memory = memory::make_unique<graphics::arena>(
        _allocator, _device->create_arena(_device_buffer_arena,
                        m_vertex_buffer->get_memory_requirements().size));
    graphics::arena vertex_staging_arena = _device->create_arena(
        _staging_buffer_arena, vertex_staging.get_memory_requirements().size);

    m_vertex_buffer->bind_memory(m_vertex_buffer_memory.get(), 0);
    vertex_staging.bind_memory(&vertex_staging_arena, 0);

    m_num_indices = static_cast<uint32_t>(_entity->get_indices().size());
    uint32_t ib_size =
        static_cast<uint32_t>(_entity->get_indices().size() * sizeof(uint32_t));

    graphics::buffer index_staging = _device->create_buffer(ib_size,
        static_cast<graphics::resource_states>(
            static_cast<uint32_t>(graphics::resource_state::copy_source) |
            static_cast<uint32_t>(graphics::resource_state::host_write)));

    m_index_buffer = memory::make_unique<graphics::buffer>(_allocator,
        _device->create_buffer(ib_size,
            static_cast<graphics::resource_states>(
                static_cast<uint32_t>(graphics::resource_state::index_buffer) |
                static_cast<uint32_t>(graphics::resource_state::copy_dest))));

    m_index_buffer_memory = memory::make_unique<graphics::arena>(
        _allocator, _device->create_arena(_device_buffer_arena,
                        m_index_buffer->get_memory_requirements().size));
    graphics::arena index_staging_arena = _device->create_arena(
        _staging_buffer_arena, index_staging.get_memory_requirements().size);

    m_index_buffer->bind_memory(m_index_buffer_memory.get(), 0);
    index_staging.bind_memory(&index_staging_arena, 0);

    uint8_t* v = static_cast<uint8_t*>(vertex_staging.map());
    memcpy(v, _entity->get_vertices().data(), vb_size);
    vertex_staging.unmap();

    v = static_cast<uint8_t*>(index_staging.map());
    memcpy(v, _entity->get_indices().data(), ib_size);
    index_staging.unmap();

    m_constant_buffer = memory::make_unique<graphics::buffer>(_allocator,
        _device->create_buffer(16,
            static_cast<graphics::resource_states>(
                static_cast<uint32_t>(
                    graphics::resource_state::read_only_buffer) |
                static_cast<uint32_t>(graphics::resource_state::copy_dest))));

    graphics::buffer constant_staging = _device->create_buffer(
        16, static_cast<graphics::resource_states>(
                static_cast<uint32_t>(graphics::resource_state::copy_source) |
                static_cast<uint32_t>(graphics::resource_state::host_write)));

    m_constant_buffer_memory = memory::make_unique<graphics::arena>(
        _allocator, _device->create_arena(_device_buffer_arena,
                        m_constant_buffer->get_memory_requirements().size));
    graphics::arena constant_staging_arena = _device->create_arena(
        _staging_buffer_arena, constant_staging.get_memory_requirements().size);

    m_constant_buffer->bind_memory(m_constant_buffer_memory.get());
    constant_staging.bind_memory(&constant_staging_arena);

    float* pos = static_cast<float*>(constant_staging.map());
    pos[0] = _entity->x();
    pos[1] = _entity->y();
    pos[2] = _entity->z();
    constant_staging.unmap();

    graphics::command_list_ptr list = _command_allocator->create_command_list();
    list->transition_resource(vertex_staging,
        graphics::resource_state::host_write,
        graphics::resource_state::copy_source);
    list->transition_resource(index_staging,
        graphics::resource_state::host_write,
        graphics::resource_state::copy_source);
    list->transition_resource(constant_staging,
        graphics::resource_state::host_write,
        graphics::resource_state::copy_source);

    list->transition_resource(*m_vertex_buffer,
        graphics::resource_state::initial, graphics::resource_state::copy_dest);

    list->transition_resource(*m_index_buffer,
        graphics::resource_state::initial, graphics::resource_state::copy_dest);
    list->transition_resource(*m_constant_buffer,
        graphics::resource_state::initial, graphics::resource_state::copy_dest);

    list->copy_buffer(vertex_staging, 0, *m_vertex_buffer, 0, vb_size);
    list->copy_buffer(index_staging, 0, *m_index_buffer, 0, ib_size);
    list->copy_buffer(constant_staging, 0, *m_constant_buffer, 0, 16);

    list->transition_resource(*m_vertex_buffer,
        graphics::resource_state::copy_dest,
        graphics::resource_state::vertex_buffer);

    list->transition_resource(*m_index_buffer,
        graphics::resource_state::copy_dest,
        graphics::resource_state::index_buffer);

    list->transition_resource(*m_constant_buffer,
        graphics::resource_state::copy_dest,
        graphics::resource_state::read_only_buffer);
    list->finalize();

    _staging_queue->enqueue_command_list(list.get());
    _staging_queue->enqueue_fence(completion_fence);

    completion_fence.wait();
    _log->log_debug("Done uploading data ", _entity->get_name());
    _log->flush();

    if (_entity->diffuse()) {
      m_diffuse_texture = _textures->find(_entity->diffuse())->second.get();
    }
    if (_entity->spec()) {
      m_specular_texture = _textures->find(_entity->spec())->second.get();
    }
    if (_entity->mask()) {
      m_mask_texture = _textures->find(_entity->mask())->second.get();
    }
    if (_entity->normal()) {
      m_normal_texture = _textures->find(_entity->normal())->second.get();
    }
  }

  void setup_pipeline(graphics::graphics_pipeline* _pipeline,
      graphics::descriptor_set_layout* _layout,
      graphics::descriptor_pool* _pool, graphics::sampler* _sampler) {
    m_pipeline = _pipeline;

    m_descriptor_set = memory::make_unique<graphics::descriptor_set>(
        m_allocator, _pool->create_descriptor_set(_layout));

    graphics::buffer_descriptor buff[] = {
        {0, 0, graphics::descriptor_type::read_only_buffer,
            m_constant_buffer.get(), 0, 16, 1}};
    containers::dynamic_array<graphics::sampler_descriptor> samplers(
        m_allocator);
    containers::dynamic_array<graphics::image_descriptor> images(m_allocator);

    texture* entity::*textures[] = {&entity::m_diffuse_texture,
        &entity::m_specular_texture, &entity::m_mask_texture,
        &entity::m_normal_texture};

    uint32_t offset = 0;
    for (auto t : textures) {
      if (this->*t) {
        samplers.push_back({
            offset * 2 + 1,
            0,
            _sampler,
        });
        images.push_back({offset * 2 + 2, 0,
            graphics::descriptor_type::sampled_image,
            (this->*t)->get_image_view(), graphics::resource_state::texture});
      }
      offset += 1;
    }

    m_descriptor_set->update_descriptors(buff, images, samplers);
  }

  uint32_t get_pipeline_index() {
    uint32_t pipeline_index = 0;

    texture* entity::*textures[] = {&entity::m_diffuse_texture,
        &entity::m_specular_texture, &entity::m_mask_texture,
        &entity::m_normal_texture};

    size_t i = 1;
    for (auto t : textures) {
      if (this->*t) {
        pipeline_index |= i;
      }
      i <<= 1;
    }
    return pipeline_index;
  }

  graphics::descriptor_set* descriptor_set() {
    return m_descriptor_set.get();
  }

  graphics::buffer* vertex_buffer() {
    return m_vertex_buffer.get();
  }

  graphics::buffer* index_buffer() {
    return m_index_buffer.get();
  }

  uint32_t num_indices() {
    return m_num_indices;
  }

  uint32_t has_diffuse() {
    return m_diffuse_texture != nullptr;
  }

private:
  memory::unique_ptr<graphics::buffer> m_vertex_buffer;
  memory::unique_ptr<graphics::buffer> m_index_buffer;
  memory::unique_ptr<graphics::buffer> m_constant_buffer;

  memory::unique_ptr<graphics::arena> m_vertex_buffer_memory;
  memory::unique_ptr<graphics::arena> m_index_buffer_memory;
  memory::unique_ptr<graphics::arena> m_constant_buffer_memory;

  memory::unique_ptr<graphics::descriptor_set> m_descriptor_set;

  texture* m_diffuse_texture = nullptr;
  texture* m_specular_texture = nullptr;
  texture* m_mask_texture = nullptr;
  texture* m_normal_texture = nullptr;

  graphics::graphics_pipeline* m_pipeline = nullptr;
  memory::allocator* m_allocator;
  uint32_t m_num_indices = 0;
};

#endif  // __WN_APPLICATIONS_DEMOS_OBJECT_H__
