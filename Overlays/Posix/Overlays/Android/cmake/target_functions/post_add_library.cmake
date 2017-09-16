foreach(SRC ${${name}_OVERLAY_SOURCES})
  get_filename_component(EXTENSION ${SRC} EXT)
  if (EXTENSION)
    if (${EXTENSION} STREQUAL ".S" OR ${EXTENSION} STREQUAL ".s")
      SET_SOURCE_FILES_PROPERTIES(${SRC} PROPERTIES LANGUAGE CXX)
      SET_SOURCE_FILES_PROPERTIES(${SRC} PROPERTIES COMPILE_FLAGS
        "-x assembler-with-cpp"
      )
    endif()
  endif()
endforeach()
