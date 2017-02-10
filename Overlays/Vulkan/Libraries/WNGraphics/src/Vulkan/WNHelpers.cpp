// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/Vulkan/WNAdapter.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanContext.h"
#include "WNGraphics/src/Vulkan/WNHelpers.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNIntrusivePtr.h"

namespace wn {
namespace graphics {
namespace internal {
namespace vulkan {

#ifdef _WN_WINDOWS
using library_type = HMODULE;
library_type open_function(const char* _name) {
  return LoadLibrary(_name);
}
const char* vulkan_library_name = "vulkan-1.dll";
void* load_symbol(library_type _lib, const char* _symbol) {
  return GetProcAddress(_lib, _symbol);
}
#else
#include <dlfcn.h>
library_type open_function(const char* _name) {
  return dlopen(_name, RTLD_NOW);
}
const char* vulkan_library_name = "libvulkan.so";
void* load_symbol(library_type _lib, const char* _symbol) {
  return dlsym(_lib, _symbol);
}
#endif

namespace {

void* VKAPI_CALL allocate(void* _user_data, size_t _size, size_t _alignment,
    VkSystemAllocationScope) {
  memory::allocator* alloc = reinterpret_cast<memory::allocator*>(_user_data);
  return alloc->aligned_allocate(_size, _alignment);
}

void* VKAPI_CALL reallocate(void* _user_data, void* _original, size_t _size,
    size_t _alignment, VkSystemAllocationScope) {
  memory::allocator* alloc = reinterpret_cast<memory::allocator*>(_user_data);
  return alloc->aligned_reallocate(_original, _size, _alignment);
}

void VKAPI_CALL free(void* _user_data, void* _data) {
  memory::allocator* alloc = reinterpret_cast<memory::allocator*>(_user_data);
  return alloc->aligned_deallocate(_data);
}

#ifndef _WN_GRAPHICS_SINGLE_DEVICE_TYPE
using vulkan_adapter_constructable = vulkan_adapter;
#else
using vulkan_adapter_constructable = adapter;
#endif

}  // anonymous namespace

#define LOAD_VK_SYMBOL(instance, symbol)                                       \
  context->symbol = reinterpret_cast<PFN_##symbol>(                            \
      context->vkGetInstanceProcAddr(instance, #symbol));                      \
  if (!context->vkGetInstanceProcAddr) {                                       \
    _log->log_info("Could not find " #symbol ".");                             \
    _log->log_info("Vulkan will not be available.");                           \
    return nullptr;                                                            \
  }                                                                            \
  _log->log_debug(#symbol " is at ", context->symbol);

memory::intrusive_ptr<vulkan_context> get_vulkan_context(
    memory::allocator* _allocator, logging::log* _log) {
  vulkan_context_ptr context =
      wn::memory::make_intrusive<vulkan_context>(_allocator, _allocator);
  context->library = open_function(vulkan_library_name);
  if (!context->library) {
    _log->log_info("Could not find vulkan loader library.");
    _log->log_info("Vulkan will not be available.");
    return nullptr;
  }
  _log->log_info("Found vulkan loader library.");

  context->vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
      load_symbol(context->library, "vkGetInstanceProcAddr"));
  if (!context->vkGetInstanceProcAddr) {
    _log->log_info("Could not find vkGetInstanceProcAddr.");
    _log->log_info("Vulkan will not be available.");
    return nullptr;
  }
  _log->log_debug(
      "vkGetInstanceProcAddr is at ", context->vkGetInstanceProcAddr);

  LOAD_VK_SYMBOL(nullptr, vkCreateInstance);
  VkApplicationInfo app_info{
      VK_STRUCTURE_TYPE_APPLICATION_INFO,  // sType;
      nullptr,                             // pNext;
      "",                                  // pApplicationName;
      0,                                   // applicationVersion;
      "",                                  // pEngineName;
      0,                                   // engineVersion;
      VK_MAKE_VERSION(1, 0, 0)             // apiVersion;
  };

  VkInstanceCreateInfo create_info{
      VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,  // sType
      nullptr,                                 // pNext;
      0,                                       // flags;
      &app_info,                               // pApplicationInfo;
      0,                                       // enabledLayerCount;
      nullptr,                                 // ppEnabledLayerNames;
      0,                                       // enabledExtensionCount;
      nullptr,                                 // ppEnabledExtensionNames;
  };

  if (context->vkCreateInstance(&create_info, nullptr, &context->instance) !=
          VK_SUCCESS ||
      context->instance == VK_NULL_HANDLE) {
    _log->log_info("Could not create Vulkan Instance.");
    _log->log_info("Vulkan will not be available.");
    return nullptr;
  }

  _log->log_debug("Created vkInstance ");
  LOAD_VK_SYMBOL(context->instance, vkDestroyInstance);
  LOAD_VK_SYMBOL(context->instance, vkEnumeratePhysicalDevices);
  LOAD_VK_SYMBOL(context->instance, vkGetPhysicalDeviceProperties);
  LOAD_VK_SYMBOL(context->instance, vkGetPhysicalDeviceQueueFamilyProperties);

  LOAD_VK_SYMBOL(context->instance, vkCreateDevice);
  LOAD_VK_SYMBOL(context->instance, vkDestroyDevice);

  LOAD_VK_SYMBOL(context->instance, vkGetPhysicalDeviceMemoryProperties);
  return wn::core::move(context);
}

void enumerate_adapters(memory::allocator* _allocator, logging::log* _log,
    containers::dynamic_array<adapter_ptr>& _arr) {
  vulkan_context_ptr context = get_vulkan_context(_allocator, _log);
  if (!context) {
    return;
  }
  uint32_t num_physical_devices;
  if (VK_SUCCESS !=
      context->vkEnumeratePhysicalDevices(
          context->instance, &num_physical_devices, nullptr)) {
    _log->log_error("vkEnumeratePhysicalDevices failed");
    return;
  }
  _log->log_info("Found: ", num_physical_devices, " devices.");
  containers::dynamic_array<VkPhysicalDevice> devices(
      num_physical_devices, _allocator);

  if (VK_SUCCESS !=
      context->vkEnumeratePhysicalDevices(
          context->instance, &num_physical_devices, devices.data())) {
    _log->log_error(
        "Expected ", num_physical_devices, " but result was failure");
    return;
  }

  for (size_t i = 0; i < num_physical_devices; ++i) {
    VkPhysicalDeviceProperties properties;
    context->vkGetPhysicalDeviceProperties(devices[i], &properties);
    _log->log_info("Vulkan Device: ", i + 1);
    _log->log_info("--------------------------------");
    _log->log_info("Name: ", properties.deviceName);
    _log->log_info("Vendor: ", properties.vendorID);
    _log->log_info("Device: ", properties.deviceID);

    uint32_t num_queue_families;

    context->vkGetPhysicalDeviceQueueFamilyProperties(
        devices[i], &num_queue_families, nullptr);
    _log->log_info("Device supports ", num_queue_families, " queue types.");

    containers::dynamic_array<VkQueueFamilyProperties> queue_properties(
        num_queue_families, _allocator);

    context->vkGetPhysicalDeviceQueueFamilyProperties(
        devices[i], &num_queue_families, queue_properties.data());

    uint32_t graphics_and_compute_queue = static_cast<uint32_t>(-1);

    for (uint32_t j = 0; j < num_queue_families; ++j) {
      _log->log_info("Queue type ", j + 1);
      _log->log_info("--------------------------------");
      _log->log_info("Num Queues: ", queue_properties[j].queueCount);
      _log->log_info("Bits:       ", queue_properties[j].queueFlags);
      _log->log_info("--------------------------------");
      // We only care about graphics && compute devices.
      if (graphics_and_compute_queue == static_cast<uint32_t>(-1) &&
          ((queue_properties[j].queueFlags &
               (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT)) ==
              (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT))) {
        graphics_and_compute_queue = j;
      }
    }
    if (graphics_and_compute_queue == static_cast<uint32_t>(-1)) {
      _log->log_info("Does not support graphics and compute, ignoring");
    }
    _log->log_info("--------------------------------");

    memory::unique_ptr<vulkan_adapter_constructable> ptr(
        memory::make_unique_delegated<vulkan_adapter_constructable>(
            _allocator, [](void* _memory) {
              return new (_memory) vulkan_adapter_constructable();
            }));

    if (ptr) {
      ptr->initialize(context, devices[i],
          containers::string(properties.deviceName, _allocator),
          properties.vendorID, properties.deviceID, graphics_and_compute_queue);
      ptr->initialize_device();
    }

    _arr.emplace_back(core::move(ptr));
  }
}

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn
