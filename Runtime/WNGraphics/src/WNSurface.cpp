// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

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

}  // namespace graphics
}  // namespace runtime
}  // namespace wn