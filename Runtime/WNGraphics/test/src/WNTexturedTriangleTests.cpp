// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNGraphics/test/inc/WNPixelTestFixture.h"

#include "WNGraphics/inc/WNSampler.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNMultiTasking/inc/WNJobSignal.h"
#include "WNWindow/inc/WNWindow.h"
#include "WNWindow/inc/WNWindowFactory.h"

namespace vertex_shader {
const uint32_t vulkan[] =
#include "WNGraphics/test/shaders/textured_pipeline.vs.glsl.h"
    ;
const size_t size_vulkan = sizeof(vulkan);

#ifndef BYTE
#define BYTE uint8_t
#endif

#include "WNGraphics/test/shaders/textured_pipeline.vs.hlsl.h"
const BYTE* d3d12 = g_main;
const size_t size_d3d12 = sizeof(g_main);
};  // namespace vertex_shader

namespace pixel_shader {
const uint32_t vulkan[] =
#include "WNGraphics/test/shaders/textured_pipeline.ps.glsl.h"
    ;
const size_t size_vulkan = sizeof(vulkan);

#include "WNGraphics/test/shaders/textured_pipeline.ps.hlsl.h"
const BYTE* d3d12 = g_main;
const size_t size_d3d12 = sizeof(g_main);
};  // namespace pixel_shader

using textured_triangle_test = wn::runtime::graphics::testing::pixel_test<100,
    100, wn::runtime::graphics::testing::fuzzy_comparator<1, 100, 55>>;

TEST_F(textured_triangle_test, basic) {
  run_test([this](wn::runtime::graphics::adapter::api_type _api,
               wn::runtime::graphics::device* _device,
               wn::runtime::graphics::queue* _queue,
               wn::runtime::graphics::image* _image) {
    wn::runtime::graphics::command_allocator alloc =
        _device->create_command_allocator();

    wn::runtime::graphics::shader_module vs = _device->create_shader_module(
        _api == wn::runtime::graphics::adapter::api_type::vulkan
            ? wn::containers::contiguous_range<const uint8_t>(
                  reinterpret_cast<const uint8_t*>(vertex_shader::vulkan),
                  vertex_shader::size_vulkan)
            : wn::containers::contiguous_range<const uint8_t>(
                  reinterpret_cast<const uint8_t*>(vertex_shader::d3d12),
                  vertex_shader::size_d3d12));

    wn::runtime::graphics::shader_module ps = _device->create_shader_module(
        _api == wn::runtime::graphics::adapter::api_type::vulkan
            ? wn::containers::contiguous_range<const uint8_t>(
                  reinterpret_cast<const uint8_t*>(pixel_shader::vulkan),
                  pixel_shader::size_vulkan)
            : wn::containers::contiguous_range<const uint8_t>(
                  reinterpret_cast<const uint8_t*>(pixel_shader::d3d12),
                  pixel_shader::size_d3d12));

    const wn::runtime::graphics::descriptor_binding_info infos[] = {
        {
            0,  // binding
            0,  // register_base
            1,  // array_size
            static_cast<uint32_t>(wn::runtime::graphics::shader_stage::vertex),
            wn::runtime::graphics::descriptor_type::read_only_buffer,  // type
        },
        {
            1,  // binding
            0,  // register_base
            1,  // array_size
            static_cast<uint32_t>(wn::runtime::graphics::shader_stage::pixel),
            wn::runtime::graphics::descriptor_type::sampler,  // type
        },
        {
            2,  // binding
            0,  // register_base
            1,  // array_size
            static_cast<uint32_t>(wn::runtime::graphics::shader_stage::pixel),
            wn::runtime::graphics::descriptor_type::sampled_image,  // type
        }};

    wn::runtime::graphics::descriptor_set_layout descriptor_layout =
        _device->create_descriptor_set_layout(infos);

    const wn::runtime::graphics::descriptor_pool_create_info pool_infos[] = {
        {1, wn::runtime::graphics::descriptor_type::read_only_buffer},
        {1, wn::runtime::graphics::descriptor_type::sampler},
        {1, wn::runtime::graphics::descriptor_type::sampled_image},
    };
    wn::runtime::graphics::descriptor_pool pool =
        _device->create_descriptor_pool(pool_infos);
    wn::runtime::graphics::descriptor_set set =
        pool.create_descriptor_set(&descriptor_layout);

    const wn::runtime::graphics::descriptor_set_layout* layouts[] = {
        &descriptor_layout};

    wn::runtime::graphics::pipeline_layout layout =
        _device->create_pipeline_layout({layouts}, {});

    wn::runtime::graphics::render_pass_attachment attachment[] = {{}};
    attachment[0].attachment_load_op = wn::runtime::graphics::load_op::clear;

    const wn::runtime::graphics::attachment_reference color_attachments[] = {
        {}};
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
    found_arena = false;
    size_t texture_arena_index = 0;
    for (; texture_arena_index < arena_properties.size();
         ++texture_arena_index) {
      if (arena_properties[texture_arena_index].allow_images) {
        found_arena = true;
        break;
      }
    }
    ASSERT_TRUE(found_arena);

    float triangle[] = {
        0.0f,
        0.5,
        // Vertex 1 <Texcoord>
        0.0f,
        0.0f,

        // Vertex 3 <pos>
        -0.5f,
        -0.5f,
        // Vertex 3 <texcoord>
        1.0f,
        0.0f,

        // Vertex 2 <pos>
        0.5f,
        -0.5f,
        // Vertex 2 <texcoord>
        0.0f,
        1.0f,

    };
    auto vertex_buffer = wn::runtime::graphics::testing::create_and_fill_buffer(
        _device, arena_index,
        wn::runtime::graphics::resource_state::vertex_buffer, triangle);
    float constants[] = {0.25f, -0.25f};
    auto constant_buffer =
        wn::runtime::graphics::testing::create_and_fill_buffer(_device,
            arena_index,
            wn::runtime::graphics::resource_state::read_only_buffer, constants);
    uint32_t indices[] = {0, 2, 1};
    auto index_buffer = wn::runtime::graphics::testing::create_and_fill_buffer(
        _device, arena_index,
        wn::runtime::graphics::resource_state::index_buffer, indices);

    wn::runtime::graphics::image_create_info info = {16, 16,
        wn::runtime::graphics::data_format::r8g8b8a8_unorm,
        wn::runtime::graphics::resource_states(
            static_cast<uint32_t>(
                wn::runtime::graphics::resource_state::copy_dest) |
            static_cast<uint32_t>(
                wn::runtime::graphics::resource_state::texture))};
    wn::runtime::graphics::image texture =
        _device->create_image(info, wn::runtime::graphics::clear_value{});
    auto reqs = texture.get_memory_requirements();
    wn::runtime::graphics::arena mem =
        _device->create_arena(texture_arena_index, reqs.size);
    texture.bind_memory(&mem, 0);

    const wn::runtime::graphics::image::image_buffer_resource_info&
        resource_info = texture.get_buffer_requirements();

    auto image_upload_buffer =
        wn::runtime::graphics::testing::create_buffer(_device, arena_index,
            wn::runtime::graphics::resource_state::copy_source,
            static_cast<uint32_t>(resource_info.total_memory_required));

    uint32_t* v = static_cast<uint32_t*>(image_upload_buffer.buffer.map());
    const uint32_t row_pitch_in_words =
        static_cast<uint32_t>(resource_info.row_pitch_in_bytes / 4);
    for (uint32_t i = 0; i < 16; ++i) {
      for (uint32_t j = 0; j < 16; ++j) {
        v[i * row_pitch_in_words + j] = ((i * 16 + j) << 8 | 0xFF);
      }
    }
    image_upload_buffer.buffer.unmap();

    wn::runtime::graphics::image_view texture_view = _device->create_image_view(
        &texture, static_cast<wn::runtime::graphics::image_components>(
                      wn::runtime::graphics::image_component::color));
    wn::runtime::graphics::sampler_create_info s;
    s.min = wn::runtime::graphics::sampler_filter::linear;
    s.mag = wn::runtime::graphics::sampler_filter::linear;
    wn::runtime::graphics::sampler sampler = _device->create_sampler(s);

    wn::runtime::graphics::buffer_descriptor buff[] = {
        {0, 0, wn::runtime::graphics::descriptor_type::read_only_buffer,
            &constant_buffer.buffer, 0, 4, 2}};
    wn::runtime::graphics::sampler_descriptor samp[] = {{1, 0, &sampler}};
    wn::runtime::graphics::image_descriptor im[] = {
        {2, 0, wn::runtime::graphics::descriptor_type::sampled_image,
            &texture_view, wn::runtime::graphics::resource_state::texture}};
    set.update_descriptors(buff, im, samp);

    wn::runtime::graphics::command_list_ptr setup_command_list =
        alloc.create_command_list();
    setup_command_list->transition_resource(vertex_buffer.buffer,
        wn::runtime::graphics::resource_state::host_write,
        wn::runtime::graphics::resource_state::vertex_buffer);
    setup_command_list->transition_resource(image_upload_buffer.buffer,
        wn::runtime::graphics::resource_state::host_write,
        wn::runtime::graphics::resource_state::copy_source);
    setup_command_list->transition_resource(texture,
        wn::runtime::graphics::resource_state::initial,
        wn::runtime::graphics::resource_state::copy_dest);
    setup_command_list->copy_buffer_to_image(
        image_upload_buffer.buffer, 0, texture);
    setup_command_list->transition_resource(texture,
        wn::runtime::graphics::resource_state::copy_dest,
        wn::runtime::graphics::resource_state::texture);

    setup_command_list->finalize();
    _queue->enqueue_command_list(setup_command_list.get());

    wn::runtime::graphics::graphics_pipeline pipeline =
        _device->create_graphics_pipeline(
            wn::runtime::graphics::graphics_pipeline_description(&m_allocator)
                .add_vertex_stream(
                    0, 16, wn::runtime::graphics::stream_frequency::per_vertex)
                .add_vertex_attribute(0, 0, "POSITION", 0,
                    wn::runtime::graphics::data_format::r32g32_sfloat)
                .add_vertex_attribute(0, 1, "TEXCOORD", 8,
                    wn::runtime::graphics::data_format::r32g32_sfloat)
                .add_color_output(0)
                .set_shader(
                    wn::runtime::graphics::shader_stage::vertex, &vs, "main")
                .set_shader(
                    wn::runtime::graphics::shader_stage::pixel, &ps, "main")
                .set_static_viewport(
                    0, wn::runtime::graphics::viewport{0, 0,
                           static_cast<float>(get_width()),
                           static_cast<float>(get_height()), 0, 1}),
            &layout, &render_pass, 0);

    wn::runtime::graphics::image_view image_view = _device->create_image_view(
        _image, static_cast<wn::runtime::graphics::image_components>(
                    wn::runtime::graphics::image_component::color));

    const wn::runtime::graphics::image_view* views[] = {&image_view};

    wn::runtime::graphics::framebuffer_create_info framebuffer_create = {
        &render_pass,
        wn::containers::contiguous_range<
            const wn::runtime::graphics::image_view*>(views),
        get_width(), get_height(), 1};

    wn::runtime::graphics::framebuffer framebuffer =
        _device->create_framebuffer(framebuffer_create);

    wn::runtime::graphics::command_list_ptr list = alloc.create_command_list();

    wn::runtime::graphics::clear_value clear{};
    clear.color.float_vals[0] = 0.1f;
    clear.color.float_vals[1] = 0.1f;
    clear.color.float_vals[2] = 0.1f;
    clear.color.float_vals[3] = 1.0f;
    list->begin_render_pass(&render_pass, &framebuffer,
        {0, 0, get_width(), get_height()},
        wn::containers::contiguous_range<wn::runtime::graphics::clear_value>(
            &clear, 1));
    list->bind_graphics_pipeline_layout(&layout);
    list->bind_graphics_pipeline(&pipeline);
    const wn::runtime::graphics::descriptor_set* sets[]{&set};
    list->bind_graphics_descriptor_sets(sets, 0);
    list->bind_vertex_buffer(0, &vertex_buffer.buffer);
    list->bind_index_buffer(
        wn::runtime::graphics::index_type::u32, &index_buffer.buffer);
    list->set_scissor(
        wn::runtime::graphics::scissor{static_cast<int32_t>(get_width() / 2), 0,
            get_width() / 2, get_height()});
    list->draw_indexed(3, 1, 0, 0, 0);
    list->end_render_pass();
    list->finalize();
    _queue->enqueue_command_list(list.get());
    wn::runtime::graphics::fence completion_fence = _device->create_fence();
    _queue->enqueue_fence(completion_fence);
    completion_fence.wait();
  });
}
