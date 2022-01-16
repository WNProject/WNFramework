enable_testing()
include(CMakeParseArguments)

function(wn_create_tests_from_list_internal)
  if(WN_OVERLAY_IS_ENABLED)
    propagate_from_parent(WN_ALL_EXECUTABLES)
    propagate_from_parent(WN_ALL_SHARED_LIBS)
    propagate_from_parent(WN_ALL_STATIC_LIBS)
    propagate_from_parent(WN_ALL_APPLICATIONS)
    cmake_parse_arguments(
      PARSED_ARGS
      ""
      "RUN_WRAPPER;TEST_PREFIX;SOURCE_DIR;TEST_WRAPPER"
      "SOURCES;COMMON_SOURCES;ADDITIONAL_INCLUDES;LIBS;LABELS"
      ${ARGN}
    )

    if(NOT PARSED_ARGS_SOURCES)
      message(FATAL_ERROR "You must provide at least one source file")
    endif()

    _add_sources_to_target(
      ${PARSED_ARGS_TEST_PREFIX}_test
      SOURCES
        ${PARSED_ARGS_SOURCES}
    )

    foreach(source ${${PARSED_ARGS_TEST_PREFIX}_test_OVERLAY_SOURCES})
      get_filename_component(source ${source} NAME)
      overload_create_test_wrapper(
        TEST_NAME ${PARSED_ARGS_TEST_PREFIX}_${source}
        TEST_WRAPPER ${PARSED_ARGS_TEST_WRAPPER}
        TEST_PREFIX ${PARSED_ARGS_TEST_PREFIX}
        SOURCES
          ${PARSED_ARGS_SOURCE_DIR}/${source}Tests.cpp
          ${PARSED_ARGS_COMMON_SOURCES}
        RUN_WRAPPER ${PARSED_ARGS_RUN_WRAPPER}
        ADDITIONAL_INCLUDES ${PARSED_ARGS_ADDITIONAL_INCLUDES}
        LIBS ${PARSED_ARGS_LIBS}
        LABELS ${PARSED_ARGS_LABELS}
      )
    endforeach()

    propagate_to_parent(WN_ALL_EXECUTABLES)
    propagate_to_parent(WN_ALL_SHARED_LIBS)
    propagate_to_parent(WN_ALL_STATIC_LIBS)
    propagate_to_parent(WN_ALL_APPLICATIONS)
  endif()
endfunction()

function(overload_create_test_wrapper)
  propagate_from_parent(WN_ALL_EXECUTABLES)
  propagate_from_parent(WN_ALL_SHARED_LIBS)
  propagate_from_parent(WN_ALL_STATIC_LIBS)
  propagate_from_parent(WN_ALL_APPLICATIONS)
  if(WN_OVERLAY_IS_ENABLED)
    overlay_named_file(cmake/target_functions/pre_add_test.cmake)
    overload_create_test(${ARGN})
    overlay_named_file(cmake/target_functions/post_add_test.cmake)
  endif()
  propagate_to_parent(WN_ALL_EXECUTABLES)
  propagate_to_parent(WN_ALL_SHARED_LIBS)
  propagate_to_parent(WN_ALL_STATIC_LIBS)
  propagate_to_parent(WN_ALL_APPLICATIONS)
endfunction()

macro(overload_create_test)
  wn_create_test(${ARGN})
endmacro()

macro(wn_create_test)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    "TEST_NAME;RUN_WRAPPER;TEST_PREFIX;TEST_WRAPPER"
    "SOURCES;ADDITIONAL_INCLUDES;LIBS;LABELS"
    ${ARGN}
  )

  if(NOT PARSED_ARGS_TEST_NAME)
    message(FATAL_ERROR "You must provide a test name")
  endif()

  if(NOT PARSED_ARGS_SOURCES)
    message(FATAL_ERROR "You must provide at least one source file")
  endif()

  if(NOT PARSED_ARGS_TEST_WRAPPER)
    message(ERROR "No test wrapper path given and no default available ${PARSED_ARGS_TEST_NAME}")
  else()
    include(${PARSED_ARGS_TEST_WRAPPER})
  endif()

  wn_add_test_wrapper(
    OUTPUT_TEST_COMAND
    OUTPUT_TEST_NAME
    TEST_NAME ${PARSED_ARGS_TEST_NAME}_test
    SOURCES ${PARSED_ARGS_SOURCES}
    ADDITIONAL_INCLUDES ${PARSED_ARGS_ADDITIONAL_INCLUDES}
    LIBS ${PARSED_ARGS_LIBS} WNLogging
    ${PARSED_ARGS_UNPARSED_ARGUMENTS}
  )
  add_test(NAME ${OUTPUT_TEST_NAME} COMMAND ${OUTPUT_TEST_COMAND})

  if(PARSED_ARGS_LABELS)
    set_tests_properties(
      ${OUTPUT_TEST_NAME}
      PROPERTIES
        LABELS ${PARSED_ARGS_LABELS}
    )
  endif()

  wn_post_add_test_wrapper(${OUTPUT_TEST_NAME})
endmacro()

macro(overload_create_call_test)
  if (${prefix}_test_OVERLAY_SOURCES)
    foreach(source ${${prefix}_test_OVERLAY_SOURCES})
        get_filename_component(source ${source} NAME)
        set(OUTPUT_TEST_NAME ${prefix}_${source}_test)
        add_test(
          NAME ${OUTPUT_TEST_NAME}
          COMMAND
            ${PARSED_ARGS_APPLICATION}
            ${PARSED_ARGS_COMMAND_PRE_DIR}
            ${PARSED_ARGS_COMMAND_DIR}
            ${PARSED_ARGS_COMMAND_POST_DIR}
            ${source}
          WORKING_DIRECTORY ${PARSED_ARGS_WORKING_DIRECTORY}
        )

        if(PARSED_ARGS_LABELS)
          set_tests_properties(
            ${OUTPUT_TEST_NAME}
            PROPERTIES
              LABELS ${PARSED_ARGS_LABELS}
          )
        endif()
    endforeach()
  else()
    set(OUTPUT_TEST_NAME ${prefix}_test_run)
    add_test(
      NAME ${OUTPUT_TEST_NAME}
      COMMAND
        ${PARSED_ARGS_APPLICATION}
        ${PARSED_ARGS_COMMAND_PRE_DIR}
        ${PARSED_ARGS_COMMAND_DIR}
        ${PARSED_ARGS_COMMAND_POST_DIR}
      WORKING_DIRECTORY ${PARSED_ARGS_WORKING_DIRECTORY}
    )

    if(PARSED_ARGS_LABELS)
      set_tests_properties(
        ${OUTPUT_TEST_NAME}
        PROPERTIES
          LABELS ${PARSED_ARGS_LABELS}
      )
    endif()
  endif()
endmacro()

function(wn_create_call_test prefix)
  if(WN_OVERLAY_IS_ENABLED)
    cmake_parse_arguments(
      PARSED_ARGS
      ""
      "WORKING_DIRECTORY;COMMAND_DIR;COMMAND_POST_DIR;APPLICATION"
      "COMMAND_PRE_DIR;UNIQUE_ARGS;LABELS"
      ${ARGN}
    )

    if(NOT PARSED_ARGS_COMMAND_PRE_DIR)
      set(PARSED_ARGS_COMMAND_PRE_DIR "")
    endif()
    if (NOT PARSED_ARGS_COMMAND_DIR)
      set(PARSED_ARGS_COMMAND_DIR "")
    endif()
    if (NOT PARSED_ARGS_COMMAND_POST_DIR)
      set(PARSED_ARGS_COMMAND_POST_DIR "")
    endif()

    if(PARSED_ARGS_UNIQUE_ARGS)
      _add_sources_to_target(
        ${prefix}_test
        SOURCES ${PARSED_ARGS_UNIQUE_ARGS}
      )
    endif()

    overload_create_call_test()
    propagate_to_parent(WN_ALL_EXECUTABLES)
    propagate_to_parent(WN_ALL_SHARED_LIBS)
    propagate_to_parent(WN_ALL_STATIC_LIBS)
    propagate_to_parent(WN_ALL_APPLICATIONS)
  endif()
endfunction()

enable_overlay_file()
