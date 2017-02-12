set(ENABLED OFF)

if(NOT WN_GRAPHICS_DEVICE_TYPES)
  set(ENABLED ON)
else()
  list(FIND WN_GRAPHICS_DEVICE_TYPES "vulkan" has_vulkan)

  if (has_vulkan GREATER -1)
    set(ENABLED ON)
  endif()
endif()

register_overlay(${ENABLED} Vulkan)