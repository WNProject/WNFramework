enable_overlay_directory()

set(WN_GRAPHICS_DEVICE_TYPES
  ${WN_GRAPHICS_DEVICE_TYPES}
  CACHE STRING "List of device types to include in build"
)

option(WN_GRAPHICS_ALLOW_REFERENCE_DEVICES
  "Whether or not reference devices are allowed"
  ${WN_GRAPHICS_ALLOW_REFERENCE_DEVICES}
)

set(WN_GRAPHICS_DEBUG_MODE
  ${WN_GRAPHICS_DEBUG_MODE}
  CACHE STRING "Specifies which mode to use to enable debug devices"
)

if (NOT DEFINED WN_GRAPHICS_DEBUG_MODE OR "${WN_GRAPHICS_DEBUG_MODE}" STREQUAL "")
  set(WN_GRAPHICS_DEBUG_MODE Default)
endif()

list(LENGTH WN_GRAPHICS_DEVICE_TYPES_ENABLED
  WN_GRAPHICS_DEVICE_TYPES_AVAILABLE_COUNT
)

foreach(WN_GRAPHICS_DEVICE_TYPE ${WN_GRAPHICS_DEVICE_TYPES_ENABLED})
  string(TOUPPER ${WN_GRAPHICS_DEVICE_TYPE} WN_SANITIZED_GRAPHICS_DEVICE_TYPE)
  set(_WN_GRAPHICS_${WN_SANITIZED_GRAPHICS_DEVICE_TYPE}_DEVICE_TYPE_AVAILABLE
    TRUE
  )
endforeach()

if (WN_GRAPHICS_DEVICE_TYPES_AVAILABLE_COUNT EQUAL 1)
  set(_WN_GRAPHICS_SINGLE_DEVICE_TYPE true)
else()
  set(WN_GRAPHICS_FINAL WN_FINAL)
  set(WN_GRAPHICS_OVERRIDE WN_OVERRIDE)
endif()

string(REPLACE ";" "," WN_GRAPHICS_TYPE_LIST
  "${WN_GRAPHICS_DEVICE_TYPES_ENABLED}")
string(TOLOWER "${WN_GRAPHICS_TYPE_LIST}" WN_GRAPHICS_TYPE_LIST)

set(_WN_GRAPHICS_DEVICE_TYPE ${WN_GRAPHICS_TYPE_LIST})

expand_list(WN_ADAPTER_INCLUDES "#include \"WNGraphics/inc/Internal/"
  "/WNAdapter.h\"" "\n" ${WN_GRAPHICS_DEVICE_TYPES_ENABLED})
expand_list(WN_COMMAND_LIST_INCLUDES "#include \"WNGraphics/inc/Internal/"
  "/WNCommandList.h\"" "\n" ${WN_GRAPHICS_DEVICE_TYPES_ENABLED})
expand_list(WN_DEVICE_INCLUDES "#include \"WNGraphics/inc/Internal/"
  "/WNDevice.h\"" "\n" ${WN_GRAPHICS_DEVICE_TYPES_ENABLED})
expand_list(WN_HELPER_INCLUDES "#include \"WNGraphics/src/"
  "/WNHelpers.h\"" "\n" ${WN_GRAPHICS_DEVICE_TYPES_ENABLED})
expand_list(WN_QUEUE_INCLUDES "#include \"WNGraphics/inc/Internal/"
  "/WNQueue.h\"" "\n" ${WN_GRAPHICS_DEVICE_TYPES_ENABLED})
expand_list(WN_SWAPCHAIN_INCLUDES "#include \"WNGraphics/inc/Internal/"
  "/WNSwapchain.h\"" "\n" ${WN_GRAPHICS_DEVICE_TYPES_ENABLED})

if (${WN_GRAPHICS_ALLOW_REFERENCE_DEVICES})
  message(STATUS "Graphics Reference Devices: Allowed")
  set(_WN_GRAPHICS_ALLOW_REFERENCE_DEVICES true)
else()
  message(STATUS "Graphics Reference Devices: Not Allowed")
endif()

string(TOLOWER ${WN_GRAPHICS_DEBUG_MODE}
  WN_GRAPHICS_DEBUG_MODE_LOWER
)

if (${WN_GRAPHICS_DEBUG_MODE_LOWER} STREQUAL default)
  message(STATUS "Graphics Debug Mode: Default")
  set(_WN_GRAPHICS_DEBUG_MODE 0)
elseif (${WN_GRAPHICS_DEBUG_MODE_LOWER} STREQUAL enabled)
  message(STATUS "Graphics Debug Mode: Enabled")
  set(_WN_GRAPHICS_DEBUG_MODE 1)
elseif (${WN_GRAPHICS_DEBUG_MODE_LOWER} STREQUAL disabled)
  message(STATUS "Graphics Debug Mode: Disabled")
  set(_WN_GRAPHICS_DEBUG_MODE 2)
else()
  message(FATAL_ERROR
    "Invalid graphics debug device option specified: ${WN_GRAPHICS_DEBUG_MODE}"
  )
endif()

configure_file(inc/Internal/WNConfig.h.in
  inc/Internal/WNConfig.h)
configure_file(inc/Internal/WNHelperIncludes.h.in
  inc/Internal/WNHelperIncludes.h)
configure_file(inc/Internal/WNAdapterIncludes.h.in
  inc/Internal/WNAdapterIncludes.h)
configure_file(inc/Internal/WNCommandListIncludes.h.in
  inc/Internal/WNCommandListIncludes.h)
configure_file(inc/Internal/WNDeviceIncludes.h.in
  inc/Internal/WNDeviceIncludes.h)
configure_file(inc/Internal/WNQueueIncludes.h.in
  inc/Internal/WNQueueIncludes.h)
configure_file(inc/Internal/WNSwapchainIncludes.h.in
  inc/Internal/WNSwapchainIncludes.h)

add_wn_library(WNGraphics
  SOURCES
    inc/Internal/WNAdapterIncludes.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNAdapterIncludes.h
    inc/Internal/WNCommandListIncludes.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNCommandListIncludes.h
    inc/Internal/WNConfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNConfig.h
    inc/Internal/WNDeviceIncludes.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNDeviceIncludes.h
    inc/Internal/WNHelperIncludes.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNHelperIncludes.h
    inc/Internal/WNQueueIncludes.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNQueueIncludes.h
    inc/Internal/WNSwapchainIncludes.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/Internal/WNSwapchainIncludes.h
    inc/WNAdapter.h
    inc/WNArena.h
    inc/WNArenaProperties.h
    inc/WNBuffer.h
    inc/WNCommandAllocator.h
    inc/WNCommandList.h
    inc/WNDescriptorData.h
    inc/WNDescriptors.h
    inc/WNDevice.h
    inc/WNFactory.h
    inc/WNFence.h
    inc/WNFramebuffer.h
    inc/WNFramebufferData.h
    inc/WNGraphicsObjectBase.h
    inc/WNImage.h
    inc/WNImageView.h
    inc/WNGraphicsEnums.h
    inc/WNGraphicsPipelineDescription.h
    inc/WNGraphicsPipeline.h
    inc/WNGraphicsTypes.h
    inc/WNQueue.h
    inc/WNRenderPass.h
    inc/WNRenderPassTypes.h
    inc/WNShaderModule.h
    inc/WNSwapchain.h
    src/WNDevice.cpp
    src/WNFactory.cpp
    src/WNQueue.cpp
  LIBS
    WNWindow
)

string(REPLACE ";" "," WN_GRAPHICS_DEVICE_TYPES_ENABLED_LIST
  "${WN_GRAPHICS_DEVICE_TYPES_ENABLED}"
)

message(STATUS
  "Graphics Device Types: ${WN_GRAPHICS_DEVICE_TYPES_ENABLED_LIST}"
)

wn_add_subdirectory(test)

post_enable_overlay_directory()
