// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_UI_H__
#define __WN_ENGINE_UI_H__

#include "renderer/inc/renderable_object.h"

namespace wn {
namespace memory {
class allocator;
}
namespace scripting {
class engine;
}
namespace file_system {
class mapping;
}

namespace engine {
namespace ui {

class ui : public renderer::renderable_object {
public:
  void render();

  static void register_scripting(memory::allocator* _allocator,
      scripting::engine* _engine, file_system::mapping* _mapping);
  static bool resolve_scripting(scripting::engine* _engine);

private:
};

}  // namespace ui
}  // namespace engine
}  // namespace wn
#endif  // __WN_ENGINE_UI_H__
