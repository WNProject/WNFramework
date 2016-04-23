include(${Posix_Android_ROOT_DIR}/cmake/build_apk.cmake)

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
  if (PARSED_ARGS_SYSTEMS_TEST)
          add_wn_library(${PARSED_ARGS_TEST_NAME}_test SHARED
            SOURCES ${PARSED_ARGS_SOURCES}
            LIBS WNTesting WNUtilities ${PARSED_ARGS_LIBS})
    build_apk(
      ACTIVITY ${PARSED_ARGS_TEST}_test
      PROGRAM_NAME ${PARSED_ARGS_TEST_NAME}_test
      TARGET ${PARSED_ARGS_TEST_NAME}_test
    )
    add_test(${PARSED_ARGS_TEST_NAME}
      ${WN_PYTHON}
        ${PARSED_ARGS_TEST_NAME}_test/${PARSED_ARGS_TEST_NAME}_test.py
        --install --run --remove
    )
  else()
    add_wn_executable(${PARSED_ARGS_TEST_NAME}_test
      SOURCES ${PARSED_ARGS_SOURCES}
        LIBS WNTesting WNUtilities ${PARSED_ARGS_LIBS})
    set(target ${PARSED_ARGS_TEST_NAME}_test)
    configure_file(
      ${Posix_Android_ROOT_DIR}/cmake/android_helpers/standalone/native_runner.py.in
      ${PARSED_ARGS_TEST_NAME}_test.py)
    add_test(${PARSED_ARGS_TEST_NAME}
      ${WN_PYTHON} ${PARSED_ARGS_TEST_NAME}_test.py --install --run --remove)
  endif()
endfunction()
