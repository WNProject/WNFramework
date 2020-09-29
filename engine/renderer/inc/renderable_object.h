// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once
#ifndef __WN_ENGINE_RENDERABLE_OBJECT_H__
#define __WN_ENGINE_RENDERABLE_OBJECT_H__

namespace wn {
namespace scripting {
class engine;
}
namespace engine {
namespace renderer {
class render_pass;
class renderer;

class renderable_object {
public:
  static void register_scripting(scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);

  virtual void render(renderer* _renderer, render_pass* _render_pass) = 0;

private:
};

}  // namespace renderer
}  // namespace engine
}  // namespace wn
#endif  // __WN_ENGINE_RENDERABLE_OBJECT__