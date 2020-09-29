// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_RENDERER_RENDER_CONTEXT_H__
#define __WN_ENGINE_RENDERER_RENDER_CONTEXT_H__

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

namespace wn {
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

  virtual ~render_context() {
    m_log->log_info("Destroyed Renderer");
  }
  static void register_scripting(scripting::engine* _engine);
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

private:
  gpu_allocation get_allocation_for_render_target(
      uint64_t _size, uint64_t _alignment);

  logging::log* m_log;
  window::window* m_window;
  memory::allocator* m_allocator;
  int32_t m_width;
  int32_t m_height;
  int32_t m_output_rt;
  uint64_t m_frame_num = 0;

  runtime::graphics::adapter_ptr m_adapter;
  runtime::graphics::factory m_factory;
  runtime::graphics::device_ptr m_device;
  runtime::graphics::queue_ptr m_queue;
  memory::unique_ptr<runtime::graphics::surface> m_surface;
  runtime::graphics::swapchain_ptr m_swapchain;
  wn::containers::contiguous_range<
      const wn::runtime::graphics::arena_properties>
      m_arena_properties;

  containers::dynamic_array<runtime::graphics::command_allocator>
      m_command_allocators;
  containers::dynamic_array<runtime::graphics::command_list_ptr>
      m_command_lists;
  containers::dynamic_array<runtime::graphics::fence> m_frame_fences;
  containers::dynamic_array<runtime::graphics::signal> m_swapchain_get_signals;
  containers::dynamic_array<runtime::graphics::signal>
      m_swapchain_ready_signals;
  containers::dynamic_array<bool> m_swapchain_image_initialized;

  memory::unique_ptr<gpu_heap> m_render_target_heap;
  // Keep the render targets below the render target heap, we need the
  // targets to be cleaned up before the heap.
  containers::dynamic_array<render_target> m_render_targets;

  containers::dynamic_array<render_pass> m_render_passes;
  containers::dynamic_array<containers::string> m_render_pass_names;
  friend class render_target;
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_RENDERER_RENDER_CONTEXT_H__