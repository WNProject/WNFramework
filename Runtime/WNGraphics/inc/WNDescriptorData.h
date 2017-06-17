// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_DATA_TYPES_H__
#define __WN_GRAPHICS_DATA_TYPES_H__

#include "WNGraphics/inc/WNGraphicsEnums.h"

namespace wn {
namespace runtime {
namespace graphics {

struct descriptor_binding_info {
  size_t binding;
  size_t register_base;
  size_t array_size;
  uint32_t shader_stages;  // Bitwise OR of shader_stage values
  descriptor_type type;
};

struct descriptor_pool_create_info {
  size_t max_descriptors;
  descriptor_type type;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_DATA_TYPES_H__