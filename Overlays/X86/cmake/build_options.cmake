add_definitions(-D_WN_X86)
if (WN_ARCHITECTURE STREQUAL "x86-64")
    add_definitions(-D_WN_64_BIT)
endif()
