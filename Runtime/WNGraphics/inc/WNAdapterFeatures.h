// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_RUNTIME_GRAPHICS_ADAPTER_FEATURES_H__
#define __WN_RUNTIME_GRAPHICS_ADAPTER_FEATURES_H__

#include "WNCore/inc/types.h"

namespace wn {
namespace runtime {
namespace graphics {

struct adapter_features {
  bool non_solid_fill = false;
  bool tessellation = false;
  bool geometry = false;
  bool depth_clamp = false;
  bool depth_bias_clamp = false;
  bool dual_src_blending = false;
  bool sample_rate_shading = false;
  bool input_attachments = false;

  bool etc2_textures = false;
  bool astc_ldr_textures = false;
  bool bc_textures = false;

  bool clip_distance = false;
  bool cull_distance = false;
  uint32_t num_viewports = 1;
  uint32_t num_scissors = 1;

  bool is_superset_of(const adapter_features& _other) const {
    if (_other.non_solid_fill > non_solid_fill) {
      return false;
    }
    if (_other.tessellation > tessellation) {
      return false;
    }
    if (_other.geometry > geometry) {
      return false;
    }
    if (_other.depth_clamp > depth_clamp) {
      return false;
    }
    if (_other.depth_bias_clamp > depth_bias_clamp) {
      return false;
    }
    if (_other.dual_src_blending > dual_src_blending) {
      return false;
    }
    if (_other.sample_rate_shading > sample_rate_shading) {
      return false;
    }
    if (_other.input_attachments > input_attachments) {
      return false;
    }

    if (_other.etc2_textures > etc2_textures) {
      return false;
    }
    if (_other.astc_ldr_textures > astc_ldr_textures) {
      return false;
    }
    if (_other.bc_textures > bc_textures) {
      return false;
    }

    if (_other.clip_distance > clip_distance) {
      return false;
    }
    if (_other.cull_distance > cull_distance) {
      return false;
    }
    if (_other.num_viewports > num_viewports) {
      return false;
    }
    if (_other.num_scissors > num_scissors) {
      return false;
    }
    return true;
  }
};

struct adapter_formats {
  bool has_d16_unorm = false;
  bool has_d24_unorm = false;
  bool has_d32_float = false;
  bool has_s8_unorm = false;
  bool has_d16_unorm_s8_unorm = false;
  bool has_d24_unorm_s8_unorm = false;
  bool has_d32_float_s8_unorm = false;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_GRAPHICS_ADAPTER_FEATURES_H__
