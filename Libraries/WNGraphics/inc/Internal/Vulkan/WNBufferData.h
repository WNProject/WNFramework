// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vulkan.h>

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

struct buffer_data {
  VkDeviceMemory device_memory;
  VkBuffer buffer;
};
} // namespace vulkan
} // namespace internal
} // namespace graphics
} // namespace wn