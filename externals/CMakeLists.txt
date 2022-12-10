set(WN_ADDITIONAL_TRACY_OPTIONS)
set(WN_ADDITIONAL_FREETYPE_OPTIONS)
set(WN_RLOTTIE_ADDITIONAL_OPTIONS)
set(ARCH ) #rlottie uses this define
find_program(GIT NAMES git)
execute_process(
  COMMAND ${GIT} apply --check -p0 -
  INPUT_FILE ./externals/.patches/antlr3.patch
  WORKING_DIRECTORY ${WNFramework_SOURCE_DIR}
  RESULT_VARIABLE PATCH_APPLY_CHECK_RESULT
  OUTPUT_QUIET
  ERROR_QUIET
)

if (PATCH_APPLY_CHECK_RESULT MATCHES 0)
  execute_process(
    COMMAND ${GIT} apply -p0 -
    INPUT_FILE ./externals/.patches/antlr3.patch
    WORKING_DIRECTORY ${WNFramework_SOURCE_DIR}
    RESULT_VARIABLE PATCH_APPLY_RESULT
    OUTPUT_QUIET
    ERROR_QUIET
  )

  if (PATCH_APPLY_RESULT MATCHES 0)
    message(STATUS "Git patched: antlr3")
  else()
    message(FATAL_ERROR "Git failed to patch: antlr3")
  endif()
else()
    message(STATUS "Git already patched: antlr3")
endif()

execute_process(
  COMMAND ${GIT} apply --check -p0 -
  INPUT_FILE ./externals/.patches/rmlui.patch
  WORKING_DIRECTORY ${WNFramework_SOURCE_DIR}
  RESULT_VARIABLE PATCH_APPLY_CHECK_RESULT
  OUTPUT_QUIET
  ERROR_QUIET
)

if (PATCH_APPLY_CHECK_RESULT MATCHES 0)
  execute_process(
    COMMAND ${GIT} apply -p0 -
    INPUT_FILE ./externals/.patches/rmlui.patch
    WORKING_DIRECTORY ${WNFramework_SOURCE_DIR}
    RESULT_VARIABLE PATCH_APPLY_RESULT
    OUTPUT_QUIET
    ERROR_QUIET
  )

  if (PATCH_APPLY_RESULT MATCHES 0)
    message(STATUS "Git patched: rmlui")
  else()
    message(FATAL_ERROR "Git failed to patch: rmlui")
  endif()
else()
    message(STATUS "Git already patched: rmlui")
endif()


enable_overlay_directory()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options(
    /wd4100
    /wd4201
    /wd4456
    /wd4457
    /wd4702
    /wd4815
    /wd4459
    /wd4244
    /wd4127
  )
else()
  add_compile_options(
    -Wno-unused-variable
    -Wno-format
  )
endif()

set(
  RE2_SOURCES
  re2/re2/bitstate.cc
  re2/re2/compile.cc
  re2/re2/dfa.cc
  re2/re2/filtered_re2.cc
  re2/re2/mimics_pcre.cc
  re2/re2/nfa.cc
  re2/re2/onepass.cc
  re2/re2/parse.cc
  re2/re2/perl_groups.cc
  re2/re2/prefilter.cc
  re2/re2/prefilter_tree.cc
  re2/re2/prog.cc
  re2/re2/re2.cc
  re2/re2/regexp.cc
  re2/re2/set.cc
  re2/re2/simplify.cc
  re2/re2/stringpiece.cc
  re2/re2/tostring.cc
  re2/re2/unicode_casefold.cc
  re2/re2/unicode_groups.cc
  re2/util/rune.cc
  re2/util/strutil.cc
)

add_wn_library(
  re2
  SOURCES
    ${RE2_SOURCES}
  INCLUDES
    re2
)

set(
  EFFCEE_SOURCES
  effcee/effcee/check.cc
  effcee/effcee/match.cc
)

add_wn_library(
  effcee
  SOURCES
    ${EFFCEE_SOURCES}
  INCLUDES
    re2
    effcee
  LIBS
    re2
)

set(RAPIDJSON_SOURCES
  rapidjson/include/rapidjson/allocators.h
  rapidjson/include/rapidjson/cursorstreamwrapper.h
  rapidjson/include/rapidjson/document.h
  rapidjson/include/rapidjson/encodedstream.h
  rapidjson/include/rapidjson/encodings.h
  rapidjson/include/rapidjson/filereadstream.h
  rapidjson/include/rapidjson/filewritestream.h
  rapidjson/include/rapidjson/fwd.h
  rapidjson/include/rapidjson/istreamwrapper.h
  rapidjson/include/rapidjson/memorybuffer.h
  rapidjson/include/rapidjson/memorystream.h
  rapidjson/include/rapidjson/ostreamwrapper.h
  rapidjson/include/rapidjson/pointer.h
  rapidjson/include/rapidjson/prettywriter.h
  rapidjson/include/rapidjson/rapidjson.h
  rapidjson/include/rapidjson/reader.h
  rapidjson/include/rapidjson/schema.h
  rapidjson/include/rapidjson/stream.h
  rapidjson/include/rapidjson/stringbuffer.h
  rapidjson/include/rapidjson/writer.h
  rapidjson/include/rapidjson/error/en.h
  rapidjson/include/rapidjson/error/error.h
  rapidjson/include/rapidjson/internal/biginteger.h
  rapidjson/include/rapidjson/internal/clzll.h
  rapidjson/include/rapidjson/internal/diyfp.h
  rapidjson/include/rapidjson/internal/dtoa.h
  rapidjson/include/rapidjson/internal/ieee754.h
  rapidjson/include/rapidjson/internal/itoa.h
  rapidjson/include/rapidjson/internal/meta.h
  rapidjson/include/rapidjson/internal/pow10.h
  rapidjson/include/rapidjson/internal/regex.h
  rapidjson/include/rapidjson/internal/stack.h
  rapidjson/include/rapidjson/internal/strfunc.h
  rapidjson/include/rapidjson/internal/strtod.h
  rapidjson/include/rapidjson/internal/swap.h
  rapidjson/include/rapidjson/msinttypes/inttypes.h
  rapidjson/include/rapidjson/msinttypes/stdint.h
)

add_wn_header_library(
  rapidjson
  SOURCES
    ${RAPIDJSON_SOURCES}
  INCLUDES
    rapidjson/include
)

set(TRACY_OPTIONS)
if(WN_ENABLE_TRACY)
  set(TRACY_OPTIONS TRACY_ENABLE)
endif()

add_wn_library(
  tracy_client
  SOURCES
    tracy/TracyClient.cpp
  INCLUDES
    tracy
  PUBLIC_COMPILE_DEFINES
    ${TRACY_OPTIONS}
  COMPILE_OPTIONS
    -DENABLE_PRELOAD
    -DTRACY_TIMER_FALLBACK
)

add_wn_externals(
  marl
  DIRECTORY
    marl
)

add_wn_externals(
  rlottie
    DIRECTORY
    rlottie
  COMPILE_OPTIONS
    ${WN_RLOTTIE_ADDITIONAL_OPTIONS}
)

add_wn_externals(
  freetype
    DIRECTORY
    freetype
  COMPILE_OPTIONS
    ${WN_ADDITIONAL_FREETYPE_OPTIONS}
)

include(RmlUI.txt)

post_enable_overlay_directory()
