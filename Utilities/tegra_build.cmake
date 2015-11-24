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

function(fix_path source target)
  file(TO_NATIVE_PATH ${source} temp_target)
  string(REPLACE "\\" "\\\\" temp_target "${temp_target}")
  set(${target} ${temp_target} PARENT_SCOPE)
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
  set(WN_ANDROID_DEBUG "false")

  set(WN_ACTIVITY_NAME ${PARSED_ARGS_ACTIVITY})
  set(WN_PROGRAM_NAME ${PARSED_ARGS_PROGRAM_NAME})
  set(WN_TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR}/${WN_PROGRAM_NAME})
  set(ANDROID_RUNNER ${PROJECT_SOURCE_DIR}/Utilities/android_runner.py)

  set(WN_TARGET_NAME ${PARSED_ARGS_TARGET} )

  get_target_property(PERMISSIONS ${target} WN_ANDROID_PERMISSIONS)
  if (PERMISSIONS)
    afix(APP_PERMISSIONS
      "      <uses-permission android:name=\"android.permission."
      "\" />" ${PERMISSIONS})
    string(REPLACE ";" "\n" WN_PERMISSIONS "${APP_PERMISSIONS}")
  endif()
  library_name(LIB_NAME ${PARSED_ARGS_TARGET})

  set(WN_NATIVE_LIBRARIES ${LIB_NAME})
  #set(SOLIB_PATH ${WN_TARGET_DIR}/obj/local/${ANDROID_ABI}/)

  fix_path(${WN_TARGET_DIR} WN_TARGET_DIR)

  configure_file("${PROJECT_SOURCE_DIR}/Utilities/AndroidManifest.xml.in"
    "${WN_TARGET_DIR}/AndroidManifest.xml")
  configure_file("${PROJECT_SOURCE_DIR}/Utilities/strings.xml.in"
    "${WN_TARGET_DIR}/res/values/strings.xml")
  configure_file("${PROJECT_SOURCE_DIR}/Utilities/LibraryLoader.java.in"
    "${WN_TARGET_DIR}/src/${PARSED_ARGS_TOP_LEVEL}/${PARSED_ARGS_DOMAIN}/${PARSED_ARGS_PROGRAM_NAME}/${WN_ACTIVITY_NAME}.java")

  set_property(TARGET ${PARSED_ARGS_TARGET} APPEND PROPERTY SOURCES ${WN_TARGET_DIR}/AndroidManifest.xml)
  set_property(TARGET ${PARSED_ARGS_TARGET} APPEND PROPERTY SOURCES ${WN_TARGET_DIR}/res/values/strings.xml)
  set_property(TARGET ${PARSED_ARGS_TARGET} APPEND PROPERTY SOURCES ${WN_TARGET_DIR}/src/${PARSED_ARGS_TOP_LEVEL}/${PARSED_ARGS_DOMAIN}/${PARSED_ARGS_PROGRAM_NAME}/${WN_ACTIVITY_NAME}.java)
  set_property(TARGET ${PARSED_ARGS_TARGET} PROPERTY ANDROID_GUI ON)
  set_property(TARGET ${PARSED_ARGS_TARGET} PROPERTY ANDROID_API_MIN "15")
  set_property(TARGET ${PARSED_ARGS_TARGET} PROPERTY ANDROID_API "15")

endfunction()
