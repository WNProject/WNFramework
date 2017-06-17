// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_GRAPHICS_ARENA_PROPERTIES_H__
#define __WN_RUNTIME_GRAPHICS_ARENA_PROPERTIES_H__

#include "WNCore/inc/WNBase.h"

namespace wn {
namespace runtime {
namespace graphics {

struct arena_properties WN_FINAL {
  const bool device_local : 1;
  const bool host_visible : 1;
  const bool host_coherent : 1;
  const bool host_cached : 1;
  const bool allow_buffers : 1;
  const bool allow_images : 1;
  const bool allow_render_targets : 1;
  const bool allow_depth_stencils : 1;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_GRAPHICS_ARENA_PROPERTIES_H__
