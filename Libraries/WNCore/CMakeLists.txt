include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

set(WN_CDECL_BEGIN)
set(WN_CDECL_END)

enable_overlay_directory()

# run specifier tests

wn_check_cxx_source_compiles(
  "
    __forceinline void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___FORCE_INLINE
)

wn_check_cxx_source_compiles(
  "
    inline __attribute__((always_inline)) void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___ATTRIBUTE__ALWAYS_INLINE
)

wn_check_cxx_source_compiles(
  "
    __declspec(noinline) void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___DECLSPEC_NOINLINE
)

wn_check_cxx_source_compiles(
  "
    __attribute__((noinline)) void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___ATTRIBUTE__NOINLINE
)

wn_check_cxx_source_compiles(
  "
    __declspec(selectany) int temp = 0;
    int main() {
      return temp;
    }
  "
  HAS___DECLSPEC_SELECTANY
)

wn_check_cxx_source_compiles(
  "
    __attribute__((weak)) int temp = 0;
    int main() {
      return temp;
    }
  "
  HAS___ATTRIBUTE__WEAK
)


# make specifier selections

set(WN_FORCE_INLINE)
set(WN_NO_INLINE)
set(WN_WEAK_LINK)

if (HAS___ATTRIBUTE__ALWAYS_INLINE)
  set(WN_FORCE_INLINE "inline __attribute__((always_inline))")
elseif (HAS___FORCE_INLINE)
  set(WN_FORCE_INLINE __forceinline)
else()
  set(WN_FORCE_INLINE inline)
endif()

if (HAS___ATTRIBUTE__NOINLINE)
  set(WN_NO_INLINE "__attribute__((noinline))")
elseif (HAS___DECLSPEC_NOINLINE)
  set(WN_NO_INLINE "__declspec(noinline)")
endif()

if (HAS___ATTRIBUTE__WEAK)
  set(WN_WEAK_LINK "__attribute__((weak))")
elseif (HAS___DECLSPEC_SELECTANY)
  set(WN_WEAK_LINK "__declspec(selectany)")
elseif(NOT WN_DISABLE_CXX)
  message(FATAL_ERROR "No weak link style keyword was found")
endif()

# print selections

message(STATUS "WN_FORCE_INLINE Selection: ${WN_FORCE_INLINE}")

if (WN_NO_INLINE)
  message(STATUS "WN_NO_INLINE Selection: ${WN_NO_INLINE}")
else()
  message(STATUS "WN_NO_INLINE Selection: [none]")
endif()

message(STATUS "WN_WEAK_LINK Selection: ${WN_WEAK_LINK}")

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
  WNCore
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
