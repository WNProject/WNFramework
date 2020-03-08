// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_APPLICATIONS_SCENE_DEMO_H__
#define __WN_APPLICATIONS_SCENE_DEMO_H__

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNContainers/inc/WNHashSet.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/inc/WNSampler.h"
#include "WNGraphics/inc/WNSignal.h"
#include "WNGraphics/inc/WNSwapchain.h"
#include "WNMath/inc/WNMatrix.h"
#include "WNMath/inc/WNMatrixHelpers.h"
#include "WNMath/inc/WNVector.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/string_utility.h"
#include "WNWindow/inc/WNWindow.h"
#include "WNWindow/inc/WNWindowFactory.h"
#include "object.h"
#include "scene_file.h"
#include "scene_object.h"
#include "texture.h"
#include "texture_file.h"
#include "texture_manager.h"

using namespace wn;
using namespace wn::runtime;

static const uint32_t k_any_adapter = 0xFFFFFFFF;

const uint32_t k_width = 1000;
const uint32_t k_height = 1000;
const float k_scale = 100.0f;

struct global_constants {
  math::mat44f m_camera_projection;
  math::mat44f m_camera_transform;
  math::mat44f m_normal_object;
  math::vec4f m_light_position;
};

static_assert(sizeof(global_constants) == 64 + 64 + 64 + 16,
    "Unvalid global constant size");

class scene_demo {
public:
  scene_demo(memory::allocator* _allocator, logging::log* _log,
      const application::application_data* _data, texture_manager* _textures,
      scene_file* _scene, file_system::mapping* _mapping)
    : m_allocator(_allocator),
      m_adapter_factory(_allocator, _log),
      m_window_factory(_allocator, _log),
      m_system_data(_data),
      m_log(_log),
      m_texture_file_manager(_textures),
      m_scene(_scene),
      m_file_mapping(_mapping),
      m_pipelines(_allocator),
      m_textures(_allocator),
      m_entities(_allocator),
      m_swapchain_data(_allocator) {
    bool choosing_adapter = false;
    for (size_t i = 0; i < static_cast<size_t>(_data->executable_data->argc);
         ++i) {
      if (containers::string_view("--vulkan") ==
          containers::string_view(_data->executable_data->argv[i])) {
        WN_RELEASE_ASSERT(!m_force_api, "Cannot force 2 apis");
        m_forced_api = graphics::adapter::api_type::vulkan;
        m_force_api = true;
        continue;
      } else if (containers::string_view("--d3d12") ==
                 containers::string_view(_data->executable_data->argv[i])) {
        WN_RELEASE_ASSERT(!m_force_api, "Cannot force 2 apis");
        m_force_api = true;
        m_forced_api = graphics::adapter::api_type::d3d12;
      } else if (containers::string_view("--force_adapter") ==
                 containers::string_view(_data->executable_data->argv[i])) {
        choosing_adapter = true;
      } else if (choosing_adapter) {
        choosing_adapter = false;
        memory::readuint32(_data->executable_data->argv[i], m_forced_adapter,
            strnlen(_data->executable_data->argv[i], 10));
      }
    }

    for (auto& it : _textures->files()) {
      if (graphics::is_format_astc(it.second->format()) &&
          !m_features.astc_ldr_textures) {
        m_features.astc_ldr_textures = true;
        m_log->log_info(
            "Requiring a device that supports ASTC textures because ", it.first,
            " is an ASTC texture");
      } else if (graphics::is_format_bc(it.second->format()) &&
                 !m_features.bc_textures) {
        m_features.bc_textures = true;
        m_log->log_info("Requiring a device that supports BC textures because ",
            it.first, " is a BC texture");
      }
    }

    create_window();
    create_adapter_and_surface();
    create_device_and_queue();
    setup_arenas();
    determine_depth_stencil_format();

    create_command_allocator_and_textures();
    setup_objects();

    create_descriptor_pool_and_renderpass();
    create_swapchain();
    create_pipelines();
  }

  bool render_scene(bool _exit, float _time_since_last_update);

  void create_command_allocator_and_textures() {
    m_command_allocator = memory::make_unique<graphics::command_allocator>(
        m_allocator, m_device->create_command_allocator());

    for (auto& texture_file : m_texture_file_manager->files()) {
      m_textures[texture_file.second.get()] =
          memory::make_unique<texture>(m_allocator, m_allocator, m_log,
              texture_file.first, texture_file.second.get(),
              m_texture_arena_index, m_staging_buffer_arena_index,
              m_device.get(), m_queue.get(), m_command_allocator.get());
    }
  }
  uint32_t get_arena(
      std::initializer_list<const bool graphics::arena_properties::*>
          properties) {
    bool found_arena = false;
    auto arena_properties = m_device->get_arena_properties();
    uint32_t arena_index = 0;
    for (; arena_index < static_cast<uint32_t>(arena_properties.size());
         ++arena_index) {
      bool valid = true;
      for (auto& prop : properties) {
        if (!(arena_properties[arena_index].*prop)) {
          valid = false;
          break;
        }
      }
      if (!valid) {
        continue;
      }
      found_arena = true;
      break;
    }
    WN_RELEASE_ASSERT(found_arena, "Could not find arena");
    return arena_index;
  }

  void create_window() {
    multi_tasking::job_signal signal(0);
    m_window = m_window_factory.create_window(window::window_type::system,
        m_system_data->default_job_pool, &signal, m_system_data, 100, 100,
        k_width, k_height);
    WN_RELEASE_ASSERT(m_window, "Could not create window");
    // Wait until the window is ready
    signal.wait_until(1);
  }

  void create_adapter_and_surface() {
    auto adapters = m_adapter_factory.query_adapters();

    if (m_forced_adapter != k_any_adapter) {
      WN_RELEASE_ASSERT(
          m_forced_adapter < adapters.size(), "Cannot find specified adapter");
      m_adapter = adapters[m_forced_adapter].get();
      auto surface = m_adapter->make_surface(m_allocator, m_window.get());
      if (surface.second != graphics::graphics_error::ok) {
        WN_RELEASE_ASSERT(
            false, "Specified adapter could not render to the window");
      }
      if (!m_adapter->get_features().is_superset_of(m_features)) {
        WN_RELEASE_ASSERT(false,
            "Could not use requested adapter because it does not support "
            "the required texture formats");
      }
      m_surface = memory::make_unique<graphics::surface>(
          m_allocator, core::move(surface.first));
    } else {
      for (uint32_t i = 0; i < adapters.size(); ++i) {
        if (m_force_api && adapters[i]->api() != m_forced_api) {
          m_log->log_warning(
              "Skipping adapter ", i, " since it is not of the requested api");
          continue;
        }
        if (!adapters[i]->get_features().is_superset_of(m_features)) {
          m_log->log_warning("Could not use adapter ", i,
              " since it does not support the required texture formats");
          continue;
        }
        auto surface = adapters[i]->make_surface(m_allocator, m_window.get());
        if (surface.second != graphics::graphics_error::ok) {
          m_log->log_warning(
              "Could not use adapter ", i, " since it cannot render to screen");
          continue;
        }
        m_adapter = adapters[i].get();
        m_surface = memory::make_unique<graphics::surface>(
            m_allocator, core::move(surface.first));
        break;
      }
    }
    WN_RELEASE_ASSERT(m_surface != nullptr, "Could not find a valid surface");
    m_log->log_info("Selected adapter: ", m_adapter->name());
    m_log->log_info("Selected API: ",
        m_adapter->api() == graphics::adapter::api_type::vulkan ? "vulkan"
                                                                : "d3d12");
  }

  void create_device_and_queue() {
    m_device = m_adapter->make_device(m_allocator, m_log, m_features);
    WN_RELEASE_ASSERT(m_device, "Could not create device");
    m_queue = m_device->create_queue();
    WN_RELEASE_ASSERT(m_queue, "Could not create queue");
  }

  void determine_depth_stencil_format() {
    auto& formats = m_adapter->get_formats();
    if (formats.has_d32_float) {
      m_depth_stencil_format = graphics::data_format::d32_float;
      m_depth_buffer_arena_index =
          get_arena({&graphics::arena_properties::allow_d32_f});
    } else if (formats.has_d24_unorm) {
      m_depth_stencil_format = graphics::data_format::d24_unorm;
      m_depth_buffer_arena_index =
          get_arena({&graphics::arena_properties::allow_d24_u});
    } else {
      WN_RELEASE_ASSERT(false, "Could not find a viable depth stencil format");
    }
  }

  void create_swapchain() {
    const graphics::swapchain_create_info create_info = {
        graphics::data_format::r8g8b8a8_unorm, 2,
        graphics::swap_mode::immediate, graphics::discard_policy::discard};

    m_swapchain =
        m_device->create_swapchain(*m_surface, create_info, m_queue.get());
    m_swapchain_data.reserve(m_swapchain->info().num_buffers);
    for (uint32_t i = 0; i < m_swapchain->info().num_buffers; ++i) {
      // This is gross, but we need to do more work to get dynamic_array
      // to be resizable with move-only types.
      m_swapchain_data.push_back(per_swapchain_image_data());
      m_swapchain_data[i].m_swapchain_view =
          memory::make_unique<graphics::image_view>(m_allocator,
              m_device->create_image_view(m_swapchain->get_image_for_index(i),
                  0, 1,
                  static_cast<graphics::image_components>(
                      graphics::image_component::color)));

      graphics::image_create_info info = {k_width, k_height,
          m_depth_stencil_format,
          graphics::resource_states(
              static_cast<uint32_t>(graphics::resource_state::depth_target)),
          1};

      m_swapchain_data[i].m_depth_buffer = memory::make_unique<graphics::image>(
          m_allocator, m_device->create_image(info, graphics::clear_value{}));

      auto reqs = m_swapchain_data[i].m_depth_buffer->get_memory_requirements();
      m_swapchain_data[i].m_depth_arena =
          memory::make_unique<graphics::arena>(m_allocator,
              m_device->create_arena(m_depth_buffer_arena_index, reqs.size));
      m_swapchain_data[i].m_depth_buffer->bind_memory(
          m_swapchain_data[i].m_depth_arena.get(), 0);

      m_swapchain_data[i].m_depth_stencil_view =
          memory::make_unique<graphics::image_view>(
              m_allocator, m_device->create_image_view(
                               m_swapchain_data[i].m_depth_buffer.get(), 0, 1,
                               static_cast<graphics::image_components>(
                                   graphics::image_component::depth)));

      const graphics::image_view* views[] = {
          m_swapchain_data[i].m_swapchain_view.get(),
          m_swapchain_data[i].m_depth_stencil_view.get()};

      graphics::framebuffer_create_info framebuffer_create = {
          m_render_pass.get(),
          containers::contiguous_range<const graphics::image_view*>(views),
          k_width, k_height, 1};

      m_swapchain_data[i].m_swapchain_framebuffer =
          memory::make_unique<graphics::framebuffer>(
              m_allocator, m_device->create_framebuffer(framebuffer_create));
    }
  }

  void setup_arenas() {
    m_texture_arena_index =
        get_arena({&graphics::arena_properties::allow_images,
            &graphics::arena_properties::device_local});
    m_staging_buffer_arena_index =
        get_arena({&graphics::arena_properties::allow_buffers,
            &graphics::arena_properties::host_visible});
    m_device_buffer_arena_index =
        get_arena({&graphics::arena_properties::allow_buffers,
            &graphics::arena_properties::device_local});
    m_constant_buffer_arena_index =
        get_arena({&graphics::arena_properties::allow_buffers,
            &graphics::arena_properties::host_visible});
  }

  void setup_objects() {
    for (auto& obj : m_scene->entities()) {
      m_entities[obj.first] = memory::make_unique<entity>(m_allocator,
          m_allocator, m_log, m_device.get(), m_command_allocator.get(),
          m_queue.get(), m_device_buffer_arena_index,
          m_staging_buffer_arena_index, obj.second.get(), &m_textures);
    }

    m_camera_matrix = math::get_translation(200.0f, -200.0f, -100.0f);

    for (size_t i = 0; i < 2; ++i) {
      m_frame_data[i].m_global_constants =
          memory::make_unique<graphics::buffer>(m_allocator,
              m_device->create_buffer(sizeof(global_constants),
                  static_cast<uint32_t>(graphics::resource_states(
                      static_cast<uint32_t>(
                          graphics::resource_state::read_only_buffer) |
                      static_cast<uint32_t>(
                          graphics::resource_state::host_write)))));

      m_frame_data[i].m_global_constant_arena =
          memory::make_unique<graphics::arena>(m_allocator,
              m_device->create_arena(m_constant_buffer_arena_index,
                  m_frame_data[i]
                      .m_global_constants->get_memory_requirements()
                      .size));

      m_frame_data[i].m_global_constants->bind_memory(
          m_frame_data[i].m_global_constant_arena.get(), 0);

      m_frame_data[i].m_frame_start_fence =
          memory::make_unique<graphics::fence>(
              m_allocator, m_device->create_fence());
      m_frame_data[i].m_present_image_ready_signal =
          memory::make_unique<graphics::signal>(
              m_allocator, m_device->create_signal());
      m_frame_data[i].m_rendering_done_signal =
          memory::make_unique<graphics::signal>(
              m_allocator, m_device->create_signal());
      m_frame_data[i].m_command_allocator =
          memory::make_unique<graphics::command_allocator>(
              m_allocator, m_device->create_command_allocator());
    }
  }

  void create_descriptor_pool_and_renderpass() {
    // In our current setup we have as many as:
    // 1 constant_buffer per object
    // 4 sampler per object
    // 4 sampled image per object
    // Let's leave space to grow:
    //  3 constant buffers per object
    //  6 samplers / sampled images per object
    const graphics::descriptor_pool_create_info pool_infos[] = {
        {3 * m_entities.size(), graphics::descriptor_type::read_only_buffer},
        {6 * m_entities.size(), graphics::descriptor_type::sampler},
        {6 * m_entities.size(), graphics::descriptor_type::sampled_image},
    };
    m_descriptor_pool = memory::make_unique<graphics::descriptor_pool>(
        m_allocator, m_device->create_descriptor_pool(pool_infos));

    // The "master" descriptor set layout. This one will be a super-set
    // of all other descriptor set layouts
    const graphics::descriptor_binding_info infos[] = {
        {
            0,  // binding
            0,  // register_base
            1,  // array_size
            static_cast<uint32_t>(graphics::shader_stage::vertex),
            graphics::descriptor_type::read_only_buffer,  // type
        },
        {
            1,  // binding
            0,  // register_base
            1,  // array_size
            static_cast<uint32_t>(graphics::shader_stage::pixel),
            graphics::descriptor_type::sampler,  // type
        },
        {
            2,  // binding
            0,  // register_base
            1,  // array_size
            static_cast<uint32_t>(graphics::shader_stage::pixel),
            graphics::descriptor_type::sampled_image,  // type
        },
        {
            3,  // binding
            1,  // register_base
            1,  // array_size
            static_cast<uint32_t>(graphics::shader_stage::pixel),
            graphics::descriptor_type::sampler,  // type
        },
        {
            4,  // binding
            1,  // register_base
            1,  // array_size
            static_cast<uint32_t>(graphics::shader_stage::pixel),
            graphics::descriptor_type::sampled_image,  // type
        },
        {
            5,  // binding
            2,  // register_base
            1,  // array_size
            static_cast<uint32_t>(graphics::shader_stage::pixel),
            graphics::descriptor_type::sampler,  // type
        },
        {
            6,  // binding
            2,  // register_base
            1,  // array_size
            static_cast<uint32_t>(graphics::shader_stage::pixel),
            graphics::descriptor_type::sampled_image,  // type
        }};

    m_main_layout = memory::make_unique<graphics::descriptor_set_layout>(
        m_allocator, m_device->create_descriptor_set_layout(infos));

    const graphics::descriptor_binding_info camera_infos[] = {{
        0,   // binding
        20,  // register_base
        1,   // array_size
        static_cast<uint32_t>(graphics::shader_stage::vertex) |
            static_cast<uint32_t>(graphics::shader_stage::pixel),
        graphics::descriptor_type::read_only_buffer,  // type
    }};

    m_global_set_layout = memory::make_unique<graphics::descriptor_set_layout>(
        m_allocator, m_device->create_descriptor_set_layout(camera_infos));

    for (size_t i = 0; i < 2; ++i) {
      m_frame_data[i].m_global_set =
          memory::make_unique<graphics::descriptor_set>(
              m_allocator, m_descriptor_pool->create_descriptor_set(
                               m_global_set_layout.get()));
    }

    graphics::render_pass_attachment attachment[] = {{}, {}};
    attachment[0].attachment_load_op = graphics::load_op::clear;

    attachment[1].attachment_load_op = graphics::load_op::clear;
    attachment[1].attachment_store_op = graphics::store_op::dont_care;
    attachment[1].format = m_depth_stencil_format;
    attachment[1].stencil_load_op = graphics::load_op::dont_care;
    attachment[1].stencil_store_op = graphics::store_op::dont_care;
    attachment[1].initial_state = graphics::resource_state::depth_target;
    attachment[1].final_state = graphics::resource_state::depth_target;

    const graphics::attachment_reference color_attachments[] = {{}};
    graphics::attachment_reference depth_attachment = {};
    depth_attachment.attachment = 1;
    depth_attachment.state = graphics::resource_state::depth_target;

    graphics::subpass_description subpasses[] = {{}};
    subpasses[0].color_attachments = color_attachments;
    subpasses[0].depth_stencil = &depth_attachment;

    m_render_pass = memory::make_unique<graphics::render_pass>(m_allocator,
        m_device->create_render_pass(attachment, subpasses, nullptr));

    graphics::sampler_create_info s;
    s.min = graphics::sampler_filter::linear;
    s.mag = graphics::sampler_filter::linear;
    s.max_lod = 32.0f;
    m_linear_sampler = memory::make_unique<graphics::sampler>(
        m_allocator, m_device->create_sampler(s));
  }

  void create_pipelines() {
    file_system::result res;
    file_system::file_ptr vertex_shader = m_file_mapping->open_file(
        m_adapter->api() == graphics::adapter::api_type::vulkan ? "vertex.spv"
                                                                : "vertex.dxbc",
        res);

    WN_RELEASE_ASSERT(
        res == file_system::result::ok, "Error could not load vertex shader");

    m_vertex_shader_module = memory::make_unique<graphics::shader_module>(
        m_allocator,
        m_device->create_shader_module(vertex_shader->typed_range<uint8_t>()));

    for (auto& obj : m_entities) {
      uint32_t pipeline_index = obj.second->get_pipeline_index();

      auto pipeline_it = m_pipelines.find(pipeline_index);
      if (pipeline_it != m_pipelines.end()) {
        obj.second->setup_pipeline(&pipeline_it->second->m_pipeline,
            &pipeline_it->second->m_descriptor_layout, m_descriptor_pool.get(),
            m_linear_sampler.get());
        continue;
      }

      containers::string file(m_allocator, "pixel_");
      char tempBuffer[17] = {};
      memory::writeuint32(tempBuffer, pipeline_index, 16);
      file += tempBuffer;
      if (m_adapter->api() == graphics::adapter::api_type::vulkan) {
        file += ".spv";
      } else {
        file += ".dxbc";
      }

      file_system::result file_res;
      file_system::file_ptr pixel_shader =
          m_file_mapping->open_file(file, file_res);

      WN_RELEASE_ASSERT(file_res == file_system::result::ok,
          "Error could not load pixel shader");

      auto shader_module =
          m_device->create_shader_module(pixel_shader->typed_range<uint8_t>());

      containers::dynamic_array<graphics::descriptor_binding_info> descriptors(
          m_allocator);
      descriptors.reserve(5);
      descriptors.push_back({
          0,  // binding
          0,  // register_base
          1,  // array_size
          static_cast<uint32_t>(graphics::shader_stage::vertex),
          graphics::descriptor_type::read_only_buffer,  // type
      });

      size_t index = pipeline_index;

      for (size_t i = 0; index; index >>= 1, ++i) {
        if ((index & 0x1) == 0) {
          continue;
        }
        descriptors.push_back({
            1 + (i * 2),  // binding
            i,            // register_base
            1,            // array_size
            static_cast<uint32_t>(graphics::shader_stage::pixel),
            graphics::descriptor_type::sampler,  // type
        });

        descriptors.push_back({
            2 + (i * 2),  // binding
            i,            // register_base
            1,            // array_size
            static_cast<uint32_t>(graphics::shader_stage::pixel),
            graphics::descriptor_type::sampled_image,  // type
        });
      }

      graphics::descriptor_set_layout descriptor_layout =
          m_device->create_descriptor_set_layout(containers::contiguous_range<
              const graphics::descriptor_binding_info>(
              descriptors.data(), descriptors.size()));

      const graphics::descriptor_set_layout* layouts[] = {
          &descriptor_layout, m_global_set_layout.get()};

      graphics::pipeline_layout layout =
          m_device->create_pipeline_layout({layouts}, {});

      graphics::graphics_pipeline pipeline = m_device->create_graphics_pipeline(
          graphics::graphics_pipeline_description(m_allocator)
              .add_vertex_stream(0, sizeof(packed_vertex),
                  graphics::stream_frequency::per_vertex)
              .add_vertex_attribute(
                  0, 0, "POSITION", 0, graphics::data_format::r32g32b32_sfloat)
              .add_vertex_attribute(
                  0, 1, "NORMAL", 12, graphics::data_format::r32g32b32_sfloat)
              .add_vertex_attribute(
                  0, 2, "TEXCOORD", 24, graphics::data_format::r32g32_sfloat)
              .add_vertex_attribute(0, 3, "TANGENT", 32,
                  graphics::data_format::r32g32b32a32_sfloat)
              .add_color_output(0)
              .set_shader(graphics::shader_stage::vertex,
                  m_vertex_shader_module.get(), "main")
              .set_shader(graphics::shader_stage::pixel, &shader_module, "main")
              .set_cull_mode(graphics::cull_mode::front)
              .set_static_scissor(0, graphics::scissor{0, 0, k_width, k_height})
              .set_depth_buffer_enabled(true)
              .set_depth(true, true)
              .set_static_viewport(
                  0, graphics::viewport{0, 0, static_cast<float>(k_width),
                         static_cast<float>(k_height), 0, 1}),
          &layout, m_render_pass.get(), 0);
      auto it = m_pipelines.insert(core::move(pipeline_index),
          memory::make_unique<pipeline_data>(m_allocator,
              pipeline_data(core::move(shader_module), core::move(pipeline),
                  core::move(layout), core::move(descriptor_layout))));
      obj.second->setup_pipeline(&it.first->second->m_pipeline,
          &it.first->second->m_descriptor_layout, m_descriptor_pool.get(),
          m_linear_sampler.get());
    }

    for (size_t i = 0; i < 2; ++i) {
      graphics::buffer_descriptor buff[] = {
          {0, 0, graphics::descriptor_type::read_only_buffer,
              m_frame_data[i].m_global_constants.get(), 0,
              sizeof(global_constants), 1}};

      m_frame_data[i].m_global_set->update_descriptors(buff, nullptr, nullptr);
    }
  }

private:
  memory::allocator* m_allocator;
  graphics::factory m_adapter_factory;
  graphics::adapter* m_adapter;
  graphics::device_ptr m_device;
  graphics::queue_ptr m_queue;
  memory::unique_ptr<graphics::command_allocator> m_command_allocator;

  uint32_t m_texture_arena_index = 0;
  uint32_t m_staging_buffer_arena_index = 0;
  uint32_t m_device_buffer_arena_index = 0;
  uint32_t m_constant_buffer_arena_index = 0;
  uint32_t m_depth_buffer_arena_index = 0;

  memory::unique_ptr<graphics::surface> m_surface;
  graphics::swapchain_ptr m_swapchain;

  window::window_factory m_window_factory;
  memory::unique_ptr<window::window> m_window;
  const application::application_data* m_system_data;
  logging::log* m_log;

  bool m_force_api = false;
  graphics::adapter::api_type m_forced_api =
      graphics::adapter::api_type::invalid;
  uint32_t m_forced_adapter = k_any_adapter;
  graphics::adapter_features m_features = {};

  texture_manager* m_texture_file_manager;
  scene_file* m_scene;
  file_system::mapping* m_file_mapping;

  memory::unique_ptr<graphics::descriptor_pool> m_descriptor_pool;
  memory::unique_ptr<graphics::render_pass> m_render_pass;
  memory::unique_ptr<graphics::descriptor_set_layout> m_main_layout;
  memory::unique_ptr<graphics::sampler> m_linear_sampler;
  memory::unique_ptr<graphics::descriptor_set_layout> m_global_set_layout;
  graphics::data_format m_depth_stencil_format = graphics::data_format::max;
  math::mat44f m_camera_matrix;

  struct pipeline_data {
    pipeline_data(pipeline_data&& _other)
      : m_pixel_shader(core::move(_other.m_pixel_shader)),
        m_pipeline(core::move(_other.m_pipeline)),
        m_pipeline_layout(core::move(_other.m_pipeline_layout)),
        m_descriptor_layout(core::move(_other.m_descriptor_layout)) {}
    pipeline_data(graphics::shader_module _pixel_shader,
        graphics::graphics_pipeline _pipeline,
        graphics::pipeline_layout _pipeline_layout,
        graphics::descriptor_set_layout _descriptor_layout)
      : m_pixel_shader(core::move(_pixel_shader)),
        m_pipeline(core::move(_pipeline)),
        m_pipeline_layout(core::move(_pipeline_layout)),
        m_descriptor_layout(core::move(_descriptor_layout)) {}

    graphics::shader_module m_pixel_shader;
    graphics::graphics_pipeline m_pipeline;
    graphics::pipeline_layout m_pipeline_layout;
    graphics::descriptor_set_layout m_descriptor_layout;
  };

  containers::hash_map<uint32_t, memory::unique_ptr<pipeline_data>> m_pipelines;
  memory::unique_ptr<graphics::shader_module> m_vertex_shader_module;

  containers::hash_map<const texture_file*, memory::unique_ptr<texture>>
      m_textures;
  containers::hash_map<containers::string_view, memory::unique_ptr<entity>>
      m_entities;

  struct per_frame_data {
    memory::unique_ptr<graphics::fence> m_frame_start_fence;
    memory::unique_ptr<graphics::signal> m_present_image_ready_signal;
    memory::unique_ptr<graphics::signal> m_rendering_done_signal;

    memory::unique_ptr<graphics::descriptor_set> m_global_set;
    memory::unique_ptr<graphics::buffer> m_global_constants;
    memory::unique_ptr<graphics::arena> m_global_constant_arena;

    graphics::command_list_ptr m_command_list;
    memory::unique_ptr<graphics::command_allocator> m_command_allocator;
    global_constants* m_global_constants_ptr;
  };

  struct per_swapchain_image_data {
    per_swapchain_image_data() {}
    per_swapchain_image_data(per_swapchain_image_data&& _other)
      : m_swapchain_view(core::move(_other.m_swapchain_view)),
        m_swapchain_framebuffer(core::move(_other.m_swapchain_framebuffer)) {}

    memory::unique_ptr<graphics::image_view> m_swapchain_view;
    memory::unique_ptr<graphics::framebuffer> m_swapchain_framebuffer;
    memory::unique_ptr<graphics::image> m_depth_buffer;
    memory::unique_ptr<graphics::arena> m_depth_arena;
    memory::unique_ptr<graphics::image_view> m_depth_stencil_view;
    bool m_seen = false;
  };
  containers::dynamic_array<per_swapchain_image_data> m_swapchain_data;

  per_frame_data m_frame_data[2];

  uint32_t m_frame_number = 0;
  float m_frame_accumulator = 0.f;
  uint32_t m_frames_time_counter = 0;
  float m_time = 0.f;

  bool m_last_mouse_pressed = false;
  uint32_t m_last_mouse_x = 0;
  uint32_t m_last_mouse_y = 0;

  float m_yaw = 0;
  float m_pitch = 0;
};

#endif  // __WN_APPLICATIONS_SCENE_DEMO_H__
