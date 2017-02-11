cmake_parse_arguments(
    PARSED_ARGS
    "SYSTEMS_TEST"
    "TEST_NAME;RUN_WRAPPER;TEST_PREFIX"
    "SOURCES;ADDITIONAL_INCLUDES;LIBS"
    ${ARGN})

file(RELATIVE_PATH relative_offset
    ${WNFramework_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
string(FIND ${relative_offset} "/" first_slash)
if (first_slash)
    string(SUBSTRING ${relative_offset} 0 ${first_slash} relative_offset)
endif()

set_property(TARGET ${PARSED_ARGS_TEST_NAME}_test
  PROPERTY FOLDER "Tests/${relative_offset}/${PARSED_ARGS_TEST_PREFIX}")
