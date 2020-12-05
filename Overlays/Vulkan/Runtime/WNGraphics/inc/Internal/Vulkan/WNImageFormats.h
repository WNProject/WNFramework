// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_D3D12_IMAGE_FORMATS_H__
#define __WN_GRAPHICS_INTERNAL_D3D12_IMAGE_FORMATS_H__

#include "WNGraphics/inc/WNImage.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {

static inline VkFormat image_format_to_vulkan_format(data_format _format) {
  static const VkFormat formats[] = {
      VK_FORMAT_R8G8B8A8_UNORM,
      VK_FORMAT_R32G32B32A32_SFLOAT,
      VK_FORMAT_R32G32B32_SFLOAT,
      VK_FORMAT_R32G32_SFLOAT,
      VK_FORMAT_B8G8R8A8_UNORM,
      // Depth formats
      VK_FORMAT_D16_UNORM,
      VK_FORMAT_X8_D24_UNORM_PACK32,
      VK_FORMAT_D32_SFLOAT,
      VK_FORMAT_S8_UINT,
      VK_FORMAT_D16_UNORM_S8_UINT,
      VK_FORMAT_D24_UNORM_S8_UINT,
      VK_FORMAT_D32_SFLOAT_S8_UINT,
      // Compressed formats
      VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
      VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
      VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
      VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
      VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
      VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
      VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
      VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
      VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
      VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
      VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
      VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
      VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
      VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
      VK_FORMAT_BC1_RGB_UNORM_BLOCK,
      VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
      VK_FORMAT_BC2_UNORM_BLOCK,
      VK_FORMAT_BC3_UNORM_BLOCK,
  };

  static_assert(sizeof(formats) / sizeof(formats[0]) ==
                    static_cast<uint32_t>(data_format::max),
      "Expected the number of vulkan formats and image formats to match");
  WN_DEBUG_ASSERT(_format < data_format::max, "Image format out of bounds");

  return formats[static_cast<uint32_t>(_format)];
}

static inline data_format vulkan_format_to_image_format(VkFormat _format) {
  switch (_format) {
    case VK_FORMAT_R8G8B8A8_UNORM:
      return data_format::r8g8b8a8_unorm;
    case VK_FORMAT_R32G32B32A32_SFLOAT:
      return data_format::r32g32b32a32_sfloat;
    case VK_FORMAT_R32G32B32_SFLOAT:
      return data_format::r32g32b32_sfloat;
    case VK_FORMAT_R32G32_SFLOAT:
      return data_format::r32g32_sfloat;
    case VK_FORMAT_B8G8R8A8_UNORM:
      return data_format::b8g8r8a8_unorm;
    case VK_FORMAT_D16_UNORM:
      return data_format::d16_unorm;
    case VK_FORMAT_X8_D24_UNORM_PACK32:
      return data_format::d24_unorm;
    case VK_FORMAT_D32_SFLOAT:
      return data_format::d32_float;
    case VK_FORMAT_S8_UINT:
      return data_format::s8_unorm;
    case VK_FORMAT_D16_UNORM_S8_UINT:
      return data_format::d16_unorm_s8_uint;
    case VK_FORMAT_D24_UNORM_S8_UINT:
      return data_format::d24_unorm_s8_uint;
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
      return data_format::d32_float_s8_uint;
    case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
      return data_format::astc_4x4;
    case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
      return data_format::astc_5x4;
    case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
      return data_format::astc_5x5;
    case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
      return data_format::astc_6x5;
    case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
      return data_format::astc_6x6;
    case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
      return data_format::astc_8x5;
    case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
      return data_format::astc_8x6;
    case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
      return data_format::astc_8x8;
    case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
      return data_format::astc_10x5;
    case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
      return data_format::astc_10x6;
    case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
      return data_format::astc_10x8;
    case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
      return data_format::astc_10x10;
    case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
      return data_format::astc_12x10;
    case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
      return data_format::astc_12x12;
    case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
      return data_format::bc1_rgb;
    case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
      return data_format::bc1_rgba;
    case VK_FORMAT_BC2_UNORM_BLOCK:
      return data_format::bc2;
    case VK_FORMAT_BC3_UNORM_BLOCK:
      return data_format::bc3;
    default:
      return data_format::max;
  }
}

static inline VkImageAspectFlags image_components_to_aspect(
    image_components _components) {
  VkImageAspectFlags flags{0};

  if (_components & static_cast<uint8_t>(image_component::color)) {
    flags |= VK_IMAGE_ASPECT_COLOR_BIT;
  }
  if (_components & static_cast<uint8_t>(image_component::depth)) {
    flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
  }
  if (_components & static_cast<uint8_t>(image_component::stencil)) {
    flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
  }
  return flags;
}

static inline VkSampleCountFlagBits multi_sampled_to_vulkan(
    multisample_count _count) {
  switch (_count) {
    case multisample_count::samples_1:
      return VK_SAMPLE_COUNT_1_BIT;
    case multisample_count::samples_2:
      return VK_SAMPLE_COUNT_2_BIT;
    case multisample_count::samples_4:
      return VK_SAMPLE_COUNT_4_BIT;
    case multisample_count::samples_8:
      return VK_SAMPLE_COUNT_8_BIT;
    case multisample_count::samples_16:
      return VK_SAMPLE_COUNT_16_BIT;
    case multisample_count::samples_32:
      return VK_SAMPLE_COUNT_32_BIT;
    case multisample_count::samples_64:
      return VK_SAMPLE_COUNT_64_BIT;
    case multisample_count::disabled:
      return VK_SAMPLE_COUNT_1_BIT;
  }
  return VK_SAMPLE_COUNT_1_BIT;
}

static inline VkAttachmentLoadOp load_op_to_vulkan(load_op _load) {
  switch (_load) {
    case load_op::load:
      return VK_ATTACHMENT_LOAD_OP_LOAD;
    case load_op::clear:
      return VK_ATTACHMENT_LOAD_OP_CLEAR;
    case load_op::dont_care:
      return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  }
  return VkAttachmentLoadOp(0);
}

static inline VkAttachmentStoreOp store_op_to_vulkan(store_op _store) {
  switch (_store) {
    case store_op::store:
      return VK_ATTACHMENT_STORE_OP_STORE;
    case store_op::dont_care:
      return VK_ATTACHMENT_STORE_OP_DONT_CARE;
  }
  return VkAttachmentStoreOp(0);
}

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_D3D12_IMAGE_FORMATS_H__
