// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_RENDERER_RENDER_CONTEXT_H__
#define __WN_ENGINE_RENDERER_RENDER_CONTEXT_H__

#include "WNContainers/inc/WNList.h"
#include "WNContainers/inc/WNRangePartition.h"
#include "WNGraphics/inc/WNAdapter.h"
#include "WNGraphics/inc/WNCommandAllocator.h"
#include "WNGraphics/inc/WNDevice.h"
#include "WNGraphics/inc/WNFactory.h"
#include "WNGraphics/inc/WNFence.h"
#include "WNGraphics/inc/WNFramebuffer.h"
#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNGraphics/inc/WNQueue.h"
#include "WNGraphics/inc/WNRenderPass.h"
#include "WNGraphics/inc/WNSignal.h"
#include "WNGraphics/inc/WNSwapchain.h"
#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNEngine.h"
#include "engine_base/inc/context.h"
#include "renderer/inc/gpu_heap.h"
#include "renderer/inc/render_data.h"
#include "renderer/inc/render_pass.h"
#include "renderer/inc/render_target.h"
#include "renderer/inc/temporary_objects.h"
#include "renderer/inc/texture.h"

#include <chrono>

namespace wn {
namespace file_system {
class mapping;
}
namespace engine {
namespace window {
class window;
}

namespace renderer {
class renderable_object;
class render_context {
public:
  render_context(memory::allocator* _allocator, logging::log* _log,
      window::window* _window, int32_t _width, int32_t _height,
      uint32_t _forced_adapter = 0);

  virtual ~render_context();
  static void register_scripting(
      memory::allocator* _allocator, scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);

  virtual void register_description(
      scripting::script_pointer<render_description> _context);

  virtual int32_t width() {
    return m_width;
  }
  virtual int32_t height() {
    return m_height;
  }

  virtual bool render();

  virtual void add_renderable_to_passes(
      scripting::shared_cpp_pointer<renderable_object> object,
      scripting::slice<const char*> passes);

  // TODO(awoloszyn): Once we figure out exactly what kind of things we will
  // need we can replace these with higher-level constructs.
  gpu_allocation get_allocation_for_render_target(
      uint64_t _size, uint64_t _alignment);
  gpu_allocation get_allocation_for_buffer(uint64_t _size, uint64_t _alignment);
  gpu_allocation get_allocation_for_upload(uint64_t _size, uint64_t _alignment);
  gpu_allocation get_allocation_for_texture(
      uint64_t _size, uint64_t _alignment);

  runtime::graphics::device* get_device() {
    return m_device.get();
  }
  window::window* get_window() {
    return m_window;
  }

  runtime::graphics::image* create_temporary_image(
      runtime::graphics::image_create_info* _create_info);
  runtime::graphics::buffer* create_temporary_upload_buffer(size_t _size);

  // TODO(awoloszyn): Turn this into a managed/cached resource.
  //    But for now, just return a unique_ptr.
  memory::unique_ptr<texture> load_texture(
      runtime::graphics::command_list* _setup_command_list,
      file_system::mapping* _mapping,
      scripting::script_pointer<texture_desc> _tex, logging::log* _log);

private:
  logging::log* m_log;
  window::window* m_window;
  memory::allocator* m_allocator;
  int32_t m_width;
  int32_t m_height;
  int32_t m_output_rt;
  uint64_t m_frame_num = 0;
  uint64_t m_last_second_frame = 0;
  uint64_t m_last_up_to_date_frame = 0;
  int32_t m_last_up_to_date_width = 0;
  int32_t m_last_up_to_date_height = 0;

  runtime::graphics::adapter_ptr m_adapter;
  runtime::graphics::factory m_factory;
  runtime::graphics::device_ptr m_device;
  runtime::graphics::queue_ptr m_queue;
  runtime::graphics::queue_profiler_ptr m_queue_profiler;
  memory::unique_ptr<runtime::graphics::surface> m_surface;
  runtime::graphics::swapchain_ptr m_swapchain;
  wn::containers::contiguous_range<
      const wn::runtime::graphics::arena_properties>
      m_arena_properties;

  containers::dynamic_array<runtime::graphics::command_allocator>
      m_command_allocators;
  containers::dynamic_array<runtime::graphics::command_list_ptr>
      m_command_lists;
  containers::dynamic_array<runtime::graphics::command_allocator>
      m_setup_command_allocators;
  containers::dynamic_array<runtime::graphics::command_list_ptr>
      m_setup_command_lists;
  containers::dynamic_array<runtime::graphics::fence> m_frame_fences;
  containers::dynamic_array<runtime::graphics::signal> m_swapchain_get_signals;
  containers::dynamic_array<runtime::graphics::signal>
      m_swapchain_ready_signals;
  containers::dynamic_array<bool> m_swapchain_image_initialized;

  memory::unique_ptr<gpu_heap> m_render_target_heap;
  memory::unique_ptr<gpu_heap> m_buffer_heap;
  memory::unique_ptr<gpu_heap> m_upload_heap;
  memory::unique_ptr<gpu_heap> m_texture_heap;
  memory::unique_ptr<gpu_heap> m_temporary_texture_heap;

  // Keep the render targets below the render target heap, we need the
  // targets to be cleaned up before the heap.
  containers::dynamic_array<render_target> m_render_targets;

  containers::dynamic_array<render_pass> m_render_passes;
  containers::dynamic_array<containers::string> m_render_pass_names;
  containers::dynamic_array<scripting::shared_cpp_pointer<renderable_object>>
      m_renderables;

  containers::dynamic_array<containers::list<temporary_buffer>>
      m_temporary_buffers;
  containers::dynamic_array<containers::list<temporary_image>>
      m_temporary_images;
  containers::dynamic_array<
      containers::list<scripting::shared_cpp_pointer<renderable_object>>>
      m_pending_renderables;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_time;

  friend class render_target;
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_RENDERER_RENDER_CONTEXT_H__
