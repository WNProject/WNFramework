// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_WINDOW_WINDOW_H__
#define __WN_ENGINE_WINDOW_WINDOW_H__

#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNScripting/inc/WNEngine.h"
#include "engine_base/inc/context.h"

namespace wn {
namespace runtime {
namespace window {
class window;
}  // namespace window
}  // namespace runtime
}  // namespace wn

namespace wn {
namespace engine {
namespace window {

class window {
public:
  window(engine_base::context* _context, int32_t _width = 300,
      int32_t _height = 300);
  ~window();

  static void register_scripting(scripting::engine* _engine);
  int32_t width();
  int32_t height();

  runtime::window::window* underlying() {
    return m_window.get();
  }

private:
  logging::log* m_log;
  memory::unique_ptr<runtime::window::window> m_window;
};

}  // namespace window
}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_WINDOW_WINDOW_H__
