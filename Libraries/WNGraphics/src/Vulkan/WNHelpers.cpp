// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
#include <windows.h>
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
}  // anonymous namespace

#define LOAD_VK_SYMBOL(instance, symbol)                                       \
  context->symbol = reinterpret_cast<PFN_##symbol>(                            \
      context->vkGetInstanceProcAddr(instance, #symbol));                      \
  if (!context->vkGetInstanceProcAddr) {                                       \
    _log->Log(WNLogging::eInfo, 0, "Could not find " #symbol ".");             \
    _log->Log(WNLogging::eInfo, 0, "Vulkan will not be available.");           \
    return nullptr;                                                            \
  }                                                                            \
  _log->Log(WNLogging::eDebug, 0, #symbol " is at ",                           \
      context->symbol);

memory::intrusive_ptr<vulkan_context> get_vulkan_context(
    memory::allocator* _allocator, WNLogging::WNLog* _log) {
  vulkan_context_ptr context =
      wn::memory::make_intrusive<vulkan_context>(_allocator, _allocator);
  context->library = open_function(vulkan_library_name);
  if (!context->library) {
    _log->Log(WNLogging::eInfo, 0, "Could not find vulkan loader library.");
    _log->Log(WNLogging::eInfo, 0, "Vulkan will not be available.");
    return nullptr;
  }
  _log->Log(WNLogging::eInfo, 0, "Found vulkan loader library.");

  context->vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
      load_symbol(context->library, "vkGetInstanceProcAddr"));
  if (!context->vkGetInstanceProcAddr) {
    _log->Log(WNLogging::eInfo, 0, "Could not find vkGetInstanceProcAddr.");
    _log->Log(WNLogging::eInfo, 0, "Vulkan will not be available.");
    return nullptr;
  }
  _log->Log(WNLogging::eDebug, 0, "vkGetInstanceProcAddr is at ",
      context->vkGetInstanceProcAddr);

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
    _log->Log(WNLogging::eInfo, 0, "Could not create Vulkan Instance.");
    _log->Log(WNLogging::eInfo, 0, "Vulkan will not be available.");
  }

  _log->Log(WNLogging::eDebug, 0, "Created vkInstance ");
  LOAD_VK_SYMBOL(context->instance, vkDestroyInstance);
  LOAD_VK_SYMBOL(context->instance, vkEnumeratePhysicalDevices);
  LOAD_VK_SYMBOL(context->instance, vkGetPhysicalDeviceProperties);

  return wn::core::move(context);
}

void enumerate_physical_devices(memory::allocator* _allocator,
    WNLogging::WNLog* _log,
    containers::dynamic_array<physical_device_ptr>& _arr) {
  vulkan_context_ptr context = get_vulkan_context(_allocator, _log);
  if (!context) {
    return;
  }
  uint32_t num_physical_devices;
  if (VK_SUCCESS !=
      context->vkEnumeratePhysicalDevices(
          context->instance, &num_physical_devices, nullptr)) {
    _log->Log(WNLogging::eError, 0, "vkEnumeratePhysicalDevices failed");
    return;
  }
  _log->Log(WNLogging::eInfo, 0, "Found: ", num_physical_devices, " devices.");
  containers::dynamic_array<VkPhysicalDevice> devices(
      num_physical_devices, _allocator);

  if (VK_SUCCESS !=
      context->vkEnumeratePhysicalDevices(
          context->instance, &num_physical_devices, devices.data())) {
    _log->Log(WNLogging::eError, 0, "Expected ", num_physical_devices,
        " but result was failure");
    return;
  }

  for (size_t i = 0; i < num_physical_devices; ++i) {
    VkPhysicalDeviceProperties properties;
    context->vkGetPhysicalDeviceProperties(devices[i], &properties);
    _log->Log(WNLogging::eInfo, 0, "Vulkan Device: ", i+1);
    _log->Log(WNLogging::eInfo, 0, "------------------------------");
    _log->Log(WNLogging::eInfo, 0, "Name: ", properties.deviceName);
    _log->Log(WNLogging::eInfo, 0, "Vendor: ", properties.vendorID);
    _log->Log(WNLogging::eInfo, 0, "Device: ", properties.deviceID);
    _log->Log(WNLogging::eInfo, 0, "------------------------------");
    _arr.emplace_back(
        wn::memory::make_unique<physical_device>(_allocator, context,
            devices[i], containers::string(properties.deviceName, _allocator),
            properties.vendorID, properties.deviceID));
  }
}

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace wn