// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_RUNTIME_GRAPHICS_ARENA_PROPERTIES_H__
#define __WN_RUNTIME_GRAPHICS_ARENA_PROPERTIES_H__

#include "core/inc/base.h"

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
  const bool allow_d16_u;
  const bool allow_d24_u;
  const bool allow_d32_f;
  const bool allow_s8;
  const bool allow_d16_us8;
  const bool allow_d24_us8;
  const bool allow_d32_fs8;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_GRAPHICS_ARENA_PROPERTIES_H__
