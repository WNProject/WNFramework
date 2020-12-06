// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_GRAPHICS_DATA_TYPES_H__
#define __WN_GRAPHICS_DATA_TYPES_H__

#include "WNGraphics/inc/WNGraphicsEnums.h"
namespace wn {
namespace runtime {
namespace graphics {

class buffer;
class sampler;
class image_view;

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

struct buffer_descriptor {
  uint32_t binding;
  uint32_t array_offset;
  descriptor_type type;
  buffer* resource;
  uint32_t offset_in_elements;
  uint32_t element_size;
  uint32_t num_elements;
};

struct image_descriptor {
  uint32_t binding;
  uint32_t array_offset;
  descriptor_type type;
  image_view* resource;
  resource_state state;
};

struct sampler_descriptor {
  uint32_t binding;
  uint32_t array_offset;
  sampler* resource;
};

}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_DATA_TYPES_H__
