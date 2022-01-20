foreach(EXTERNAL_TARGET ${EXTERNAL_TARGETS})
    set_property(
      TARGET ${EXTERNAL_TARGET}
      PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )

    if (WN_USE_SCCACHE)
      get_target_property(CFLAGS ${EXTERNAL_TARGET} COMPILE_FLAGS)
      if (COMPILE_FLAGS AND NOT COMPILE_FLAGS MATCHES ".*NOTFOUND")
        string(REGEX REPLACE "/Zi" "" CFLAGS ${CFLAGS})
        string(REGEX REPLACE "-Zi" "" CFLAGS ${CFLAGS})
        set_target_properties(
          ${EXTERNAL_TARGET}
          PROPERTIES
            COMPILE_FLAGS "${CFLAGS}"
        )
      endif()
    endif()
endforeach()
