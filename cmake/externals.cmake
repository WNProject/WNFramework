function(get_all_added_targets var)
    set(targets)
    get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
    set(${var} ${targets} PARENT_SCOPE)
endfunction()

macro(get_all_targets_recursive targets dir)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
        get_all_targets_recursive(${targets} ${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    list(APPEND ${targets} ${current_targets})
endmacro()

function(add_wn_externals name)
  if (NOT WN_DISABLE_CXX)
    cmake_parse_arguments(
      PARSED_ARGS
      ""
      "DIRECTORY"
      "CXX_FLAGS"
      ${ARGN}
    )

    add_subdirectory(${PARSED_ARGS_DIRECTORY} ${name})
    get_all_added_targets(EXTERNAL_TARGETS)

    if (PARSED_ARGS_CXX_FLAGS)
      foreach(TARGET ${EXTERNAL_TARGETS})
        target_compile_options(${TARGET} PRIVATE ${PARSED_ARGS_CXX_FLAGS})
      endforeach()
    endif()

    overlay_named_file(cmake/externals.cmake)
  endif()
endfunction()
