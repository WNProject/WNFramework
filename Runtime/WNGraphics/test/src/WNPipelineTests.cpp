// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNArena.h"
#include "WNGraphics/inc/WNDescriptors.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNFramebuffer.h"
#include "WNGraphics/inc/WNGraphicsPipeline.h"
#include "WNGraphics/inc/WNImage.h"
#include "WNGraphics/inc/WNImageView.h"
#include "WNGraphics/inc/WNRenderPass.h"
#include "WNGraphics/inc/WNShaderModule.h"
#include "WNGraphics/test/inc/WNTestFixture.h"

namespace vertex_shader {
const uint32_t vulkan[] =
#include "WNGraphics/test/shaders/pipeline.vs.glsl.h"
    ;
const size_t size_vulkan = sizeof(vulkan);

#ifndef BYTE
#define BYTE uint8_t
#endif

#include "WNGraphics/test/shaders/pipeline.vs.hlsl.h"
const BYTE* d3d12 = g_main;
const size_t size_d3d12 = sizeof(g_main);
};

namespace pixel_shader {
const uint32_t vulkan[] =
#include "WNGraphics/test/shaders/pipeline.ps.glsl.h"
    ;
const size_t size_vulkan = sizeof(vulkan);

#include "WNGraphics/test/shaders/pipeline.ps.hlsl.h"
const BYTE* d3d12 = g_main;
const size_t size_d3d12 = sizeof(g_main);
};

using pipeline_test = wn::runtime::graphics::testing::test;

TEST_F(pipeline_test, basic_pipeline) {
  wn::runtime::graphics::factory device_factory(&m_allocator, m_log);

  for (auto& adapter : device_factory.query_adapters()) {
    wn::runtime::graphics::device_ptr device =
        adapter->make_device(&m_allocator, m_log);
    ASSERT_NE(nullptr, device);

    // Time to find an image arena
    wn::containers::contiguous_range<
        const wn::runtime::graphics::arena_properties>
        properties = device->get_arena_properties();

    size_t idx = 0;
    for (idx = 0; idx < properties.size(); ++idx) {
      if (properties[idx].allow_render_targets &&
          properties[idx].device_local) {
        break;
      }
    }

    wn::runtime::graphics::shader_module vs = device->create_shader_module(
        adapter->api() == wn::runtime::graphics::adapter::api_type::vulkan
            ? wn::containers::contiguous_range<const uint8_t>(
                  reinterpret_cast<const uint8_t*>(vertex_shader::vulkan),
                  vertex_shader::size_vulkan)
            : wn::containers::contiguous_range<const uint8_t>(
                  reinterpret_cast<const uint8_t*>(vertex_shader::d3d12),
                  vertex_shader::size_d3d12));

    wn::runtime::graphics::shader_module ps = device->create_shader_module(
        adapter->api() == wn::runtime::graphics::adapter::api_type::vulkan
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
            1,   // binding
            20,  // register_base
            5,   // array_size
            static_cast<uint32_t>(wn::runtime::graphics::shader_stage::pixel),
            wn::runtime::graphics::descriptor_type::sampler,  // type
        }};
    wn::runtime::graphics::descriptor_set_layout descriptor_layout =
        device->create_descriptor_set_layout(infos);

    const wn::runtime::graphics::descriptor_pool_create_info pool_infos[] = {
        {5, wn::runtime::graphics::descriptor_type::read_only_buffer},
        {5, wn::runtime::graphics::descriptor_type::sampler}};
    wn::runtime::graphics::descriptor_pool pool =
        device->create_descriptor_pool(pool_infos);
    wn::runtime::graphics::descriptor_set set =
        pool.create_descriptor_set(&descriptor_layout);

    const wn::runtime::graphics::descriptor_set_layout* layouts[] = {
        &descriptor_layout};

    wn::runtime::graphics::pipeline_layout layout =
        device->create_pipeline_layout(layouts);

    const wn::runtime::graphics::render_pass_attachment attachment[] = {{}};
    const wn::runtime::graphics::attachment_reference color_attachments[] = {
        {}};
    wn::runtime::graphics::subpass_description subpasses[] = {{}};
    subpasses[0].color_attachments = color_attachments;
    wn::runtime::graphics::render_pass render_pass =
        device->create_render_pass(attachment, subpasses, nullptr);

    wn::runtime::graphics::image_create_info create_info = {
        1024, 1024, wn::runtime::graphics::data_format::r8g8b8a8_unorm,
        static_cast<uint32_t>(
            wn::runtime::graphics::resource_state::render_target)};
    wn::runtime::graphics::clear_value value = {};

    wn::runtime::graphics::image image =
        device->create_image(create_info, value);
    wn::runtime::graphics::image_memory_requirements reqs =
        image.get_memory_requirements();

    wn::runtime::graphics::arena image_arena =
        device->create_arena(idx, reqs.size);
    image.bind_memory(&image_arena, 0);

    wn::runtime::graphics::image_view view = device->create_image_view(
        &image, static_cast<wn::runtime::graphics::image_components>(
                    wn::runtime::graphics::image_component::color));
    const wn::runtime::graphics::image_view* views[] = {&view};
    wn::runtime::graphics::framebuffer_create_info framebuffer_create = {
        &render_pass, wn::containers::contiguous_range<
                          const wn::runtime::graphics::image_view*>(views),
        1024, 1024, 1};

    wn::runtime::graphics::framebuffer framebuffer =
        device->create_framebuffer(framebuffer_create);

    wn::runtime::graphics::graphics_pipeline pipeline =
        device->create_graphics_pipeline(
            wn::runtime::graphics::graphics_pipeline_description(&m_allocator)
                .add_vertex_stream(0, 8 * sizeof(float),
                    wn::runtime::graphics::stream_frequency::per_vertex)
                .add_vertex_attribute(0, 0, "POSITION", 0,
                    wn::runtime::graphics::data_format::r32g32b32a32_sfloat)
                .add_vertex_attribute(0, 1, "TEXCOORD", 16,
                    wn::runtime::graphics::data_format::r32g32b32a32_sfloat)
                .add_color_output(0)
                .set_shader(
                    wn::runtime::graphics::shader_stage::vertex, &vs, "main")
                .set_shader(
                    wn::runtime::graphics::shader_stage::pixel, &ps, "main"),
            &layout, &render_pass, 0);

    m_log->flush();
    // On normal operation the log buffer should be empty.
    EXPECT_EQ("", m_buffer);
    m_buffer.clear();
  }
}
