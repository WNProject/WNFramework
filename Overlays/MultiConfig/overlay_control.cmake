set(ENABLED OFF)
set(NUM_CONFIGS -1)
list(LENGTH CMAKE_CONFIGURATION_TYPES num_elements)
if (num_elements GREATER 1)
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} MulitConfig)