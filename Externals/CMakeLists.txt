if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options(
    /wd4100
    /wd4201
    /wd4456
    /wd4457
    /wd4702
    /wd4815
    /wd4459
    /wd4244
  )
endif()

set(RE2_SOURCES
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

add_wn_library(re2
  SOURCES ${RE2_SOURCES}
  INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/re2_helper
    re2
)

add_subdirectory(stb)
add_subdirectory(xxhash)

set(EFFCEE_SOURCES
  effcee/effcee/check.cc
  effcee/effcee/match.cc)

add_wn_library(effcee
  SOURCES ${EFFCEE_SOURCES}
LIBS
    re2
INCLUDES
    re2
    effcee)
