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

static WN_FORCE_INLINE VkFormat image_format_to_vulkan_format(
    data_format _format) {
  static const VkFormat formats[] = {
      VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R32G32B32A32_SFLOAT};
  static_assert(sizeof(formats) / sizeof(formats[0]) ==
                    static_cast<uint32_t>(data_format::max),
      "Expected the number of vulkan formats and image formats to match");
  WN_DEBUG_ASSERT_DESC(
      _format < data_format::max, "Image format out of bounds");

  return formats[static_cast<uint32_t>(_format)];
}

static WN_FORCE_INLINE VkImageAspectFlags image_components_to_aspect(
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

static WN_FORCE_INLINE VkSampleCountFlagBits multi_sampled_to_vulkan(
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
  }
  return VK_SAMPLE_COUNT_1_BIT;
}

static WN_FORCE_INLINE VkAttachmentLoadOp load_op_to_vulkan(load_op _load) {
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

static WN_FORCE_INLINE VkAttachmentStoreOp store_op_to_vulkan(store_op _store) {
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