add_definitions(-D_WN_X86)

wn_has_architecture(x86-64 HAS)

if (HAS)
    add_definitions(-D_WN_64_BIT)
endif()
