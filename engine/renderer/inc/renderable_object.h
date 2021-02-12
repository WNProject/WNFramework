// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_RENDERABLE_OBJECT_H__
#define __WN_ENGINE_RENDERABLE_OBJECT_H__

#include <cstddef>
#include <cstdint>

namespace wn {
namespace scripting {
class engine;
}
namespace runtime {
namespace graphics {
class command_list;
}
}  // namespace runtime
namespace engine {
namespace renderer {
class render_pass;
class render_context;

class renderable_object {
public:
  static void register_scripting(scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);

  virtual void render(render_pass* _render_pass,
      runtime::graphics::command_list* _setup_cmd_list,
      runtime::graphics::command_list* _cmd_list) = 0;
  virtual void initialize_for_renderpass(render_context* _renderer,
      render_pass* _render_pass,
      runtime::graphics::command_list* _setup_list) = 0;
  virtual void update_render_data(
      size_t _frame_parity, runtime::graphics::command_list* _command_list) = 0;

private:
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn
#endif  // __WN_ENGINE_RENDERABLE_OBJECT__
