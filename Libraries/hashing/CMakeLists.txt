set(WN_HASHING_XXHASH_COMPILE_OPTIONS)

enable_overlay_directory()

# must force cache override for this to work properly, remove once
# https://github.com/Cyan4973/xxHash/pull/468 merges
set(XXHASH_BUILD_XXHSUM OFF CACHE BOOL "" FORCE)

add_wn_externals(
  xxhash
  DIRECTORY
    ${WNFramework_SOURCE_DIR}/externals/xxhash/cmake_unofficial
  COMPILE_OPTIONS
    ${WN_HASHING_XXHASH_COMPILE_OPTIONS}
)

add_wn_library(
  hashing
  SOURCES
    inc/hash.h
    src/dummy.cpp
  INCLUDES
    ${WNFramework_SOURCE_DIR}/externals
  LIBS
    WNLogging
    xxhash
)

wn_add_subdirectory(test)

post_enable_overlay_directory()
