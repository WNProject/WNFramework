if (WN_ANDROID_WRAPPER)
  if (NOT WN_HOST_BUILD_CONFIGURED)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory
            ${WNFramework_BINARY_DIR}/host
    )
    message(STATUS " -- Confguring host build")
    message(${CMAKE_COMMAND})
    execute_process(
        COMMAND ${CMAKE_COMMAND}
            -G ${CMAKE_GENERATOR} ${WNFramework_SOURCE_DIR}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        WORKING_DIRECTORY ${WNFramework_BINARY_DIR}/host
    )
    set(WN_HOST_BUILD_CONFIGURED TRUE CACHE INTERNAL
        "Android host-build has been configured" FORCE)
  endif()
endif()