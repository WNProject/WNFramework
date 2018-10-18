// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_RENDERER_RENDERER_H__
#define __WN_ENGINE_RENDERER_RENDERER_H__

#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNEngine.h"
#include "engine_base/inc/context.h"
#include "renderer/inc/render_data.h"

namespace wn {
namespace engine {
namespace renderer {

class renderer {
public:
  renderer(logging::log* _log) : m_log(_log) {
    m_log->log_info("Created Renderer");
  }

  ~renderer() {
    m_log->log_info("Destroyed Renderer");
  }
  static void register_scripting(scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);

  void register_object(
      scripting::shared_script_pointer<render_data> _render_data);
  void register_pass(scripting::script_pointer<pass_data> _pass_data);

private:
  logging::log* m_log;
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_RENDERER_RENDERER_H__