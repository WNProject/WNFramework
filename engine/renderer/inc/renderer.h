// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_RENDERER_RENDERER_H__
#define __WN_ENGINE_RENDERER_RENDERER_H__

#include "WNGraphics/inc/WNGraphicsEnums.h"
#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNEngine.h"
#include "engine_base/inc/context.h"
#include "renderer/inc/render_data.h"
#include "renderer/inc/render_target.h"

namespace wn {
namespace engine {
namespace window {
class window;
}
namespace renderer {

class renderer {
public:
  renderer(memory::allocator* _allocator, logging::log* _log,
      window::window* _window, int32_t _width, int32_t _height)
    : m_log(_log),
      m_width(_width),
      m_height(_height),
      m_attachments(_allocator) {
    if (_window) {
      m_log->log_info("Created Renderer With Window");
    } else {
      m_log->log_info("Created Renderer Without Window");
    }

    m_log->log_info("Renderer Size: ", _width, "x", _height);
    if (_width > 0 && _height > 0) {
      m_attachments.push_back(color_attachment{
          runtime::graphics::data_format::r8g8b8a8_unorm, _width, _height});
    }
  }

  ~renderer() {
    m_log->log_info("Destroyed Renderer");
  }
  static void register_scripting(scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);

  void register_context(scripting::script_pointer<render_context> _context);

  int32_t width() {
    return m_width;
  }
  int32_t height() {
    return m_height;
  }

private:
  logging::log* m_log;
  int32_t m_width;
  int32_t m_height;

  struct color_attachment {
    runtime::graphics::data_format _format;
    int32_t width;
    int32_t height;
  };

  containers::dynamic_array<color_attachment>
      m_attachments;

};  // namespace renderer

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_RENDERER_RENDERER_H__