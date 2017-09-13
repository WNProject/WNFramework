// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/WNAdapter.h"
#include "WNWindow/inc/WNWindow.h"

namespace wn {
namespace runtime {
namespace graphics {

core::pair<surface, graphics_error> adapter::make_surface(
    runtime::window::window* _window) {
  surface s(this, _window->get_width(), _window->get_height());
  graphics_error err = initialize_surface(&s, _window);
  if (err != graphics_error::ok) {
    s.m_adapter = nullptr;
    return core::make_pair(surface(nullptr), err);
  }
  return core::make_pair(core::move(s), graphics_error::ok);
}

}  // namespace graphics
}  // namespace runtime
}  // namespace wn