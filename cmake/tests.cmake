enable_testing()
include(CMakeParseArguments)
# Arguments
#     SOURCE_DIR: Directory that contains each individual test.
#     COMMON_SOURCES: Sourcs to include into each test.
#     SOURCES: List of source files in SOURCE_DIR of the form ${SOURCE}Tests.cpp.
#     ADDITIONAL_INCLUDES: Any additional include directories.
#     RUN_WRAPPER: Wrappe script to run the test.
#     TEST_PREFIX: The prefix to use for the name of the test.
#     LIBS: Any additional libraries to include.
function(wn_create_tests_from_list)
  if(WN_OVERLAY_IS_ENABLED)
    cmake_parse_arguments(
      PARSED_ARGS
      "SYSTEMS_TEST"
      "RUN_WRAPPER;TEST_PREFIX;SOURCE_DIR"
      "SOURCES;COMMON_SOURCES;ADDITIONAL_INCLUDES;LIBS"
      ${ARGN})

    if(NOT PARSED_ARGS_SOURCES)
      message(FATAL_ERROR "You must provide at least one source file")
    endif()
    set(ST)
    if (PARSED_ARGS_SYSTEMS_TEST)
      set(ST "SYSTEMS_TEST")
    endif()

    _add_sources_to_target(${PARSED_ARGS_TEST_PREFIX}_test
      SOURCES
        ${PARSED_ARGS_SOURCES})

    foreach(source ${${PARSED_ARGS_TEST_PREFIX}_test_OVERLAY_SOURCES})
      get_filename_component(source ${source} NAME)
      overload_create_test_wrapper(TEST_NAME ${PARSED_ARGS_TEST_PREFIX}_${source}
        ${ST}
        TEST_PREFIX ${PARSED_ARGS_TEST_PREFIX}
        SOURCES ${PARSED_ARGS_SOURCE_DIR}/${source}Tests.cpp
          ${PARSED_ARGS_COMMON_SOURCES}
          RUN_WRAPPER ${PARSED_ARGS_RUN_WRAPPER}
          ADDITIONAL_INCLUDES ${PARSED_ARGS_ADDITIONAL_INCLUDES}
          LIBS ${PARSED_ARGS_LIBS})
    endforeach()
  endif()
endfunction()

function(overload_create_test_wrapper)
  if(WN_OVERLAY_IS_ENABLED)
    overlay_named_file(cmake/target_functions/pre_add_test.cmake)
    overload_create_test(${ARGN})
    overlay_named_file(cmake/target_functions/post_add_test.cmake)
  endif()
endfunction()

function(overload_create_test)
wn_create_test(${ARGN})
endfunction()

# Arguments
#     TEST_NAME: Name of the test to create.
#     RUN_WRAPPER: Wrapper script to run the test.
#     TEST_PREFIX: The prefix to use for the name of the test.
#     SOURCES: Source files for this test.
#     ADDITIONAL_INCLUDES: Any additional include directories.
#     SYSTEMS_TEST: Defined if this is a system test
#     LIBS: Any additional libraries to include.
function(wn_create_test)
  cmake_parse_arguments(
    PARSED_ARGS
    "SYSTEMS_TEST"
    "TEST_NAME;RUN_WRAPPER;TEST_PREFIX"
    "SOURCES;ADDITIONAL_INCLUDES;LIBS"
    ${ARGN})
  if(NOT PARSED_ARGS_TEST_NAME)
    message(FATAL_ERROR "You must provide a test name")
  endif()
  if(NOT PARSED_ARGS_SOURCES)
    message(FATAL_ERROR "You must provide at least one source file")
  endif()
  add_wn_executable(${PARSED_ARGS_TEST_NAME}_test SOURCES ${PARSED_ARGS_SOURCES}
    LIBS WNTesting WNUtilities ${PARSED_ARGS_LIBS})

  add_test(${PARSED_ARGS_TEST_NAME}
    ${PARSED_ARGS_RUN_WRAPPER} ${PARSED_ARGS_TEST_NAME}_test)
endfunction()

enable_overlay_file()