enable_overlay_directory()

add_wn_library(hashing
  SOURCES
    inc/hash.h
    src/dummy.cpp
  INCLUDES
    ${WNFramework_SOURCE_DIR}/Externals/
  LIBS
    WNLogging
    xxhash
)

wn_add_subdirectory(test)


post_enable_overlay_directory()