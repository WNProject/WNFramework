// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNGraphics/src/Vulkan/WNHelpers.h"

#include "WNGraphics/inc/Internal/Vulkan/WNAdapter.h"
#include "WNGraphics/inc/Internal/Vulkan/WNVulkanContext.h"
#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMemory/inc/allocator.h"
#include "WNMemory/inc/intrusive_ptr.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace internal {
namespace vulkan {

#ifdef _WN_WINDOWS
using library_type = HMODULE;
library_type open_function(const char* _name) {
  return LoadLibrary(_name);
}
const char* vulkan_library_name = "vulkan-1.dll";
const char* backup_vulkan_libray_name = "vulkan.dll";
void* load_symbol(library_type _lib, const char* _symbol) {
  return GetProcAddress(_lib, _symbol);
}
#elif defined _WN_MACOS
#include <dlfcn.h>
library_type open_function(const char* _name) {
  return dlopen(_name, RTLD_NOW);
}
const char* vulkan_library_name = "libvulkan.dylib";
const char* backup_vulkan_libray_name = "libvulkan.dylib";
void* load_symbol(library_type _lib, const char* _symbol) {
  return dlsym(_lib, _symbol);
}
#else
#include <dlfcn.h>
library_type open_function(const char* _name) {
  return dlopen(_name, RTLD_NOW);
}
const char* vulkan_library_name = "libvulkan.so.1";
const char* backup_vulkan_libray_name = "libvulkan.so";
void* load_symbol(library_type _lib, const char* _symbol) {
  return dlsym(_lib, _symbol);
}
#endif

namespace {

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

#ifdef _WN_GRAPHICS_ALLOW_DEBUG_MODE
VKAPI_ATTR VkBool32 VKAPI_CALL debug_report_cb(VkDebugReportFlagsEXT,
    VkDebugReportObjectTypeEXT, uint64_t, size_t, int32_t,
    const char* _layer_prefix, const char* _message, void* _user_data) {
  wn::logging::log* _log = static_cast<wn::logging::log*>(_user_data);
  _log->log_error(_layer_prefix, ": ", _message);
  _log->flush();
  return VK_FALSE;
}
#endif

memory::intrusive_ptr<vulkan_context> get_vulkan_context(
    memory::allocator* _allocator, logging::log* _log) {
  vulkan_context_ptr context =
      wn::memory::make_intrusive<vulkan_context>(_allocator, _allocator);
  context->library = open_function(vulkan_library_name);
  if (!context->library) {
    context->library = open_function(backup_vulkan_libray_name);
  }
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

  uint32_t num_extensions = 2;
  const char* instance_extensions[] = {VK_KHR_SURFACE_EXTENSION_NAME,
      VK_SURFACE_EXTENSION, "VK_EXT_debug_report"};
  uint32_t num_layers = 0;
  const char* layers[] = {"VK_LAYER_KHRONOS_validation"};

#ifdef _WN_GRAPHICS_ALLOW_DEBUG_MODE
  num_extensions = 3;
  num_layers = 1;
  bool has_debug_layers = true;
#endif

  VkInstanceCreateInfo create_info{
      VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,  // sType
      nullptr,                                 // pNext;
      0,                                       // flags;
      &app_info,                               // pApplicationInfo;
      num_layers,                              // enabledLayerCount;
      layers,                                  // ppEnabledLayerNames;
      num_extensions,                          // enabledExtensionCount;
      instance_extensions,                     // ppEnabledExtensionNames;
  };

  VkResult ret =
      context->vkCreateInstance(&create_info, nullptr, &context->instance);
#ifdef _WN_GRAPHICS_ALLOW_DEBUG_MODE
  if (ret == VK_ERROR_EXTENSION_NOT_PRESENT ||
      ret == VK_ERROR_LAYER_NOT_PRESENT) {
    has_debug_layers = false;
    create_info.enabledLayerCount = 0;
    create_info.enabledExtensionCount = 2;
    ret = context->vkCreateInstance(&create_info, nullptr, &context->instance);
  }
#endif

  if (ret != VK_SUCCESS || context->instance == VK_NULL_HANDLE) {
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
  LOAD_VK_SYMBOL(context->instance, vkGetPhysicalDeviceSurfaceSupportKHR);
  LOAD_VK_SYMBOL(context->instance, vkGetPhysicalDeviceSurfaceFormatsKHR);
  LOAD_VK_SYMBOL(context->instance, vkGetPhysicalDeviceSurfaceCapabilitiesKHR);

  LOAD_VK_SYMBOL(context->instance, vkGetPhysicalDeviceFeatures);
  LOAD_VK_SYMBOL(context->instance, vkGetPhysicalDeviceFormatProperties);

#ifdef _WN_GRAPHICS_ALLOW_DEBUG_MODE
  if (has_debug_layers) {
    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
        reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
            context->vkGetInstanceProcAddr(
                context->instance, "vkCreateDebugReportCallbackEXT"));

    VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {
        VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT,  // sType
        nullptr,                                         // pNext
        VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
            VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,  // flags
        &debug_report_cb,                                 // callback
        _log,                                             // pUserData
    };
    VkDebugReportCallbackEXT cb;
    // We never actually destroy our instance, so we can avoid cleaning up
    // the callback.
    vkCreateDebugReportCallbackEXT(
        context->instance, &callbackCreateInfo, nullptr, &cb);
  }
#endif
  return context;
}

void enumerate_adapters(memory::allocator* _allocator, logging::log* _log,
    containers::dynamic_array<adapter_ptr>& _arr) {
  vulkan_context_ptr context = get_vulkan_context(_allocator, _log);
  if (!context) {
    return;
  }
  uint32_t num_physical_devices;
  if (VK_SUCCESS != context->vkEnumeratePhysicalDevices(
                        context->instance, &num_physical_devices, nullptr)) {
    _log->log_warning("vkEnumeratePhysicalDevices found no devices");
    return;
  }
  _log->log_info("Found: ", num_physical_devices, " devices.");
  containers::dynamic_array<VkPhysicalDevice> devices(
      _allocator, num_physical_devices);

  if (VK_SUCCESS != context->vkEnumeratePhysicalDevices(context->instance,
                        &num_physical_devices, devices.data())) {
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
        _allocator, num_queue_families);

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
      ptr->initialize(_allocator, context, devices[i],
          containers::string(_allocator, properties.deviceName), _log,
          properties.vendorID, properties.deviceID, graphics_and_compute_queue);
      ptr->initialize_device();
    }

    _arr.emplace_back(core::move(ptr));
  }
}

}  // namespace vulkan
}  // namespace internal
}  // namespace graphics
}  // namespace runtime
}  // namespace wn
