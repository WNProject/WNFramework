file(RELATIVE_PATH relative_offset ${WNFramework_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
string(FIND ${relative_offset} "/" first_slash)
if (first_slash)
    string(SUBSTRING ${relative_offset} 0 ${first_slash} relative_offset)
endif()

set_property(TARGET ${name} PROPERTY FOLDER "${relative_offset}/assets")
