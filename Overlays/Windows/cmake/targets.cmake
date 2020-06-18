
function(overload_add_executable name)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    ""
    "SOURCES;LIBS;INCLUDES"
    ${ARGN})
  add_executable(${name}
    ${PARSED_ARGS_SOURCES} 
    ${WNFramework_SOURCE_DIR}/Overlays/Windows/Libraries/executable_entry/src/Application.manifest
  )
  if (PARSED_ARGS_LIBS)
    target_link_libraries(${name} ${PARSED_ARGS_LIBS})
  endif()
  if (PARSED_ARGS_INCLUDES)
    target_include_directories(${name} PUBLIC ${PARSED_ARGS_INCLUDES})
  endif()
endfunction()
