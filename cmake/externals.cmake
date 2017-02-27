function(add_wn_externals name)
  if (NOT WN_DISABLE_CXX)
    cmake_parse_arguments(
      PARSED_ARGS
      ""
      "DIRECTORY"
      "TARGETS"
      ${ARGN})
    
    add_subdirectory(${PARSED_ARGS_DIRECTORY} ${name})
    overlay_named_file(cmake/externals.cmake)
  endif()
endfunction()
