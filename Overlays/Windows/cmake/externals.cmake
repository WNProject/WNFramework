foreach(EXTERNAL_TARGET ${EXTERNAL_TARGETS})
    get_target_property(type ${EXTERNAL_TARGET} TYPE)
    if (NOT ${type} STREQUAL "INTERFACE_LIBRARY")
      target_compile_options(
        ${EXTERNAL_TARGET} PRIVATE
        ${PARSED_ARGS_COMPILE_OPTIONS}
      )
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
    endif()
endforeach()
