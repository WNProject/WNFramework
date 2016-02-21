// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/inc/Internal/Vulkan/WNDevice.h"
#include "WNLogging/inc/WNLog.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

device::device(
    memory::allocator* _allocator, WNLogging::WNLog* _log, VkDevice _device)
  : internal::device(_allocator, _log), m_device(_device) {}

#define LOAD_VK_DEVICE_SYMBOL(device, symbol)                                  \
  symbol =                                                                     \
      reinterpret_cast<PFN_##symbol>(vkGetDeviceProcAddr(device, #symbol));    \
  if (!symbol) {                                                               \
    m_log->Log(WNLogging::eError, 0, "Could not find " #symbol ".");           \
    m_log->Log(WNLogging::eError, 0, "Error configuring device");              \
    return false;                                                              \
  }                                                                            \
  m_log->Log(WNLogging::eDebug, 0, #symbol " is at ", symbol);

bool device::initialize(
    vulkan_context* _context, uint32_t graphics_and_device_queue) {
  vkGetDeviceProcAddr =
      reinterpret_cast<PFN_vkGetDeviceProcAddr>(_context->vkGetInstanceProcAddr(
          _context->instance, "vkGetDeviceProcAddr"));
  if (!vkGetDeviceProcAddr) {
    m_log->Log(WNLogging::eError, 0, "Could not find vkGetDeviceProcAddr.");
    m_log->Log(WNLogging::eError, 0, "Device initialization failed");
    return false;
  }
  m_log->Log(
      WNLogging::eDebug, 0, "vkGetDeviceProcAddr is at ", vkGetDeviceProcAddr);

  LOAD_VK_DEVICE_SYMBOL(m_device, vkGetDeviceQueue);

  vkGetDeviceQueue(m_device, graphics_and_device_queue, 0, &m_queue);
  return true;
}

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn
