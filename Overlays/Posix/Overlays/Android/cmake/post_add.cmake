include(${WN_CURRENT_OVERLAY_ROOT}/cmake/gradle_build.cmake)
if (WN_ANDROID_WRAPPER)
  if (NOT WN_ANDROID_BUILD_CONFIGURED)
    foreach(BUILD_ABI ${WN_SANITIZED_ABIS})
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E make_directory
                ${WNFramework_BINARY_DIR}/${BUILD_ABI}
        )

        message(STATUS "Configuring: ${BUILD_ABI}")

        execute_process(
            COMMAND ${CMAKE_COMMAND}
                -DCMAKE_TOOLCHAIN_FILE=${WN_ANDROID_TOOLCHAIN_FILE}
                -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                -G ${CMAKE_GENERATOR} ${WNFramework_SOURCE_DIR}
                -DWN_ANDROID_SDK=${WN_ANDROID_SDK}
                -DANDROID_ABI=${BUILD_ABI}
                -DANDROID_STL=c++_static
                -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${WNFramework_BINARY_DIR}/${BUILD_ABI}/lib
                -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${WNFramework_BINARY_DIR}/${BUILD_ABI}/bin
                -DANDROID_NATIVE_API_LEVEL=${WN_ANDROID_NATIVE_API_LEVEL}
                -DWN_ANDROID_LLVM_TBLGEN=${WNFramework_BINARY_DIR}/host/Externals/llvm/bin/llvm-tblgen${CMAKE_EXECUTABLE_SUFFIX}
            WORKING_DIRECTORY ${WNFramework_BINARY_DIR}/${BUILD_ABI}
        )

        message(STATUS "Configuring: ${BUILD_ABI} - done")
    endforeach()
    set(WN_ANDROID_BUILD_CONFIGURED TRUE CACHE INTERNAL
        "Android sub-builds have been configured" FORCE)
  endif()
    set(SUB_BUILDS)
    set(SUB_BUILD_FAKE)
    foreach(BUILD_ABI ${WN_SANITIZED_ABIS})
        set(BUILT_APPS)
        foreach(app ${WN_ALL_APPLICATIONS})
            list(APPEND BUILT_APPS ${WNFramework_BINARY_DIR}/${BUILD_ABI}/lib/stripped/lib${app}.so)
        endforeach()
        add_custom_target(
            sub-build-${BUILD_ABI}
            DEPENDS llvm-host-tblgen
        )
        add_custom_command(TARGET
            sub-build-${BUILD_ABI}
            COMMAND ${CMAKE_COMMAND} --build .
            WORKING_DIRECTORY ${WNFramework_BINARY_DIR}/${BUILD_ABI}
        )
        add_custom_command(
            OUTPUT ${BUILT_APPS}
            DEPENDS sub-build-${BUILD_ABI} ${SUB_BUILD_FAKE}
            WORKING_DIRECTORY ${WNFramework_BINARY_DIR}/${BUILD_ABI}
        )
        set(SUB_BUILD_FAKE sub-build-fake-${BUILD_ABI})
    endforeach()
    add_custom_target(
        build-all-sub-targets
        DEPENDS
            ${SUB_BUILD_FAKE})
    set(ALL_APKS)
    foreach(app ${WN_ALL_APPLICATIONS})
        add_application(${app})
        list(APPEND ALL_APKS build-${app}-apk)
    endforeach()
    add_custom_target(build-all
        ALL
        DEPENDS ${ALL_APKS})
endif()