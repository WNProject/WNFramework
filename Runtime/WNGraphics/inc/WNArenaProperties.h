// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_GRAPHICS_ARENA_PROPERTIES_H__
#define __WN_RUNTIME_GRAPHICS_ARENA_PROPERTIES_H__

#include "WNCore/inc/WNBase.h"

namespace wn {
namespace runtime {
namespace graphics {

struct arena_properties final {
  const bool device_local;
  const bool host_visible;
  const bool host_coherent;
  const bool host_cached;
  const bool allow_buffers;
  const bool allow_images;
  const bool allow_render_targets;
  const bool allow_depth_stencils;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_GRAPHICS_ARENA_PROPERTIES_H__
