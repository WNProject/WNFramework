
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
    list(APPEND tempvar "${prefix}${f}${afix}")
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
  if(NOT PARSED_ARGS_SUBDOMAIN)
    set(PARSED_ARGS_SUBDOMAIN "test")
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

  set(WN_PACKAGE_NAME ${PARSED_ARGS_TOP_LEVEL}.${PARSED_ARGS_DOMAIN}.${PARSED_ARGS_SUBDOMAIN})
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
  #get_target_property(SO_NAME ${PARSED_ARGS_TARGET} LOCATION)
  library_name(LIB_NAME ${PARSED_ARGS_TARGET})

  set(WN_NATIVE_LIBRARIES ${LIB_NAME})

  configure_file("${PROJECT_SOURCE_DIR}/Utilities/AndroidManifest.xml.in"
    "${WN_TARGET_DIR}/AndroidManifest.xml")
  configure_file("${PROJECT_SOURCE_DIR}/Utilities/strings.xml.in"
    "${WN_TARGET_DIR}/res/values/strings.xml")
  configure_file("${PROJECT_SOURCE_DIR}/Utilities/LibraryLoader.java.in"
    "${WN_TARGET_DIR}/src/${PARSED_ARGS_TOP_LEVEL}/${PARSED_ARGS_DOMAIN}/${PARSED_ARGS_SUBDOMAIN}/${WN_ACTIVITY_NAME}.java")

  set(WN_TARGET_NAME ${PARSED_ARGS_TARGET} )
  
  #add_custom_command(TARGET ${WN_TARGET_NAME}
  #  PRE_BUILD
  #  COMMAND ${CMAKE_COMMAND} -E remove_directory "${WN_TARGET_DIR}/libs/${ANDROID_ABI}")
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
  endif()
  
  if (ANDROID_SDK_DIR)
    set(ANDROID_TOOL_PATH ${ANDROID_SDK_DIR}/tools/android)
    if (${ANDROID_NDK_HOST_SYSTEM_NAME} STREQUAL "windows" OR
      ${ANDROID_NDK_HOST_SYSTEM_NAME} STREQUAL "windows-x86_64")
      find_host_program(PYTHON python REQUIRED)
      set(ANDROID_TOOL_PATH ${PYTHON} ${WNFramework_SOURCE_DIR}/Utilities/single_process_windows.py
        "WNFrameworkAndroidUpdateProject"
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
    endif()
  endif()

endfunction()
