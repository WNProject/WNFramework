set(WN_GRAPHICS_SRC)

option(WN_GRAPHICS_DEVICE_TYPES
  "List of device types to include in build" ${WN_GRAPHICS_DEVICE_TYPES}
)

string(REPLACE "," ";" WN_GRAPHICS_DEVICE_TYPES "${WN_GRAPHICS_DEVICE_TYPES}")
set(WN_GRAPHICS_DEVICE_TYPES_AVAILABLE)

if (WN_PLATFORM STREQUAL Windows)
  set(WN_GRAPHICS_INTERNAL_D3D12_SRC
    inc/Internal/D3D12/WNAdapter.h
    inc/Internal/D3D12/WNCommandList.h
    inc/Internal/D3D12/WNDevice.h
    inc/Internal/D3D12/WNFenceData.h
    inc/Internal/D3D12/WNQueue.h
  )

  set(WN_GRAPHICS_D3D12_SRC
    src/D3D12/WNAdapter.cpp
    src/D3D12/WNCommandList.cpp
    src/D3D12/WNDevice.cpp
    src/D3D12/WNHelpers.h
    src/D3D12/WNHelpers.cpp
    src/D3D12/WNQueue.cpp
  )

  list(APPEND WN_GRAPHICS_SRC
    ${WN_GRAPHICS_INTERNAL_D3D12_SRC}
    ${WN_GRAPHICS_D3D12_SRC}
  )

  check_include_file_cxx(d3d12.h HAS_D3D12_H)

  if (HAS_D3D12_H)
    list(APPEND WN_GRAPHICS_DEVICE_TYPES_AVAILABLE d3d12)
  endif()
endif()

list(APPEND WN_GRAPHICS_DEVICE_TYPES_AVAILABLE vulkan)

if (NOT WN_GRAPHICS_DEVICE_TYPES)
  set(WN_GRAPHICS_DEVICE_TYPES_ENABLED ${WN_GRAPHICS_DEVICE_TYPES_AVAILABLE})
else()
  foreach (WN_GRAPHICS_DEVICE_TYPE ${WN_GRAPHICS_DEVICE_TYPES})
    list(FIND WN_GRAPHICS_DEVICE_TYPES_AVAILABLE
      ${WN_GRAPHICS_DEVICE_TYPE} WN_FOUND_DEVICE_TYPE
    )

    if (WN_FOUND_DEVICE_TYPE EQUAL -1)
      message(FATAL_ERROR
        "Device type '${WN_GRAPHICS_DEVICE_TYPE}' not valid")
    endif()
  endforeach()

  set(WN_GRAPHICS_DEVICE_TYPES_ENABLED ${WN_GRAPHICS_DEVICE_TYPES})
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

# generate config file
set(CONFIG_FILE_INPUT_PATH_RELATIVE inc/Internal/WNConfig.h.in)
set(CONFIG_FILE_OUTPUT_PATH_RELATIVE inc/Internal/WNConfig.h)

configure_file(
  ${CONFIG_FILE_INPUT_PATH_RELATIVE}
  ${CONFIG_FILE_OUTPUT_PATH_RELATIVE}
)

set(CONFIG_FILE_INPUT_PATH ${CONFIG_FILE_INPUT_PATH_RELATIVE})

set(CONFIG_FILE_OUTPUT_PATH
  ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_OUTPUT_PATH_RELATIVE}
)

set(WN_GRAPHICS_INTERNAL_SRC
  ${CONFIG_FILE_INPUT_PATH}
  ${CONFIG_FILE_OUTPUT_PATH}
)

set(WN_GRAPHICS_INTERNAL_VULKAN_SRC
  inc/Internal/Vulkan/WNAdapter.h
  inc/Internal/Vulkan/WNBufferData.h
  inc/Internal/Vulkan/WNCommandList.h
  inc/Internal/Vulkan/WNDevice.h
  inc/Internal/Vulkan/WNQueue.h
  inc/Internal/Vulkan/WNVulkanCommandListContext.h
  inc/Internal/Vulkan/WNVulkanContext.h
  inc/Internal/Vulkan/WNVulkanQueueContext.h
)

set(WN_GRAPHICS_VULKAN_SRC
  src/Vulkan/WNAdapter.cpp
  src/Vulkan/WNCommandList.cpp
  src/Vulkan/WNDevice.cpp
  src/Vulkan/WNHelpers.cpp
  src/Vulkan/WNHelpers.h
  src/Vulkan/WNQueue.cpp
)

list(APPEND WN_GRAPHICS_SRC
  ${WN_GRAPHICS_INTERNAL_VULKAN_SRC}
  ${WN_GRAPHICS_INTERNAL_SRC}
  ${WN_GRAPHICS_VULKAN_SRC}
)

add_wn_library(WNGraphics
  SOURCES
    ${WN_GRAPHICS_SRC}
    inc/WNAdapter.h
    inc/WNCommandAllocator.h
    inc/WNCommandList.h
    inc/WNDevice.h
    inc/WNFactory.h
    inc/WNFence.h
    inc/WNHeap.h
    inc/WNHeapTraits.h
    inc/WNQueue.h
    src/WNDevice.cpp
    src/WNFactory.cpp
    src/WNQueue.cpp
)

list(FIND WN_GRAPHICS_DEVICE_TYPES_ENABLED d3d12 WN_FOUND_DEVICE_TYPE)

if (NOT WN_FOUND_DEVICE_TYPE EQUAL -1) # d3d12 is enabled
  wn_target_link_libraries(WNGraphics DXGI.lib D3D12.lib)
else() # d3d12 is not enabled
  set_source_files_properties(
    ${WN_GRAPHICS_D3D12_SRC}
    PROPERTIES HEADER_FILE_ONLY true
  )
endif()

list(FIND WN_GRAPHICS_DEVICE_TYPES_ENABLED vulkan WN_FOUND_DEVICE_TYPE)

if (NOT WN_FOUND_DEVICE_TYPE EQUAL -1) # vulkan is enabled
  target_include_directories(WNGraphics PUBLIC
    ${CMAKE_SOURCE_DIR}/Externals/vulkan
  )

  if(WN_PLATFORM STREQUAL Linux)
    wn_target_link_libraries(WNGraphics dl)
  endif()
else() # vulkan is not enabled
  set_source_files_properties(
    ${WN_GRAPHICS_VULKAN_SRC}
    PROPERTIES HEADER_FILE_ONLY true
  )
endif()

string(REPLACE ";" "," WN_GRAPHICS_DEVICE_TYPES_ENABLED_LIST
  "${WN_GRAPHICS_DEVICE_TYPES_ENABLED}"
)

message(STATUS
  "Graphics Device Types: ${WN_GRAPHICS_DEVICE_TYPES_ENABLED_LIST}"
)

add_subdirectory(test)
