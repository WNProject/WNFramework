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
#include "WNGraphics/inc/WNSampler.h"
#include "WNGraphics/inc/WNShaderModule.h"
#include "WNGraphics/inc/WNSignal.h"
#include "WNGraphics/inc/WNSurface.h"

namespace wn {
namespace runtime {
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

struct pipeline_layout_data {
  ::VkPipelineLayout layout;
  struct push_constant {
    uint32_t offset_in_bytes;
    uint32_t shader_stages;
  };
  containers::dynamic_array<push_constant> push_constants;
};

template <>
struct data_type<pipeline_layout> {
  using value = memory::unique_ptr<pipeline_layout_data>;
};

template <>
struct data_type<const pipeline_layout> {
  using value = const memory::unique_ptr<pipeline_layout_data>;
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

struct arena_data {
  VkDeviceMemory memory;
  void* root;
};

template <>
struct data_type<arena> {
  using value = arena_data;
};

template <>
struct data_type<framebuffer> {
  using value = ::VkFramebuffer;
};

template <>
struct data_type<const framebuffer> {
  using value = const ::VkFramebuffer;
};

struct graphics_pipeline_data {
  ::VkPipeline pipeline;
};

template <>
struct data_type<graphics_pipeline> {
  using value = graphics_pipeline_data;
};

template <>
struct data_type<const graphics_pipeline> {
  using value = const graphics_pipeline_data;
};

struct buffer_info {
  VkBuffer buffer;
  VkDeviceSize offset;
  VkDeviceMemory bound_arena;
  void* arena_root;
};

template <>
struct data_type<buffer> {
  using value = memory::unique_ptr<buffer_info>;
};

template <>
struct data_type<const buffer> {
  using value = const memory::unique_ptr<const buffer_info>;
};

template <>
struct data_type<image> {
  using value = ::VkImage;
};

template <>
struct data_type<const image> {
  using value = const ::VkImage;
};

template <>
struct data_type<const fence> {
  using value = const ::VkFence;
};

template <>
struct data_type<fence> {
  using value = ::VkFence;
};

template <>
struct data_type<surface> {
  using value = ::VkSurfaceKHR;
};

template <>
struct data_type<sampler> {
  using value = ::VkSampler;
};

template <>
struct data_type<signal> {
  using value = ::VkSemaphore;
};

template <>
struct data_type<const signal> {
  using value = const ::VkSemaphore;
};

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn

#endif  // __WN_GRAPHICS_INTERNAL_VULKAN_DATA_TYPES_H__
