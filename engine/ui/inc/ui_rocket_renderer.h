// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_UI_ROCKET_RENDERER_H__
#define __WN_ENGINE_UI_ROCKET_RENDERER_H__

#include "RmlUi/Core.h"
#include "WNContainers/inc/WNHashMap.h"
#include "WNContainers/inc/WNList.h"
#include "WNGraphics/inc/WNDescriptors.h"
#include "WNGraphics/inc/WNGraphicsPipeline.h"
#include "WNGraphics/inc/WNGraphicsTypes.h"

#include "WNGraphics/inc/WNSampler.h"
#include "WNGraphics/inc/WNShaderModule.h"
#include "WNMath/inc/WNVector.h"
#include "WNMemory/inc/unique_ptr.h"
#include "ui/inc/ui_resources.h"

namespace wn {
namespace scripting {
class engine;
}
namespace memory {
class allocator;
}
namespace file_system {
class mapping;
}
namespace runtime {
namespace window {
class window;
}
}  // namespace runtime
namespace engine {
namespace renderer {
class render_context;
class render_pass;
}  // namespace renderer
namespace ui {
class rocket_renderer : public Rml::RenderInterface {
public:
  rocket_renderer(memory::allocator* _allocator,
      renderer::render_context* _render_context,
      renderer::render_pass* _render_pass, file_system::mapping* _mapping,
      scripting::engine* _engine, logging::log* _log);

  void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices,
      int num_indices, Rml::TextureHandle texture,
      const Rml::Vector2f& translation) override;
  void EnableScissorRegion(bool enable) override;
  void SetScissorRegion(int x, int y, int width, int height) override;
  bool LoadTexture(Rml::TextureHandle& texture_handle,
      Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
  bool GenerateTexture(Rml::TextureHandle& texture_handle,
      const Rml::byte* source, const Rml::Vector2i& source_dimensions) override;
  void ReleaseTexture(Rml::TextureHandle texture_handle) override;
  Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex* vertices,
      int num_vertices, int* indices, int num_indices,
      Rml::TextureHandle _texture) override;
  void RenderCompiledGeometry(Rml::CompiledGeometryHandle _geometry,
      const Rml::Vector2f& translation) override;
  void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry) override;

  void set_setup_command_list(runtime::graphics::command_list* _ptr) {
    m_setup_command_list = _ptr;
  }
  void set_render_command_list(runtime::graphics::command_list* _ptr) {
    m_render_command_list = _ptr;
  }

  void set_renderpass_descriptor_set(
      const runtime::graphics::descriptor_set* _set) {
    m_renderpass_descriptor_set = _set;
  }

  void start_frame(size_t _parity);

  size_t get_width();
  size_t get_height();

private:
  memory::allocator* m_allocator;
  renderer::render_context* m_render_context;
  runtime::window::window* m_window;
  renderer::render_pass* m_render_pass;
  file_system::mapping* m_mapping;
  scripting::engine* m_engine;
  logging::log* m_log;

  // Graphics resources
  runtime::graphics::shader_module m_vertex_shader;
  runtime::graphics::shader_module m_pixel_shader;
  runtime::graphics::shader_module m_pixel_shader_no_texture;
  runtime::graphics::sampler m_sampler;

  runtime::graphics::descriptor_set_layout m_descriptor_set_layout;
  runtime::graphics::descriptor_set_layout m_descriptor_set_layout_no_texture;

  runtime::graphics::pipeline_layout m_pipeline_layout;
  runtime::graphics::pipeline_layout m_pipeline_layout_no_texture;

  runtime::graphics::graphics_pipeline m_pipeline;
  runtime::graphics::graphics_pipeline m_pipeline_no_texture;

  runtime::graphics::descriptor_pool m_descriptor_pool;

  // Actual rendering data (because LibRocket renders as if it's immediate)
  bool m_scissor_enabled = false;
  runtime::graphics::scissor m_set_scissor = {};
  runtime::graphics::scissor m_last_set_scissor = {};

  bool m_viewport_set = false;

  wn::runtime::graphics::graphics_pipeline* m_last_bound_pipeline = nullptr;
  wn::runtime::graphics::pipeline_layout* m_last_pipeline_layout = nullptr;

  // Resource management
  containers::hash_map<ui_texture*, memory::unique_ptr<ui_texture>> m_textures;
  containers::hash_map<ui_geometry*, memory::unique_ptr<ui_geometry>>
      m_geometry;

  containers::list<memory::unique_ptr<ui_resource>> m_cleanup;

  // Temporary per-frame data
  runtime::graphics::command_list* m_setup_command_list = nullptr;
  runtime::graphics::command_list* m_render_command_list = nullptr;
  const runtime::graphics::descriptor_set* m_renderpass_descriptor_set =
      nullptr;
  size_t m_frame_parity = 0;

  // Device related data
  math::vec2f m_screen_multiplier;
};

}  // namespace ui
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_UI_ROCKET_RENDERER_H__
