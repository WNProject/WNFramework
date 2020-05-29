append_to_overlay_var(WN_GRAPHICS_DEVICE_TYPES_ENABLED "Vulkan")

overlay_library_sources(WNGraphics
  SOURCES
    inc/Internal/Vulkan/WNAdapter.h
    inc/Internal/Vulkan/WNBufferData.h
    inc/Internal/Vulkan/WNCommandList.h
    inc/Internal/Vulkan/WNDataTypes.h
    inc/Internal/Vulkan/WNDevice.h
    inc/Internal/Vulkan/WNImageFormats.h
    inc/Internal/Vulkan/WNQueue.h
    inc/Internal/Vulkan/WNResourceStates.h
    inc/Internal/Vulkan/WNSwapchain.h
    inc/Internal/Vulkan/WNVulkanCommandListContext.h
    inc/Internal/Vulkan/WNVulkanContext.h
    inc/Internal/Vulkan/WNVulkanInclude.h
    inc/Internal/Vulkan/WNVulkanQueueContext.h
    src/Vulkan/WNAdapter.cpp
    src/Vulkan/WNCommandList.cpp
    src/Vulkan/WNDevice.cpp
    src/Vulkan/WNHelpers.h
    src/Vulkan/WNHelpers.cpp
    src/Vulkan/WNQueue.cpp
    src/Vulkan/WNSwapchain.cpp
  INCLUDES
    ${WNFramework_SOURCE_DIR}/Externals/vulkan/include
)
