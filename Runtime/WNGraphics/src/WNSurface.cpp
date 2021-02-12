// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/WNSurface.h"

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNGraphics/inc/WNAdapter.h"

namespace wn {
namespace runtime {
namespace graphics {

surface::~surface() {
  if (is_valid()) {
    m_adapter->destroy_surface(this);
  }
}

graphics_error surface::get_surface_capabilities(
    surface_capabilities* _capabilities) {
  return m_adapter->get_surface_capabilities(this, _capabilities);
}

}  // namespace graphics
}  // namespace runtime
}  // namespace wn
