
function(library_name var target)
  get_target_property(LIB_NAME ${target} LIBRARY_OUTPUT_NAME)
  if(NOT LIB_NAME)
    get_target_property(LIB_NAME ${target} OUTPUT_NAME)
  endif()
  if(NOT LIB_NAME)
    set(LIB_NAME ${target})
  endif()
  set(${var} ${LIB_NAME} PARENT_SCOPE)
endfunction()

function(afix target prefix postfix)
  set(tempvar "")
  foreach(f ${ARGN})
    list(APPEND tempvar "${prefix}${f}${postfix}")
  endforeach()
  set(${target} "${tempvar}" PARENT_SCOPE)
endfunction()

function(build_apk)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    "TOP_LEVEL;DOMAIN;SUBDOMAIN;ACTIVITY;PROGRAM_NAME;TARGET"
    ""
    ${ARGN})
  if(NOT PARSED_ARGS_TOP_LEVEL)
    set(PARSED_ARGS_TOP_LEVEL "org")
  endif()
  if(NOT PARSED_ARGS_DOMAIN)
    set(PARSED_ARGS_DOMAIN "example")
  endif()
  if(NOT PARSED_ARGS_ACTIVITY)
    message(FATAL_ERROR "Must supply an activity name to build_apk")
  endif()
  if(NOT PARSED_ARGS_PROGRAM_NAME)
    message(FATAL_ERROR "Must supply a program name to build_apk")
  endif()
  if(NOT PARSED_ARGS_TARGET)
    message(FATAL_ERROR "Must supply a target shared object to build_apk")
  endif()

  set(WN_PACKAGE_NAME ${PARSED_ARGS_TOP_LEVEL}.${PARSED_ARGS_DOMAIN}.${PARSED_ARGS_PROGRAM_NAME})
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(WN_ANDROID_DEBUG "true")
    set(WN_ANDROID_BUILD "debug")
  else()
    set(WN_ANDROID_DEBUG "false")
    set(WN_ANDROID_BUILD "release")
  endif()
  set(WN_ACTIVITY_NAME ${PARSED_ARGS_ACTIVITY})
  set(WN_PROGRAM_NAME ${PARSED_ARGS_PROGRAM_NAME})
  set(WN_TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR}/${WN_PROGRAM_NAME})

  get_target_property(PERMISSIONS ${target} WN_ANDROID_PERMISSIONS)
  if (PERMISSIONS)
    afix(APP_PERMISSIONS
      "      <uses-permission android:name=\"android.permission."
      "\" />" ${PERMISSIONS})
    string(REPLACE ";" "\n" WN_PERMISSIONS "${APP_PERMISSIONS}")
  endif()
  library_name(LIB_NAME ${PARSED_ARGS_TARGET})

  set(WN_NATIVE_LIBRARIES ${LIB_NAME})
  set(SOLIB_PATH ${WN_TARGET_DIR}/obj/local/${ANDROID_ABI})
  configure_file("${PROJECT_SOURCE_DIR}/Utilities/AndroidManifest.xml.in"
    "${WN_TARGET_DIR}/AndroidManifest.xml")
  configure_file("${PROJECT_SOURCE_DIR}/Utilities/strings.xml.in"
    "${WN_TARGET_DIR}/res/values/strings.xml")
  configure_file("${PROJECT_SOURCE_DIR}/Utilities/LibraryLoader.java.in"
    "${WN_TARGET_DIR}/src/${PARSED_ARGS_TOP_LEVEL}/${PARSED_ARGS_DOMAIN}/${PARSED_ARGS_PROGRAM_NAME}/${WN_ACTIVITY_NAME}.java")
  configure_file("${PROJECT_SOURCE_DIR}/Utilities/Android.mk.in"
    "${WN_TARGET_DIR}/jni/Android.mk")
  configure_file("${PROJECT_SOURCE_DIR}/Utilities/Application.mk.in"
    "${WN_TARGET_DIR}/jni/Application.mk")
  configure_file("${PROJECT_SOURCE_DIR}/Utilities/gdb.setup.in"
    "${WN_TARGET_DIR}/libs/${ANDROID_ABI}/gdb.setup")

  set(WN_TARGET_NAME ${PARSED_ARGS_TARGET} )

  add_custom_command(TARGET ${WN_TARGET_NAME}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${WN_TARGET_DIR}/libs/${ANDROID_ABI}")
  add_custom_command(TARGET ${WN_TARGET_NAME}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${WN_TARGET_DIR}/libs/${ANDROID_ABI}")
  add_custom_command(TARGET ${WN_TARGET_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PARSED_ARGS_TARGET}>" "${WN_TARGET_DIR}/libs/${ANDROID_ABI}/"
    DEPENDS $<TARGET_FILE:${PARSED_ARGS_TARGET}>)

  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET ${WN_TARGET_NAME}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy "${ANDROID_NDK}/prebuilt/android-${ANDROID_ARCH_NAME}/gdbserver/gdbserver"
        "${WN_TARGET_DIR}/libs/${ANDROID_ABI}/")
  add_custom_command(TARGET ${WN_TARGET_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PARSED_ARGS_TARGET}>" "${SOLIB_PATH}"
    DEPENDS $<TARGET_FILE:${PARSED_ARGS_TARGET}>)
  endif()

  if (ANDROID_SDK_DIR)
    set(ANDROID_TOOL_PATH ${ANDROID_SDK_DIR}/tools/android)
    if (${ANDROID_NDK_HOST_SYSTEM_NAME} STREQUAL "windows" OR
      ${ANDROID_NDK_HOST_SYSTEM_NAME} STREQUAL "windows-x86_64")
      find_host_program(PYTHON python REQUIRED)
      set(ANDROID_TOOL_PATH ${PYTHON} ${WNFramework_SOURCE_DIR}/Utilities/single_process_windows.py
        "WNFrame/workAndroidUpdateProject"
        "${ANDROID_SDK_DIR}/tools/android.bat")
    endif()

# On windows we fail if we run more than once instance of android update at the same time.
    add_custom_command(TARGET ${WN_TARGET_NAME}
      POST_BUILD
      COMMAND  ${ANDROID_TOOL_PATH} update project -t
        android-${ANDROID_NATIVE_API_LEVEL}
        --name ${WN_TARGET_NAME} --path "${WN_TARGET_DIR}" > ${WN_NULL_LOCATION}
        COMMENT "Creating android project for ${WN_TARGET_NAME}")
    find_host_program(ANT ant)

    if (NOT ANT)
      message(WARNING "Cannot find ant, ANDROID APK will not get built")
    else()
      add_custom_command(TARGET ${WN_TARGET_NAME}
        POST_BUILD
        COMMAND ${ANT} "${WN_ANDROID_BUILD}" > ${WN_NULL_LOCATION}
        WORKING_DIRECTORY "${WN_TARGET_DIR}"
        COMMENT "Build android .apk for ${WN_TARGET_NAME}")
      if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(WN_APK_NAME ${WN_TARGET_NAME}-debug.apk)
      else()
        set(WN_APK_NAME ${WN_TARGET_NAME}-release-unsigned.apk)
      endif()
      add_custom_command(TARGET ${WN_TARGET_NAME}
        POST_BUILD
        COMMAND  ${CMAKE_COMMAND} -E copy_if_different
          ${WN_TARGET_DIR}/bin/${WN_APK_NAME}
          ${CMAKE_CURRENT_BINARY_DIR})
    endif()
  endif()

endfunction()
