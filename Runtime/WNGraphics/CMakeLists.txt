set(WN_GRAPHICS_DEVICE_TYPES_AVAILABLE)

enable_overlay_directory()

set(WN_GRAPHICS_DEVICE_TYPES
  ${WN_GRAPHICS_DEVICE_TYPES}
  CACHE STRING "List of device types to include in build"
)

option(WN_GRAPHICS_ALLOW_REFERENCE_DEVICES
  "Whether or not reference devices are allowed"
  ${WN_GRAPHICS_ALLOW_REFERENCE_DEVICES}
)

option(WN_GRAPHICS_ALLOW_DEBUG_MODE
  "Whether or not debug mode is allowed"
  ${WN_GRAPHICS_ALLOW_DEBUG_MODE}
)

string(REPLACE "," ";" WN_GRAPHICS_DEVICE_TYPES "${WN_GRAPHICS_DEVICE_TYPES}")

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

if (${WN_GRAPHICS_ALLOW_REFERENCE_DEVICES})
  message(STATUS "Graphics Reference Devices: Allowed")
  set(_WN_GRAPHICS_ALLOW_REFERENCE_DEVICES true)
else()
  message(STATUS "Graphics Reference Devices: Not Allowed")
endif()

if (${WN_GRAPHICS_ALLOW_DEBUG_MODE})
  message(STATUS "Graphics Debug Mode: Allowed")
  set(_WN_GRAPHICS_ALLOW_DEBUG_MODE true)
else()
  message(STATUS "Graphics Debug Mode: Not Allowed")
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
    inc/WNAdapter.h
    inc/WNCommandAllocator.h
    inc/WNCommandList.h
    inc/WNDevice.h
    inc/WNFactory.h
    inc/WNFence.h
    inc/WNHeap.h
    inc/WNHeapTraits.h
    inc/WNImage.h
    inc/WNQueue.h
    inc/WNResourceStates.h
    src/WNDevice.cpp
    src/WNFactory.cpp
    src/WNQueue.cpp
)

string(REPLACE ";" "," WN_GRAPHICS_DEVICE_TYPES_ENABLED_LIST
  "${WN_GRAPHICS_DEVICE_TYPES_ENABLED}"
)

message(STATUS
  "Graphics Device Types: ${WN_GRAPHICS_DEVICE_TYPES_ENABLED_LIST}"
)

add_subdirectory(test)