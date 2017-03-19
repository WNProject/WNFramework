// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNDescriptors.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
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

using pipeline_test = wn::graphics::testing::test;

TEST_F(pipeline_test, basic_pipeline) {
  wn::graphics::factory device_factory(&m_allocator, m_log);

  for (auto& adapter : device_factory.query_adapters()) {
    wn::graphics::device_ptr device = adapter->make_device(&m_allocator, m_log);
    ASSERT_NE(nullptr, device);

    wn::graphics::shader_module vs = device->create_shader_module(
        adapter->api() == wn::graphics::adapter::api_type::vulkan
            ? wn::containers::contiguous_range<const uint8_t>(
                  reinterpret_cast<const uint8_t*>(vertex_shader::vulkan),
                  vertex_shader::size_vulkan)
            : wn::containers::contiguous_range<const uint8_t>(
                  reinterpret_cast<const uint8_t*>(vertex_shader::d3d12),
                  vertex_shader::size_d3d12));

    const wn::graphics::descriptor_binding_info infos[] = {
        {
            0,  // binding
            0,  // register_base
            1,  // array_size
            static_cast<uint32_t>(wn::graphics::shader_stage::vertex),
            wn::graphics::descriptor_type::read_only_buffer,  // type
        },
        {
            1,   // binding
            20,  // register_base
            5,   // array_size
            static_cast<uint32_t>(wn::graphics::shader_stage::pixel),
            wn::graphics::descriptor_type::sampler,  // type
        }};
    wn::graphics::descriptor_set_layout descriptor_layout =
        device->create_descriptor_set_layout(infos);

    const wn::graphics::descriptor_pool_create_info pool_infos[] = {
        {5, wn::graphics::descriptor_type::read_only_buffer},
        {5, wn::graphics::descriptor_type::sampler}};
    wn::graphics::descriptor_pool pool =
        device->create_descriptor_pool(pool_infos);
    wn::graphics::descriptor_set set =
        pool.create_descriptor_set(&descriptor_layout);

    const wn::graphics::descriptor_set_layout* layouts[] = {&descriptor_layout};

    wn::graphics::pipeline_layout layout =
        device->create_pipeline_layout(layouts);

    const wn::graphics::render_pass_attachment attachment[] = {{}};
    const wn::graphics::attachment_reference color_attachments[] = {{}};
    wn::graphics::subpass_description subpasses[] = {{}};
    subpasses[0].color_attachments = color_attachments;
    wn::graphics::render_pass p =
        device->create_render_pass(attachment, subpasses, nullptr);

    wn::graphics::image_create_info create_info = {1024, 1024,
        wn::graphics::image_format::r8g8b8a8_unorm,
        static_cast<uint32_t>(wn::graphics::resource_state::render_target)};
    wn::graphics::image image = device->create_image(create_info);
    wn::graphics::image_view view = device->create_image_view(&image);
    m_log->flush();
    // On normal operation the log buffer should be empty.
    EXPECT_EQ("", m_buffer);
    m_buffer.clear();
  }
}
