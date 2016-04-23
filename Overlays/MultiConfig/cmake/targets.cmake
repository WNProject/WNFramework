function(overload_add_header_library name)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    ""
    "SOURCES"
    ${ARGN})

  if (NOT PARSED_ARGS_SOURCES)
    message(FATAL_ERROR "Must supply at least one source file for ${name}")
  endif()

  overlay_named_file(cmake/target_functions/pre_add_library.cmake)
  overload_add_library(${name} SOURCES ${PARSED_ARGS_SOURCES})
  overlay_named_file(cmake/target_functions/post_add_library.cmake)
  set_property(TARGET ${name} PROPERTY LINKER_LANGUAGE CXX)
endfunction()
