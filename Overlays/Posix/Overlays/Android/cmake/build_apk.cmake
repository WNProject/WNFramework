string(TOLOWER ${ANDROID_NDK_HOST_SYSTEM_NAME} ANDROID_SYSTEM_LOWER)

if (${ANDROID_SYSTEM_LOWER} STREQUAL "windows" OR
      ${ANDROID_SYSTEM_LOWER} STREQUAL "windows-x86_64")
    set(WN_NULL_LOCATION NUL)
else()
    set(WN_NULL_LOCATION /dev/null)
endif()

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

if (${ANDROID_SYSTEM_LOWER} STREQUAL "windows" OR
  ${ANDROID_SYSTEM_LOWER} STREQUAL "windows-x86_64")
  find_program(PYTHON python REQUIRED)
  set(DEBUG_KEYSTORE_PATH "$ENV{USERPROFILE}\\.android\\debug.keystore")
  fix_path(${DEBUG_KEYSTORE_PATH} DEBUG_KEYSTORE_PATH)
else()
  set(DEBUG_KEYSTORE_PATH "$ENV{HOME}/.android/debug.keystore")
endif()

set(ANDROID_RELEASE_KEY_STORE "${DEBUG_KEYSTORE_PATH}" CACHE STRING "Keystore used to sign release executables")
set(ANDROID_RELEASE_KEY_ALIAS "androiddebugkey"  CACHE STRING "Alias for your release key")
set(ANDROID_RELEASE_KEY_STORE_PASSWORD ""  CACHE STRING "Password for your android keystore")
set(ANDROID_RELEASE_KEY_ALIAS_PASSWORD ""  CACHE STRING "Password for your android keystore")

function(build_apk)
  cmake_parse_arguments(
    PARSED_ARGS
    ""
    "TOP_LEVEL;DOMAIN;ACTIVITY;PROGRAM_NAME;TARGET"
    ""
    ${ARGN})
  if(NOT PARSED_ARGS_TOP_LEVEL)
    set(PARSED_ARGS_TOP_LEVEL "project")
  endif()
  if(NOT PARSED_ARGS_DOMAIN)
    set(PARSED_ARGS_DOMAIN "wn")
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

  set(configure_source_dir ${Posix_Android_ROOT_DIR}/cmake/android_helpers/app)

  set(ANDROID_RUNNER ${configure_source_dir}/android_runner.py)

  file(MAKE_DIRECTORY ${WN_TARGET_DIR})

  set(WN_TARGET_NAME ${PARSED_ARGS_TARGET} )
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(WN_APK_NAME ${WN_TARGET_NAME}-debug.apk)
  else()
    set(WN_APK_NAME ${WN_TARGET_NAME}-release.apk)
  endif()
  set(WN_APK_LOCATION ${WN_TARGET_DIR}/bin/${WN_APK_NAME})

  get_target_property(PERMISSIONS ${PARSED_ARGS_TARGET} WN_ANDROID_PERMISSIONS)
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (NOT PERMISSIONS)
      set(PERMISSIONS "READ_EXTERNAL_STORAGE")
    else()
      list(APPEND PERMISSIONS "READ_EXTERNAL_STORAGE")
    endif()
  endif()
 
  if (PERMISSIONS)
    afix(APP_PERMISSIONS
      "      <uses-permission android:name=\"android.permission."
      "\" />" ${PERMISSIONS})
    string(REPLACE ";" "\n" WN_PERMISSIONS "${APP_PERMISSIONS}")
  endif()
  library_name(LIB_NAME ${PARSED_ARGS_TARGET})

  set(WN_ANDROID_GDB
    ${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-gdb${TOOL_OS_SUFFIX})

  get_target_property(SOLIB_DIRECTORIES ${PARSED_ARGS_TARGET} INCLUDE_DIRECTORIES)
  string(REPLACE ";" " " SOLIB_DIRECTORIES "${SOLIB_DIRECTORIES}")

  set(WN_NATIVE_LIBRARIES ${LIB_NAME})
  set(SOLIB_PATH ${WN_TARGET_DIR}/obj/local/${ANDROID_ABI}/)

  fix_path(${WN_TARGET_DIR} WN_TARGET_DIR)
  fix_path(${SOLIB_PATH} SOLIB_PATH)
  fix_path(${WN_ANDROID_GDB} WN_ANDROID_GDB)
  fix_path(${ANDROID_RUNNER} ANDROID_RUNNER)
  fix_path(${ANDROID_RELEASE_KEY_STORE} ANDROID_RELEASE_KEYSTORE_PATH)
  fix_path(${WN_APK_LOCATION} WN_APK_LOCATION)

  
  configure_file("${configure_source_dir}/AndroidManifest.xml.in"
    "${WN_TARGET_DIR}/AndroidManifest.xml")
  configure_file("${configure_source_dir}/strings.xml.in"
    "${WN_TARGET_DIR}/res/values/strings.xml")
  configure_file("${configure_source_dir}/LibraryLoader.java.in"
    "${WN_TARGET_DIR}/src/${PARSED_ARGS_TOP_LEVEL}/${PARSED_ARGS_DOMAIN}/${PARSED_ARGS_PROGRAM_NAME}/${WN_ACTIVITY_NAME}.java")
  configure_file("${configure_source_dir}/Android.mk.in"
    "${WN_TARGET_DIR}/jni/Android.mk")
  configure_file("${configure_source_dir}/Application.mk.in"
    "${WN_TARGET_DIR}/jni/Application.mk")
  configure_file("${configure_source_dir}/gdb.setup.in"
    "${WN_TARGET_DIR}/libs/${ANDROID_ABI}/gdb.setup")
  configure_file("${configure_source_dir}/quick_run.py.in"
    "${WN_TARGET_DIR}/${WN_TARGET_NAME}.py")
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if (ANDROID_RELEASE_KEY_ALIAS STREQUAL "androiddebugkey")
      if (ANDROID_RELEASE_KEY_STORE_PASSWORD STREQUAL "")
        set(ANDROID_RELEASE_KEY_STORE_PASSWORD "android")
      endif()
      if (ANDROID_RELEASE_KEY_ALIAS_PASSWORD STREQUAL "")
        set(ANDROID_RELEASE_KEY_ALIAS_PASSWORD "android")
      endif()
    endif()
    set(ANDROID_KEY_STORE_PASS_LINE "")
    set(ANDROID_KEY_ALIAS_PASS_LINE "")
    if (ANDROID_RELEASE_KEY_STORE_PASSWORD)
      set(ANDROID_KEY_STORE_PASS_LINE "key.store.password=${ANDROID_RELEASE_KEY_STORE_PASSWORD}")
    endif()
    if (ANDROID_RELEASE_KEY_ALIAS_PASSWORD)
      set(ANDROID_KEY_ALIAS_PASS_LINE "key.alias.password=${ANDROID_RELEASE_KEY_ALIAS_PASSWORD}")
    endif()

    configure_file("${configure_source_dir}/ant.properties.in"
      "${WN_TARGET_DIR}/ant.properties")
  endif()

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
      COMMAND ${CMAKE_COMMAND} -E make_directory "${SOLIB_PATH}")
    add_custom_command(TARGET ${WN_TARGET_NAME}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PARSED_ARGS_TARGET}>" "${SOLIB_PATH}"
      DEPENDS $<TARGET_FILE:${PARSED_ARGS_TARGET}>)
  endif()

  if (ANDROID_SDK)
    set(ANDROID_TOOL_PATH ${ANDROID_SDK}/tools/android)
    if (${ANDROID_SYSTEM_LOWER} STREQUAL "windows" OR
      ${ANDROID_SYSTEM_LOWER} STREQUAL "windows-x86_64")
      find_program(PYTHON python REQUIRED)
      set(ANDROID_TOOL_PATH ${PYTHON} ${configure_source_dir}/single_process_windows.py
        "WNFramework/AndroidUpdateProject"
        "${ANDROID_SDK}/tools/android.bat")
    endif()

# On windows we fail if we run more than once instance of android update at the same time.
    add_custom_command(TARGET ${WN_TARGET_NAME}
      POST_BUILD
      COMMAND  ${ANDROID_TOOL_PATH} update project -t
        android-${ANDROID_NATIVE_API_LEVEL}
        --name ${WN_TARGET_NAME} --path "${WN_TARGET_DIR}" > ${WN_NULL_LOCATION}
        COMMENT "Creating android project for ${WN_TARGET_NAME}")
    find_program(ANT ant)

    if (NOT ANT)
      message(WARNING "Cannot find ant, ANDROID APK will not get built")
    else()
      add_custom_command(TARGET ${WN_TARGET_NAME}
        POST_BUILD
        COMMAND ${ANT} "${WN_ANDROID_BUILD}" > ${WN_NULL_LOCATION}
        WORKING_DIRECTORY "${WN_TARGET_DIR}"
        COMMENT "Build android .apk for ${WN_TARGET_NAME}")
      add_custom_command(TARGET ${WN_TARGET_NAME}
        POST_BUILD
        COMMAND  ${CMAKE_COMMAND} -E copy_if_different
          ${WN_TARGET_DIR}/bin/${WN_APK_NAME}
          ${CMAKE_CURRENT_BINARY_DIR})
    endif()
  endif()

endfunction()
