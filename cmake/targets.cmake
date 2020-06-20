macro(_overlay_add_enabled_sources name)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    ""
    "SOURCES;LIBS;COMPILE_OPTIONS;INCLUDES"
    ${ARGN})

  set(local_overlay_sources)
  set(local_overlay_logical_sources)
  set(local_overlay_rooted_sources)
  set(local_new_sources "")
  set(local_replaced_sources "")

  set(local_removed_sources ${${name}_OVERLAY_REMOVED_SOURCES})
  set(local_removed_logical_sources ${${name}_OVERLAY_REMOVED_LOGICAL_SOURCES})
  set(local_removed_overlay_rooted_sources
    ${${name}_OVERLAY_REMOVED_ROOTED_SOURCES})

  set(HAS_EXISTING_SOURCES OFF)

  if (${name}_OVERLAY_LOGICAL_SOURCES)
    LIST(LENGTH ${name}_OVERLAY_LOGICAL_SOURCES num_local_existing_sources)
    math(EXPR num_local_existing_sources "${num_local_existing_sources} - 1")
    set(HAS_EXISTING_SOURCES ON)
  else()
    set(num_local_existing_sources -1)
  endif()

  foreach(source ${PARSED_ARGS_SOURCES})
      list(FIND ${name}_OVERLAY_LOGICAL_SOURCES ${source} existing_index)
      if (existing_index EQUAL -1)
        LIST(APPEND local_new_sources ${source})
      else()
        LIST(APPEND local_replaced_sources ${source})
      endif()
  endforeach()

  if (${HAS_EXISTING_SOURCES})
    foreach(index RANGE 0 ${num_local_existing_sources})
      list(GET ${name}_OVERLAY_LOGICAL_SOURCES ${index} existing_source)
      list(GET ${name}_OVERLAY_SOURCES ${index} existing_full_source)
      list(GET ${name}_OVERLAY_ROOTED_SOURCES ${index} existing_rooted_overlay)
      set(replaced_index -1)
      if (local_replaced_sources)
        list(FIND local_replaced_sources ${existing_source} replaced_index)
      endif()

      if (replaced_index EQUAL -1)
        list(APPEND local_overlay_sources ${existing_full_source})
        list(APPEND local_overlay_logical_sources ${existing_source})
        list(APPEND local_overlay_rooted_sources
          ${existing_rooted_overlay})
      else()
        list(APPEND local_removed_sources ${existing_full_source})
        list(APPEND local_removed_logical_sources ${existing_source})
        list(APPEND local_removed_overlay_logical_sources
          ${existing_rooted_overlay})
      endif()
    endforeach()
  endif()

  foreach(source ${local_new_sources})
    if (IS_ABSOLUTE ${source})
      list(APPEND local_overlay_sources ${source})
    else()
      list(APPEND local_overlay_sources ${CMAKE_CURRENT_SOURCE_DIR}/${source})
    endif()
    list(APPEND local_overlay_logical_sources ${source})
    merge_paths(named_path ${WN_CURRENT_OVERLAY} ${source})
    list(APPEND local_overlay_rooted_sources ${named_path})
  endforeach()

  foreach(source ${local_replaced_sources})
    if (IS_ABSOLUTE ${source})
      list(APPEND local_overlay_sources ${source})
    else()
      list(APPEND local_overlay_sources ${CMAKE_CURRENT_SOURCE_DIR}/${source})
    endif()
    list(APPEND local_overlay_logical_sources ${source})
    merge_paths(named_path ${WN_CURRENT_OVERLAY} ${source})
    list(APPEND local_overlay_rooted_sources ${named_path})
  endforeach()

  set(compile_opts ${${name}_OVERLAY_COMPILE_OPTIONS})
  foreach(opt ${PARSED_ARGS_COMPILE_OPTIONS})
    list(APPEND compile_opts ${opt})
  endforeach()

  set(include_directories ${${name}_OVERLAY_INCLUDES})
  foreach(inc ${PARSED_ARGS_INCLUDES})
    list(APPEND include_directories ${inc})
  endforeach()

  set(local_libs ${${name}_OVERLAY_LIBS})
  foreach(lib ${PARSED_ARGS_LIBS})
    list(APPEND local_libs ${lib})
  endforeach()

  set(local_removed_sources ${${name}_OVERLAY_REMOVED_SOURCES})

  set(${name}_OVERLAY_REMOVED_SOURCES ${local_removed_sources} PARENT_SCOPE)
  set(${name}_OVERLAY_REMOVED_LOGICAL_SOURCES ${local_removed_logical_sources}
    PARENT_SCOPE)
  set(${name}_OVERLAY_REMOVED_ROOTED_SOURCES
    ${local_removed_overlay_logical_sources} PARENT_SCOPE)

  set(${name}_OVERLAY_LIBS ${local_libs} PARENT_SCOPE)
  set(${name}_OVERLAY_COMPILE_OPTIONS ${compile_opts} PARENT_SCOPE)
  set(${name}_OVERLAY_INCLUDES ${include_directories} PARENT_SCOPE)

  set(${name}_OVERLAY_SOURCES ${local_overlay_sources} PARENT_SCOPE)
  set(${name}_OVERLAY_LOGICAL_SOURCES ${local_overlay_logical_sources}
    PARENT_SCOPE)
  set(${name}_OVERLAY_ROOTED_SOURCES ${local_overlay_rooted_sources}
    PARENT_SCOPE)
endmacro()

macro(_overlay_add_disabled_sources name)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    ""
    "SOURCES;LIBS;COMPILE_OPTIONS;INCLUDES"
    ${ARGN})

  set(local_overlay_sources ${${name}_OVERLAY_DISABLED_SOURCES})
  set(local_overlay_logical_sources ${${name}_OVERLAY_DISABLED_LOGICAL_SOURCES})
  set(local_overlay_rooted_sources ${${name}_OVERLAY_DISABLED_ROOTED_SOURCES})

  foreach(source ${PARSED_ARGS_SOURCES})
    if (IS_ABSOLUTE ${source})
      list(APPEND local_overlay_sources ${source})
    else()
      list(APPEND local_overlay_sources ${CMAKE_CURRENT_SOURCE_DIR}/${source})
    endif()
    list(APPEND local_overlay_logical_sources ${source})
    merge_paths(named_path ${WN_CURRENT_OVERLAY} ${source})
    list(APPEND local_overlay_rooted_sources ${named_path})
  endforeach()

  set(${name}_OVERLAY_DISABLED_SOURCES ${local_overlay_sources} PARENT_SCOPE)
  set(${name}_OVERLAY_DISABLED_LOGICAL_SOURCES ${local_overlay_logical_sources} PARENT_SCOPE)
  set(${name}_OVERLAY_DISABLED_ROOTED_SOURCES ${local_overlay_rooted_sources} PARENT_SCOPE)
endmacro()

macro(_overlay_add_sources name)
  if (WN_OVERLAY_IS_ENABLED)
    _overlay_add_enabled_sources(${name} ${ARGN})
  else()
    _overlay_add_disabled_sources(${name} ${ARGN})
  endif()
endmacro()

macro(overlay_set_variable variable value)
  if (WN_OVERLAY_IS_ENABLED)
    set(${variable} ${value} PARENT_SCOPE)
  endif()
endmacro()

macro(_add_sources_to_target name)
  cmake_parse_arguments(
    ADDED_SOURCES_PARSED_ARGS
    ""
    ""
    "SOURCES;LIBS;COMPILE_OPTIONS;INCLUDES"
    ${ARGN})

  foreach(source ${ADDED_SOURCES_PARSED_ARGS_SOURCES})
    list(FIND ${name}_OVERLAY_LOGICAL_SOURCES ${source} index)
    if (${index} EQUAL -1)
      list(APPEND ${name}_OVERLAY_LOGICAL_SOURCES ${source})
      if (IS_ABSOLUTE ${source})
        list(APPEND ${name}_OVERLAY_SOURCES ${source})
      else()
        list(APPEND ${name}_OVERLAY_SOURCES
          ${CMAKE_CURRENT_SOURCE_DIR}/${source})
      endif()
      list(APPEND ${name}_OVERLAY_ROOTED_SOURCES ".")
    else()
    endif()
  endforeach()

  if (${name}_OVERLAY_LOGICAL_SOURCES)
    LIST(LENGTH ${name}_OVERLAY_LOGICAL_SOURCES num_local_existing_sources)
    math(EXPR num_local_existing_sources "${num_local_existing_sources} - 1")
  else()
    set(num_local_existing_sources -1)
  endif()

  if (${name}_OVERLAY_DISABLED_LOGCIAL_SOURCES)
    LIST(LENGTH ${name}_OVERLAY_DISABLED_LOGCIAL_SOURCES num_local_disabled_sources)
    math(EXPR num_local_disabled_sources "${num_local_disabled_sources} - 1")
  else()
    set(num_local_disabled_sources -1)
  endif()

  foreach(lib ${ADDED_SOURCES_PARSED_ARGS_LIBS})
    list(APPEND ${name}_OVERLAY_LIBS ${lib})
  endforeach()

  foreach(opt ${ADDED_SOURCES_PARSED_ARGS_COMPILE_OPTIONS})
    list(APPEND ${name}_OVERLAY_COMPILE_OPTIONS ${opt})
  endforeach()

  foreach(inc ${ADDED_SOURCES_PARSED_ARGS_INCLUDES})
    list(APPEND ${name}_OVERLAY_INCLUDES ${inc})
  endforeach()

  foreach(index RANGE 0 ${num_local_existing_sources})
    list(GET ${name}_OVERLAY_SOURCES ${index} existing_full_source)
    list(GET ${name}_OVERLAY_ROOTED_SOURCES ${index} existing_rooted_overlay)

    get_filename_component(dir_name ${existing_rooted_overlay} DIRECTORY)
    set(overlay_group "Overlays\\")
    if (dir_name)
      if (${existing_rooted_overlay} STREQUAL ".")
        list(GET ${name}_OVERLAY_LOGICAL_SOURCES ${index} dir_name)
        get_filename_component(dir_name ${dir_name} DIRECTORY)
        set(overlay_group "")
      endif()

      string(REPLACE "/" "\\" dir_name ${dir_name})
      source_group("${overlay_group}${dir_name}" FILES "${existing_full_source}")
    endif()
  endforeach()

  foreach(index RANGE 0 ${num_local_disabled_sources})
    list(GET ${name}_OVERLAY_DISABLED_SOURCES ${index} existing_full_source)
    list(GET ${name}_OVERLAY_DISABLED_ROOTED_SOURCES ${index} existing_rooted_overlay)

    get_filename_component(dir_name ${existing_rooted_overlay} DIRECTORY)
    set(overlay_group "Disabled\\")
    if (dir_name)
      if (${existing_rooted_overlay} STREQUAL ".")
        list(GET ${name}_OVERLAY_LOGICAL_SOURCES ${index} dir_name)
        get_filename_component(dir_name ${dir_name} DIRECTORY)
        set(overlay_group "")
      endif()

      string(REPLACE "/" "\\" dir_name ${dir_name})
      source_group("${overlay_group}${dir_name}" FILES "${existing_full_source}")
    endif()
  endforeach()
endmacro()

macro(overlay_library_sources name)
  _overlay_add_sources(${name} ${ARGN})
endmacro()

macro(overlay_header_library_sources name)
  _overlay_add_sources(${name} ${ARGN})
endmacro()

macro(overlay_executable_sources name)
  _overlay_add_sources(${name} ${ARGN})
endmacro()

function(overload_add_library name)
cmake_parse_arguments(
    PARSED_ARGS
    "SHARED"
    ""
    "SOURCES;LIBS;INCLUDES;COMPILE_OPTIONS"
    ${ARGN})

  set(LIB_TYPE STATIC)
  if (PARSED_ARGS_SHARED)
    set(LIB_TYPE SHARED)
  endif()
  add_library(${name} ${LIB_TYPE} ${PARSED_ARGS_SOURCES})
  if (PARSED_ARGS_LIBS)
    target_link_libraries(${name} ${PARSED_ARGS_LIBS})
  endif()
  if (PARSED_ARGS_INCLUDES)
    target_include_directories(${name} PUBLIC ${PARSED_ARGS_INCLUDES})
  endif()
  if (PARSED_ARGS_COMPILE_OPTIONS)
    target_compile_options(${name} PRIVATE ${PARSED_ARGS_COMPILE_OPTIONS})
  endif()
endfunction()

function(overload_add_executable name)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    ""
    "SOURCES;LIBS;INCLUDES"
    ${ARGN})
  add_executable(${name} ${PARSED_ARGS_SOURCES})
  if (PARSED_ARGS_LIBS)
    target_link_libraries(${name} ${PARSED_ARGS_LIBS})
  endif()
  if (PARSED_ARGS_INCLUDES)
    target_include_directories(${name} PUBLIC ${PARSED_ARGS_INCLUDES})
  endif()
endfunction()

function(overload_add_header_library name)
  # In the default case ACTUALLY do nothing
endfunction()

function(add_wn_library name)
  if (WN_OVERLAY_IS_ENABLED)
    cmake_parse_arguments(
      PARSED_ARGS
      "SHARED"
      ""
      "SOURCES;LIBS;INCLUDES;COMPILE_OPTIONS"
      ${ARGN})

    _add_sources_to_target(${name} SOURCES ${PARSED_ARGS_SOURCES} LIBS
      ${PARSED_ARGS_LIBS} INCLUDES ${PARSED_ARGS_INCLUDES})

    if (NOT ${name}_OVERLAY_SOURCES)
      message(FATAL_ERROR "Must supply at least one source file for ${name}")
    endif()
    set(IS_SHARED)
    if (PARSED_ARGS_SHARED)
      set(IS_SHARED SHARED)
      append_to_overlay_var(WN_ALL_SHARED_LIBS ${name})
    else()
      append_to_overlay_var(WN_ALL_STATIC_LIBS ${name})
    endif()

    overlay_named_file(cmake/target_functions/pre_add_library.cmake)
    overload_add_library(${name} SOURCES ${${name}_OVERLAY_SOURCES}
      LIBS ${${name}_OVERLAY_LIBS} INCLUDES ${${name}_OVERLAY_INCLUDES}
      COMPILE_OPTIONS ${${name}_OVERLAY_COMPILE_OPTIONS}
      ${IS_SHARED})
    overlay_named_file(cmake/target_functions/post_add_library.cmake)
  endif()
endfunction()

function(add_wn_executable name)
  if (WN_OVERLAY_IS_ENABLED)
    append_to_overlay_var(WN_ALL_EXECUTABLES ${name})
    cmake_parse_arguments(
      PARSED_ARGS
      ""
      ""
      "SOURCES;LIBS;INCLUDES"
      ${ARGN})

    _add_sources_to_target(${name} SOURCES ${PARSED_ARGS_SOURCES} LIBS
      ${PARSED_ARGS_LIBS})

    overlay_named_file(cmake/target_functions/pre_add_executable.cmake)

    if (NOT ${name}_OVERLAY_SOURCES)
      message(FATAL_ERROR "Must supply at least one source file for ${name}")
    endif()

    overload_add_executable(${name} SOURCES ${${name}_OVERLAY_SOURCES} LIBS
      ${${name}_OVERLAY_LIBS} INCLUDES ${PARSED_ARGS_INCLUDES})
    overlay_named_file(cmake/target_functions/post_add_executable.cmake)
  endif()
endfunction()

function (add_wn_header_library name)
  if (WN_OVERLAY_IS_ENABLED)
    cmake_parse_arguments(
      PARSED_ARGS
      ""
      ""
     "SOURCES"
      ${ARGN})

     _add_sources_to_target(${name} SOURCES ${PARSED_ARGS_SOURCES})

     if (NOT ${name}_OVERLAY_SOURCES)
       message(FATAL_ERROR "Must supply at least one source file for ${name}")
     endif()

     overlay_named_file(cmake/target_functions/pre_add_header_library.cmake)
     overload_add_header_library(${name} SOURCES ${${name}_OVERLAY_SOURCES})
     overlay_named_file(cmake/target_functions/post_add_header_library.cmake)
  endif()
endfunction()

function(overload_add_application name)
  add_wn_executable(${name} ${ARGN})
endfunction()

function(add_wn_application name)
  if (WN_OVERLAY_IS_ENABLED)
    append_to_overlay_var(WN_ALL_APPLICATIONS ${name})
    cmake_parse_arguments(
        PARSED_ARGS
        ""
        ""
        "SOURCES;LIBS;INCLUDES"
        ${ARGN})

    overload_add_application(${name}
      SOURCES
        ${PARSED_ARGS_SOURCES}
      LIBS
        executable_entry
        WNApplicationEntry
        executable_entry
        ${PARSED_ARGS_LIBS}
      INCLUDES
        ${PARSED_ARGS_INCLUDES}
    )
  endif()
endfunction()

macro(overlay_filesystem_files name)
  _overlay_add_sources(${name} ${ARGN})
endmacro()

# TODO(awoloszyn): Add this to the overlay system
function(wn_filesystem_files name)
  if (WN_OVERLAY_IS_ENABLED)
    cmake_parse_arguments(
        PARSED_ARGS
        ""
        "OUTPUT_DIR"
        "SOURCES"
        ${ARGN})

    _add_sources_to_target(${name} SOURCES ${PARSED_ARGS_SOURCES} LIBS
        ${PARSED_ARGS_LIBS})
    set(PYTHON_ARGS)

    LIST(LENGTH ${name}_OVERLAY_LOGICAL_SOURCES num_local_existing_sources)
    math(EXPR num_local_existing_sources "${num_local_existing_sources} - 1")
    foreach(index RANGE 0 ${num_local_existing_sources})
      list(GET ${name}_OVERLAY_LOGICAL_SOURCES ${index} logical_source)
      list(GET ${name}_OVERLAY_SOURCES ${index} full_source)
      string(LENGTH ${logical_source} logical_length)
      string(LENGTH ${full_source} full_length)
      math(EXPR start_of_logical "${full_length} - ${logical_length}")
      string(SUBSTRING ${full_source} 0 ${start_of_logical} root_dir)
      list(APPEND PYTHON_ARGS "--directory" ${root_dir} "--files" ${logical_source})
    endforeach()
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PARSED_ARGS_OUTPUT_DIR}/${name}.h
                              ${CMAKE_CURRENT_BINARY_DIR}/${PARSED_ARGS_OUTPUT_DIR}/${name}.cpp
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${PARSED_ARGS_OUTPUT_DIR}
      COMMAND python ${WNFramework_SOURCE_DIR}/Utilities/compile_file_directory.py
        --output-directory ${CMAKE_CURRENT_BINARY_DIR}/${PARSED_ARGS_OUTPUT_DIR} ${PYTHON_ARGS}
        --prefix ${name}
      DEPENDS ${WNFramework_SOURCE_DIR}/Utilities/compile_file_directory.py
        ${${name}_OVERLAY_SOURCES}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_custom_target(${name} DEPENDS 
       ${CMAKE_CURRENT_BINARY_DIR}/${PARSED_ARGS_OUTPUT_DIR}/${name}.h
       ${CMAKE_CURRENT_BINARY_DIR}/${PARSED_ARGS_OUTPUT_DIR}/${name}.cpp)
  endif()
endfunction()

enable_overlay_file()
