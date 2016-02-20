set(WN_GRAPHICS_SRC)

set(WN_GRAPHICS_INTERNAL_VULKAN_SRC
  inc/Internal/Vulkan/WNDevice.h
  inc/Internal/Vulkan/WNPhysicalDevice.h
)

set(WN_GRAPHICS_INTERNAL_SRC
  inc/Internal/WNDevice.h
  inc/Internal/WNPhysicalDevice.h
)

set(WN_GRAPHICS_VULKAN_SRC
  src/Vulkan/WNDevice.cpp
)

source_group("inc\\Internal" FILES ${WN_GRAPHICS_INTERNAL_SRC})
source_group("inc\\Internal\\Vulkan" FILES ${WN_GRAPHICS_INTERNAL_VULKAN_SRC})
source_group("src\\Vulkan" FILES ${WN_GRAPHICS_VULKAN_SRC})

list(APPEND WN_GRAPHICS_SRC
  ${WN_GRAPHICS_INTERNAL_VULKAN_SRC}
  ${WN_GRAPHICS_INTERNAL_SRC}
  ${WN_GRAPHICS_VULKAN_SRC}
)

if (WN_PLATFORM STREQUAL Windows)
  set(WN_GRAPHICS_INTERNAL_D3D12_SRC
    inc/Internal/D3D12/WNDevice.h
    inc/Internal/D3D12/WNPhysicalDevice.h
  )

  set(WN_GRAPHICS_D3D12_SRC
    src/D3D12/WNDevice.cpp
  )

  list(APPEND WN_GRAPHICS_SRC
    ${WN_GRAPHICS_INTERNAL_D3D12_SRC}
    ${WN_GRAPHICS_D3D12_SRC}
  )

  source_group("inc\\Internal\\D3D12" FILES ${WN_GRAPHICS_INTERNAL_D3D12_SRC})
  source_group("src\\D3D12" FILES ${WN_GRAPHICS_D3D12_SRC})
endif()

add_wn_library(WNGraphics
  SOURCES
    ${WN_GRAPHICS_SRC}
    inc/WNDevice.h
    inc/WNFactory.h
    inc/WNPhysicalDevice.h
    src/WNFactory.cpp
)