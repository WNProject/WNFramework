// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_GRAPHICS_INTERNAL_VULKAN_DATA_TYPES_H__
#define __WN_GRAPHICS_INTERNAL_VULKAN_DATA_TYPES_H__

#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanInclude.h"
#include "WNGraphics/inc/WNArena.h"
#include "WNGraphics/inc/WNDescriptorData.h"
#include "WNGraphics/inc/WNShaderModule.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

template <typename T>
struct data_type {
  using value = void;
};

template <>
struct data_type<shader_module> {
  using value = ::VkShaderModule;
};
template <>
struct data_type<const shader_module> {
  using value = const ::VkShaderModule;
};

template <>
struct data_type<descriptor_set_layout> {
  using value = ::VkDescriptorSetLayout;
};

template <>
struct data_type<const descriptor_set_layout> {
  using value = const ::VkDescriptorSetLayout;
};

template <>
struct data_type<descriptor_pool> {
  using value = ::VkDescriptorPool;
};
template <>
struct data_type<const descriptor_pool> {
  using value = const ::VkDescriptorPool;
};

struct descriptor_set_data {
  ::VkDescriptorSet set;
  ::VkDescriptorPool pool;
};

template <>
struct data_type<descriptor_set> {
  using value = descriptor_set_data;
};

template <>
struct data_type<const descriptor_set> {
  using value = const descriptor_set_data;
};

template <>
struct data_type<pipeline_layout> {
  using value = ::VkPipelineLayout;
};

template <>
struct data_type<const pipeline_layout> {
  using value = const ::VkPipelineLayout;
};

template <>
struct data_type<render_pass> {
  using value = ::VkRenderPass;
};

template <>
struct data_type<const render_pass> {
  using value = const ::VkRenderPass;
};

template <>
struct data_type<image_view> {
  using value = ::VkImageView;
};

template <>
struct data_type<const image_view> {
  using value = const ::VkImageView;
};

template <>
struct data_type<arena> {
  using value = ::VkDeviceMemory;
};

template <>
struct data_type<framebuffer> {
  using value = ::VkFramebuffer;
};

template <>
struct data_type<const framebuffer> {
  using value = const ::VkFramebuffer;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_DATA_TYPES_H__
