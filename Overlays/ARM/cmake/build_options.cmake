add_definitions(-D_WN_ARM)
if (WN_ARCHITECTURE STREQUAL "ARM64")
    add_definitions(-D_WN_64_BIT)
endif()
