// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "ui/inc/ui_rocket_renderer.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNWindow/inc/WNWindow.h"
#include "window/inc/window.h"

namespace {

wn::runtime::graphics::shader_module get_module(
    wn::file_system::mapping* _mapping, wn::runtime::graphics::device* _device,
    wn::containers::string_view _file_name) {
  wn::file_system::result res;
  auto file = _mapping->open_file(_file_name, res);
  return _device->create_shader_module(file->range());
}

}  // namespace

namespace wn {
namespace engine {
namespace ui {

rocket_renderer::rocket_renderer(memory::allocator* _allocator,
    Rocket::Core::Context* _rocket_context,
    renderer::render_context* _render_context,
    renderer::render_pass* _render_pass, file_system::mapping* _mapping)
  : Rocket::Core::RenderInterface(_rocket_context),
    m_allocator(_allocator),
    m_rocket_context(_rocket_context),
    m_render_context(_render_context),
    m_window(_render_context->get_window()->underlying()),
    m_render_pass(_render_pass),
    m_textures(_allocator),
    m_geometry(_allocator),
    m_cleanup(_allocator),
    m_screen_multiplier(
        _render_context->get_device()->get_2d_transform_scale()) {
  auto device = _render_context->get_device();
  wn::runtime::graphics::descriptor_binding_info binding_infos[]{
      {
          0,  // binding
          0,  // register_base
          1,  // array_size
          static_cast<uint32_t>(
              wn::runtime::graphics::shader_stage::pixel),  // stage
          wn::runtime::graphics::descriptor_type::sampler,  // type
      },
      {
          1,  // binding
          0,  // register_base
          1,  // array_size
          static_cast<uint32_t>(wn::runtime::graphics::shader_stage::vertex) |
              static_cast<uint32_t>(
                  wn::runtime::graphics::shader_stage::pixel),   // stage
          wn::runtime::graphics::descriptor_type::sampled_image  // type
      },
  };

  m_descriptor_set_layout = device->create_descriptor_set_layout(binding_infos);
  m_descriptor_set_layout_no_texture = device->create_descriptor_set_layout({});

  const wn::runtime::graphics::push_constant_range ranges[] = {{
      static_cast<uint32_t>(
          wn::runtime::graphics::shader_stage::vertex),  // valid_stages
      0,                                                 // register_base
      2,                                                 // num_uint32_values
      0,                                                 // offset_in_uint32s
  }};

  const wn::runtime::graphics::descriptor_set_layout* layouts[] = {
      &m_descriptor_set_layout};
  m_pipeline_layout = device->create_pipeline_layout(layouts, ranges);

  layouts[0] = &m_descriptor_set_layout_no_texture;
  m_pipeline_layout_no_texture =
      device->create_pipeline_layout(layouts, ranges);

  // TODO: Make shaders first-class resources. Until then
  // this will have to do
  m_vertex_shader = get_module(_mapping, device,
      wn::containers::string(_allocator, "assets/built_shaders/ui.vs") +
          device->get_shader_suffix().data());
  m_pixel_shader = get_module(_mapping, device,
      wn::containers::string(_allocator, "assets/built_shaders/ui_tex.ps") +
          device->get_shader_suffix().data());
  m_pixel_shader_no_texture = get_module(_mapping, device,
      wn::containers::string(_allocator, "assets/built_shaders/ui_no_tex.ps") +
          device->get_shader_suffix().data());

  m_pipeline = device->create_graphics_pipeline(
      wn::runtime::graphics::graphics_pipeline_description(_allocator)
          .add_vertex_stream(0, sizeof(Rocket::Core::Vertex),
              wn::runtime::graphics::stream_frequency::per_vertex)
          .add_vertex_attribute(0, 0, "POSITION", 0,
              wn::runtime::graphics::data_format::r32g32_sfloat)
          .add_vertex_attribute(0, 1, "COLOR", 8,
              wn::runtime::graphics::data_format::r8g8b8a8_unorm)
          .add_vertex_attribute(0, 2, "TEXCOORD", 12,
              wn::runtime::graphics::data_format::r32g32_sfloat)
          .set_num_scissors(1)
          .set_num_viewports(1)
          .set_index_type(wn::runtime::graphics::index_type::u32)
          .set_cull_mode(wn::runtime::graphics::cull_mode::none)
          .add_color_output(0)
          .set_shader(wn::runtime::graphics::shader_stage::vertex,
              &m_vertex_shader, "main")
          .set_shader(wn::runtime::graphics::shader_stage::pixel,
              &m_pixel_shader, "main")
          .enable_blending(0, wn::runtime::graphics::blend_factor::src_alpha,
              wn::runtime::graphics::blend_factor::inv_src_alpha,
              wn::runtime::graphics::blend_op::add,
              wn::runtime::graphics::blend_factor::src_alpha,
              wn::runtime::graphics::blend_factor::inv_src_alpha,
              wn::runtime::graphics::blend_op::add),
      &m_pipeline_layout, _render_pass->get_render_pass(), 0);

  m_pipeline_no_texture = device->create_graphics_pipeline(
      wn::runtime::graphics::graphics_pipeline_description(_allocator)
          .add_vertex_stream(0, sizeof(Rocket::Core::Vertex),
              wn::runtime::graphics::stream_frequency::per_vertex)
          .add_vertex_attribute(0, 0, "POSITION", 0,
              wn::runtime::graphics::data_format::r32g32_sfloat)
          .add_vertex_attribute(0, 1, "COLOR", 8,
              wn::runtime::graphics::data_format::r8g8b8a8_unorm)
          .add_vertex_attribute(0, 2, "TEXCOORD", 12,
              wn::runtime::graphics::data_format::r32g32_sfloat)
          .set_num_scissors(1)
          .set_num_viewports(1)
          .set_index_type(wn::runtime::graphics::index_type::u32)
          .set_cull_mode(wn::runtime::graphics::cull_mode::none)
          .add_color_output(0)
          .set_shader(wn::runtime::graphics::shader_stage::vertex,
              &m_vertex_shader, "main")
          .set_shader(wn::runtime::graphics::shader_stage::pixel,
              &m_pixel_shader_no_texture, "main")
          .enable_blending(0, wn::runtime::graphics::blend_factor::src_alpha,
              wn::runtime::graphics::blend_factor::inv_src_alpha,
              wn::runtime::graphics::blend_op::add,
              wn::runtime::graphics::blend_factor::src_alpha,
              wn::runtime::graphics::blend_factor::inv_src_alpha,
              wn::runtime::graphics::blend_op::add),
      &m_pipeline_layout_no_texture, _render_pass->get_render_pass(), 0);

  const wn::runtime::graphics::descriptor_pool_create_info pool_infos[] = {
      {100, wn::runtime::graphics::descriptor_type::sampler},
      {100, wn::runtime::graphics::descriptor_type::sampled_image}};

  m_descriptor_pool = device->create_descriptor_pool(pool_infos);
  m_sampler = device->create_sampler({});
}

void rocket_renderer::RenderGeometry(Rocket::Core::Vertex* vertices,
    int num_vertices, int* indices, int num_indices,
    Rocket::Core::TextureHandle texture,
    const Rocket::Core::Vector2f& translation) {
  WN_RELEASE_ASSERT(false, "Not implemented yet");
  (void)vertices;
  (void)num_vertices;
  (void)indices;
  (void)num_indices;
  (void)texture;
  (void)translation;
}
void rocket_renderer::EnableScissorRegion(bool enable) {
  m_scissor_enabled = enable;
  if (!enable) {
    m_last_set_scissor = {};
  }
}

void rocket_renderer::SetScissorRegion(int x, int y, int width, int height) {
  m_set_scissor = {static_cast<int32_t>(x), static_cast<int32_t>(y),
      static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

bool rocket_renderer::LoadTexture(Rocket::Core::TextureHandle& texture_handle,
    Rocket::Core::Vector2i& texture_dimensions,
    const Rocket::Core::String& source) {
  (void)texture_handle;
  (void)texture_dimensions;
  (void)source;
  return false;
}

bool rocket_renderer::GenerateTexture(
    Rocket::Core::TextureHandle& texture_handle,
    const Rocket::Core::byte* source,
    const Rocket::Core::Vector2i& source_dimensions) {
  auto tb = create_and_initialize_texture(m_allocator, m_render_context,
      m_setup_command_list, source,
      source_dimensions.x * source_dimensions.y * 4, source_dimensions.x,
      source_dimensions.y, runtime::graphics::data_format::r8g8b8a8_unorm,
      runtime::graphics::resource_state::texture);
  tb.first->use_in_frame(m_frame_parity);
  m_cleanup.push_back(core::move(tb.first));
  tb.second->use_in_frame(m_frame_parity);
  ui_texture* tex = tb.second.get();
  m_textures[tex] = core::move(tb.second);
  texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>(tex);
  return true;
}

void rocket_renderer::ReleaseTexture(
    Rocket::Core::TextureHandle texture_handle) {
  ui_texture* tex = reinterpret_cast<ui_texture*>(texture_handle);
  auto it = m_textures.find(tex);
  WN_DEBUG_ASSERT(it != m_textures.end(), "Texture does not exist");
  m_cleanup.push_back(core::move(it->second));
  m_textures.erase(it);
}

Rocket::Core::CompiledGeometryHandle rocket_renderer::CompileGeometry(
    Rocket::Core::Vertex* vertices, int num_vertices, int* indices,
    int num_indices, Rocket::Core::TextureHandle _texture) {
  ui_texture* tex = reinterpret_cast<ui_texture*>(_texture);
  if (tex) {
    tex->use_in_frame(m_frame_parity);
  }
  uint64_t width = m_render_pass->get_width();
  uint64_t height = m_render_pass->get_height();

  // TODO: Replace this with push constants and do this on the GPU
  //  .. that is the only way this will work with resizing.
  for (int i = 0; i < num_vertices; ++i) {
    vertices[i].position.x =
        m_screen_multiplier[0] *
            (2.0f * vertices[i].position.x / static_cast<float>(width)) -
        1.0f;
    vertices[i].position.y =
        m_screen_multiplier[1] *
        ((-2.0f * vertices[i].position.y / static_cast<float>(height)) + 1.0f);
  }

  geometry_data dat = create_and_initialize_geometry(m_allocator,
      m_render_context, m_setup_command_list, vertices, num_vertices, indices,
      num_indices, tex);
  dat.m_data->use_in_frame(m_frame_parity);
  dat.m_setup->use_in_frame(m_frame_parity);
  ui_geometry* geo = dat.m_data.get();
  if (geo->get_texture()) {
    geo->set() =
        m_descriptor_pool.create_descriptor_set(&m_descriptor_set_layout);
    wn::runtime::graphics::sampler_descriptor samp[] = {{0, 0, &m_sampler}};
    wn::runtime::graphics::image_descriptor im[] = {
        {1, 0, wn::runtime::graphics::descriptor_type::sampled_image,
            &geo->get_texture()->view(),
            wn::runtime::graphics::resource_state::texture}};
    geo->set().update_descriptors({}, im, samp);
  }

  m_geometry[dat.m_data.get()] = core::move(dat.m_data);
  m_cleanup.push_back(core::move(dat.m_setup));

  return reinterpret_cast<Rocket::Core::CompiledGeometryHandle>(geo);
}

void rocket_renderer::RenderCompiledGeometry(
    Rocket::Core::CompiledGeometryHandle _geometry,
    const Rocket::Core::Vector2f& translation) {
  ui_geometry* geo = reinterpret_cast<ui_geometry*>(_geometry);
  geo->use_in_frame(m_frame_parity);
  uint64_t width = m_render_pass->get_width();
  uint64_t height = m_render_pass->get_height();

  wn::runtime::graphics::graphics_pipeline* next_pipeline =
      geo->get_texture() ? &m_pipeline : &m_pipeline_no_texture;

  wn::runtime::graphics::pipeline_layout* next_layout =
      geo->get_texture() ? &m_pipeline_layout : &m_pipeline_layout_no_texture;
  bool pipeline_changed = false;
  if (next_layout != m_last_pipeline_layout) {
    m_render_command_list->bind_graphics_pipeline_layout(next_layout);
    m_last_pipeline_layout = next_layout;
    pipeline_changed = true;
  }

  if (next_pipeline != m_last_bound_pipeline) {
    m_last_bound_pipeline = next_pipeline;
    m_render_command_list->bind_graphics_pipeline(next_pipeline);
  }

  if (pipeline_changed || m_last_set_scissor != m_set_scissor) {
    if (m_scissor_enabled) {
      m_render_command_list->set_scissor(m_set_scissor);
      m_last_set_scissor = m_set_scissor;
    } else {
      m_render_command_list->set_scissor(
          {0, 0, static_cast<uint32_t>(m_render_pass->get_width()),
              static_cast<uint32_t>(m_render_pass->get_height())});
    }
  }

  if (pipeline_changed || !m_viewport_set) {
    m_render_command_list->set_viewport(
        {0, 0, static_cast<float>(m_render_pass->get_width()),
            static_cast<float>(m_render_pass->get_height()), 0, 1});
    m_viewport_set = true;
  }

  if (geo->get_texture()) {
    geo->get_texture()->use_in_frame(m_frame_parity);
    const runtime::graphics::descriptor_set* sets[] = {&geo->set()};
    m_render_command_list->bind_graphics_descriptor_sets(sets, 0);
  }

  m_render_command_list->bind_vertex_buffer(0, &geo->vertex_buffer());
  m_render_command_list->bind_index_buffer(
      wn::runtime::graphics::index_type::u32, &geo->index_buffer());

  float constants[2] = {m_screen_multiplier[0] *
                            (2.0f * translation.x / static_cast<float>(width)),
      m_screen_multiplier[1] *
          (-2.0f * translation.y / static_cast<float>(height))};

  m_render_command_list->push_graphics_contants(
      0, 0, reinterpret_cast<const uint32_t*>(constants), 2);
  m_render_command_list->draw_indexed(
      static_cast<uint32_t>(geo->num_inds()), 1, 0, 0, 0);
}

void rocket_renderer::ReleaseCompiledGeometry(
    Rocket::Core::CompiledGeometryHandle _geometry) {
  ui_geometry* geo = reinterpret_cast<ui_geometry*>(_geometry);
  auto it = m_geometry.find(geo);
  WN_DEBUG_ASSERT(it != m_geometry.end(), "Geometry does not exist");
  m_cleanup.push_back(core::move(it->second));
  m_geometry.erase(it);
}

float rocket_renderer::GetPixelsPerInch() {
  const float dpi = static_cast<float>(m_window->get_dpi());
  return dpi;
}

void rocket_renderer::start_frame(size_t _parity) {
  for (auto it = m_cleanup.begin(); it != m_cleanup.end(); ++it) {
    if ((*it)->remove_from_frame(_parity)) {
      it = m_cleanup.erase(it);
      continue;
    }
    ++it;
  }
  m_frame_parity = _parity;

  m_last_bound_pipeline = nullptr;
  m_last_pipeline_layout = nullptr;
}

size_t rocket_renderer::get_width() {
  return m_render_pass->get_width();
}

size_t rocket_renderer::get_height() {
  return m_render_pass->get_height();
}

}  // namespace ui
}  // namespace engine
}  // namespace wn