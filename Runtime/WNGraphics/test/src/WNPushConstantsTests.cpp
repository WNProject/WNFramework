// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/inc/WNFactory.h"
#include "WNGraphics/test/inc/WNPixelTestFixture.h"
#include "WNGraphics/test/inc/push_constants_test.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNMultiTasking/inc/job_signal.h"
#include "WNWindow/inc/WNWindow.h"
#include "WNWindow/inc/WNWindowFactory.h"

using push_constant_test = wn::runtime::graphics::testing::pixel_test<>;

TEST_F(push_constant_test, basic) {
  wn::file_system::factory fs_factory(&m_allocator,
      wn::testing::k_executable_data, wn::logging::get_null_logger());
  auto files = fs_factory.make_mapping(
      &m_allocator, wn::file_system::mapping_type::memory_backed);
  files->initialize_files(push_constants_test::get_files());

  run_test(wn::runtime::graphics::k_empty_adapter_features,
      [this, &files](wn::runtime::graphics::adapter::api_type _api,
          wn::runtime::graphics::device* _device,
          wn::runtime::graphics::queue* _queue,
          wn::runtime::graphics::image* _image) {
        wn::runtime::graphics::command_allocator alloc =
            _device->create_command_allocator();
        wn::file_system::result res;

        wn::file_system::file_ptr vertex_shader = files->open_file(
            _api == wn::runtime::graphics::adapter::api_type::vulkan
                ? "push_constants/built_shaders/pipeline.vs.spv"
                : "push_constants/built_shaders/pipeline.vs.dxbc",
            res);
        ASSERT_EQ(wn::file_system::result::ok, res);
        wn::file_system::file_ptr pixel_shader = files->open_file(
            _api == wn::runtime::graphics::adapter::api_type::vulkan
                ? "push_constants/built_shaders/pipeline.ps.spv"
                : "push_constants/built_shaders/pipeline.ps.dxbc",
            res);
        ASSERT_EQ(wn::file_system::result::ok, res);

        wn::runtime::graphics::shader_module vs = _device->create_shader_module(
            vertex_shader->typed_range<uint8_t>());

        wn::runtime::graphics::shader_module ps =
            _device->create_shader_module(pixel_shader->typed_range<uint8_t>());
        wn::runtime::graphics::push_constant_range constants[] = {
            {static_cast<uint32_t>(wn::runtime::graphics::shader_stage::vertex),
                0, 2, 0}};

        wn::runtime::graphics::pipeline_layout layout =
            _device->create_pipeline_layout({}, constants);

        wn::runtime::graphics::render_pass_attachment attachment[] = {{}};
        attachment[0].attachment_load_op =
            wn::runtime::graphics::load_op::clear;

        const wn::runtime::graphics::attachment_reference color_attachments[] =
            {{}};
        wn::runtime::graphics::subpass_description subpasses[] = {{}};
        subpasses[0].color_attachments = color_attachments;
        wn::runtime::graphics::render_pass render_pass =
            _device->create_render_pass(attachment, subpasses, nullptr);

        const wn::containers::contiguous_range<
            const wn::runtime::graphics::arena_properties>
            arena_properties = _device->get_arena_properties();
        size_t arena_index = 0;
        bool found_arena = false;

        for (; arena_index < arena_properties.size(); ++arena_index) {
          if (arena_properties[arena_index].host_visible &&
              arena_properties[arena_index].allow_buffers) {
            found_arena = true;
            break;
          }
        }

        ASSERT_TRUE(found_arena);

        const size_t vertex_size = 8 * sizeof(float);
        const size_t num_verts = 3;

        const size_t size = vertex_size * num_verts;

        wn::runtime::graphics::buffer buffer = _device->create_buffer(size,
            static_cast<wn::runtime::graphics::resource_states>(
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::host_write) |
                static_cast<uint32_t>(
                    wn::runtime::graphics::resource_state::vertex_buffer)));

        wn::runtime::graphics::buffer_memory_requirements buffer_reqs =
            buffer.get_memory_requirements();

        wn::runtime::graphics::arena arena =
            _device->create_arena(arena_index, buffer_reqs.size);
        ASSERT_TRUE(buffer.bind_memory(&arena));
        void* raw_memory = buffer.map();
        float* memory = static_cast<float*>(raw_memory);
        // Vertex 1 <pos>
        memory[0] = 0.0f;
        memory[1] = 0.5;
        memory[2] = 0.0f;
        memory[3] = 1.0f;
        // Vertex 1 <Texcoord>
        memory[4] = 0.0f;
        memory[5] = 0.0f;
        memory[6] = 1.0f;
        memory[7] = 0.0f;

        // Vertex 2 <pos>
        memory[8] = 0.5f;
        memory[9] = -0.5f;
        memory[10] = 0.0f;
        memory[11] = 1.0f;
        // Vertex 2 <texcoord>
        memory[12] = 0.0f;
        memory[13] = 1.0f;
        memory[14] = 0.0f;
        memory[15] = 0.0f;

        // Vertex 3 <pos>
        memory[16] = -0.5f;
        memory[17] = -0.5f;
        memory[18] = 0.0f;
        memory[19] = 1.0f;
        // Vertex 3 <texcoord>
        memory[20] = 1.0f;
        memory[21] = 0.0f;
        memory[22] = 0.0f;
        memory[23] = 0.0f;

        buffer.unmap();

        wn::runtime::graphics::command_list_ptr setup_command_list =
            alloc.create_command_list();
        setup_command_list->transition_resource(buffer,
            wn::runtime::graphics::resource_state::host_write,
            wn::runtime::graphics::resource_state::vertex_buffer);
        setup_command_list->finalize();
        _queue->enqueue_command_list(setup_command_list.get());

        wn::runtime::graphics::graphics_pipeline pipeline =
            _device->create_graphics_pipeline(
                wn::runtime::graphics::graphics_pipeline_description(
                    &m_allocator)
                    .add_vertex_stream(0, vertex_size,
                        wn::runtime::graphics::stream_frequency::per_vertex)
                    .add_vertex_attribute(0, 0, "POSITION", 0,
                        wn::runtime::graphics::data_format::r32g32b32a32_sfloat)
                    .add_vertex_attribute(0, 1, "TEXCOORD", 16,
                        wn::runtime::graphics::data_format::r32g32b32a32_sfloat)
                    .add_color_output(0)
                    .set_shader(wn::runtime::graphics::shader_stage::vertex,
                        &vs, "main")
                    .set_shader(
                        wn::runtime::graphics::shader_stage::pixel, &ps, "main")
                    .set_static_scissor(0, wn::runtime::graphics::scissor{0, 0,
                                               get_width(), get_height()})
                    .set_static_viewport(
                        0, wn::runtime::graphics::viewport{0, 0,
                               static_cast<float>(get_width()),
                               static_cast<float>(get_height()), 0, 1}),
                &layout, &render_pass, 0);

        wn::runtime::graphics::image_view image_view =
            _device->create_image_view(_image, 0, 1,
                static_cast<wn::runtime::graphics::image_components>(
                    wn::runtime::graphics::image_component::color));

        const wn::runtime::graphics::image_view* views[] = {&image_view};

        wn::runtime::graphics::framebuffer_create_info framebuffer_create = {
            &render_pass,
            wn::containers::contiguous_range<
                const wn::runtime::graphics::image_view*>(views),
            get_width(), get_height(), 1};

        wn::runtime::graphics::framebuffer framebuffer =
            _device->create_framebuffer(framebuffer_create);

        wn::runtime::graphics::command_list_ptr list =
            alloc.create_command_list();

        wn::runtime::graphics::clear_value clear{};
        clear.color.float_vals[0] = 0.1f;
        clear.color.float_vals[1] = 0.1f;
        clear.color.float_vals[2] = 0.1f;
        clear.color.float_vals[3] = 1.0f;
        list->begin_render_pass(&render_pass, &framebuffer,
            {0, 0, get_width(), get_height()},
            wn::containers::contiguous_range<
                wn::runtime::graphics::clear_value>(&clear, 1));
        list->bind_graphics_pipeline_layout(&layout);
        list->bind_graphics_pipeline(&pipeline);
        list->bind_vertex_buffer(0, &buffer);
        float c[2] = {-0.25f, 0.25f};
        list->push_graphics_contants(
            0, 0, reinterpret_cast<uint32_t*>(&c[0]), 2);
        list->draw(3, 1, 0, 0);
        c[0] = 0.25f;
        c[1] = -0.25f;
        list->push_graphics_contants(
            0, 0, reinterpret_cast<uint32_t*>(&c[0]), 2);
        list->draw(3, 1, 0, 0);
        list->end_render_pass();
        list->finalize();
        _queue->enqueue_command_list(list.get());
        wn::runtime::graphics::fence completion_fence = _device->create_fence();
        _queue->enqueue_fence(completion_fence);
        completion_fence.wait();
      });
}
