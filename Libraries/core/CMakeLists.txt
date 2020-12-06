include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

set(WN_CDECL_BEGIN)
set(WN_CDECL_END)

enable_overlay_directory()

if (WN_CDECL_BEGIN)
  message(STATUS "WN_CDECL_BEGIN Selection: ${WN_CDECL_BEGIN}")
else()
  message(STATUS "WN_CDECL_BEGIN Selection: [none]")
endif()

if (WN_CDECL_END)
  message(STATUS "WN_CDECL_END Selection: ${WN_CDECL_END}")
else()
  message(STATUS "WN_CDECL_END Selection: [none]")
endif()

# set up preprocessor symbols

wn_make_preprocessor_symbol(
  _WN_${WN_ARCHITECTURE_GROUP}
  _WN_ARCHITECTURE
)

wn_make_preprocessor_symbol(
  _WN_${WN_ARCHITECTURE_BITNESS}_BIT
  _WN_ARCHITECTURE_BITNESS
)
wn_make_preprocessor_symbol(
  _WN_ENDIAN_${WN_ARCHITECTURE_ENDIANNESS}
  _WN_ARCHITECTURE_ENDIANNESS
)

wn_make_preprocessor_symbol(
  _WN_${WN_PLATFORM}
  _WN_PLATFORM
)

# generate config file

set(CONFIG_FILE_INPUT_PATH_RELATIVE inc/internal/config.h.in)
set(CONFIG_FILE_OUTPUT_PATH_RELATIVE inc/internal/config.h)

configure_file(
  ${CONFIG_FILE_INPUT_PATH_RELATIVE}
  ${CONFIG_FILE_OUTPUT_PATH_RELATIVE}
)

set(CONFIG_FILE_INPUT_PATH ${CONFIG_FILE_INPUT_PATH_RELATIVE})

set(CONFIG_FILE_OUTPUT_PATH
  ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_OUTPUT_PATH_RELATIVE}
)

# setup target

add_wn_header_library(
  core
  SOURCES
    inc/internal/config.h.in
    inc/internal/errors.inc
    ${CONFIG_FILE_INPUT_PATH}
    ${CONFIG_FILE_OUTPUT_PATH}
    inc/assert.h
    inc/base.h
    inc/endian.h
    inc/macros.h
    inc/optional.h
    inc/pair.h
    inc/tuple.h
    inc/types.h
    inc/type_traits.h
    inc/utilities.h
)

wn_add_subdirectory(test)

post_enable_overlay_directory()
