
function(afix target prefix postfix)
  set(tempvar "")
  foreach(f ${ARGN})
    list(APPEND tempvar "${prefix}${f}${postfix}")
  endforeach()
  set(${target} "${tempvar}" PARENT_SCOPE)
endfunction()

macro(_accumulate_helper name accum_sources accum_libs)
  set(my_sources ${${accum_sources}})
  set(my_libs ${${accum_libs}})
  if(TARGET ${name})
    set(has -1)
    if(my_libs)
      list(FIND my_libs ${name} has)
    endif()
    if(${has} EQUAL -1)
      list(APPEND my_libs ${name})
      get_target_property(sources ${name} WN_SOURCES)
      if (sources)
        list(APPEND my_sources ${sources})
      endif()
      get_target_property(libs ${name} WN_LIBRARIES)
      if (libs)
        set(child_sources ${my_sources})
        set(child_libs ${my_libs})
        foreach (lib ${libs})
          _accumulate_helper(${lib} child_sources child_libs)
        endforeach()
        set(my_sources ${child_sources})
        set(my_libs ${child_libs})
      endif()
    endif()
  endif()
  set(${accum_sources} ${my_sources})
  set(${accum_libs} ${my_libs})
endmacro()

function(accumulate_sources name outvar)
  set(ACCUMULATED_LIBS)
  set(ACCUMULATED_SOURCES)
  _accumulate_helper(${name} ACCUMULATED_SOURCES ACCUMULATED_LIBS)
  set(${outvar} ${ACCUMULATED_SOURCES} PARENT_SCOPE)
endfunction()

function(add_application name)
  set(APPLICATION_NAME ${name})
  set(WN_BUILD_ROOT ${WNFramework_BINARY_DIR})
  set(LIB_NAME ${name})

  set(WN_ABI_LIST "'${WN_SANITIZED_ABIS}'")
  string(REPLACE ";" "', '" WN_ABI_LIST "${WN_ABI_LIST}")

  set(PERMISSIONS)
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(PERMISSIONS "READ_EXTERNAL_STORAGE")
  endif()

  if (PERMISSIONS)
    afix(APP_PERMISSIONS
      "      <uses-permission android:name=\"android.permission."
      "\" />" ${PERMISSIONS})
    string(REPLACE ";" "\n" WN_PERMISSIONS "${APP_PERMISSIONS}")
  endif()

  set(FILES_TO_CONFIGURE
    build.gradle
    gradle.properties
    local.properties
    settings.gradle
    app/build.gradle
    app/src/main/AndroidManifest.xml
    app/src/main/CMakeLists.txt
    app/src/main/res/values/strings.xml
  )
  set(FILES_TO_COPY
    gradlew.bat
    gradle/wrapper/gradle-wrapper.properties
  )
  set(ANDROID_RUNNER ${WNFramework_SOURCE_DIR}/Overlays/Posix/Overlays/Android/cmake/android_helpers/android_runner.py)
  set(WN_ACTIVITY_NAME ${APPLICATION_NAME})
  set(WN_APK_LOCATION ${WNFramework_BINARY_DIR}/apps/${name}-debug.apk)
  set(WN_PACKAGE ${WN_PACKAGE_NAME}.${WN_ACTIVITY_NAME})
  foreach(config ${FILES_TO_CONFIGURE})
    configure_file(
      ${WNFramework_SOURCE_DIR}/Overlays/Posix/Overlays/Android/cmake/android_helpers/gradle/${config}.in
      ${WNFramework_BINARY_DIR}/apps/${name}/${config}
      @ONLY)
  endforeach()
  foreach(copy ${FILES_TO_COPY})
    configure_file(
      ${WNFramework_SOURCE_DIR}/Overlays/Posix/Overlays/Android/cmake/android_helpers/gradle/${copy}.in
      ${WNFramework_BINARY_DIR}/apps/${name}/${copy}
      COPYONLY)
  endforeach()
  file(COPY
    ${WNFramework_SOURCE_DIR}/Overlays/Posix/Overlays/Android/cmake/android_helpers/gradle/gradlew.in
    DESTINATION
    ${WNFramework_BINARY_DIR}/apps/${name}/
    FILE_PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
        GROUP_EXECUTE
  )
  file(RENAME
    ${WNFramework_BINARY_DIR}/apps/${name}/gradlew.in
    ${WNFramework_BINARY_DIR}/apps/${name}/gradlew
  )
  file(MAKE_DIRECTORY
    ${WNFramework_BINARY_DIR}/apps/${name}/gradle/wrapper/
  )
  file(COPY
    ${WNFramework_SOURCE_DIR}/Overlays/Posix/Overlays/Android/cmake/android_helpers/gradle/gradle/wrapper/gradle-wrapper.jar.in
    DESTINATION
    ${WNFramework_BINARY_DIR}/apps/${name}/gradle/wrapper/
    FILE_PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
        GROUP_EXECUTE
  )
  file(RENAME
    ${WNFramework_BINARY_DIR}/apps/${name}/gradle/wrapper/gradle-wrapper.jar.in
    ${WNFramework_BINARY_DIR}/apps/${name}/gradle/wrapper/gradle-wrapper.jar
  )

  configure_file(${WNFramework_SOURCE_DIR}/Overlays/Posix/Overlays/Android/cmake/android_helpers/quick_run.py.in
                 ${WNFramework_BINARY_DIR}/apps/${name}.py)
  set(LIBS)
  foreach(ABI ${WN_SANITIZED_ABIS})
    if(NOT EXISTS ${WNFramework_BINARY_DIR}/${ABI}/lib/stripped/lib${name}.so)
      execute_process(
        COMMAND
          ${CMAKE_COMMAND} -E make_directory ${WNFramework_BINARY_DIR}/${ABI}/lib/stripped/
      )
      execute_process(COMMAND
          ${CMAKE_COMMAND} -E touch ${WNFramework_BINARY_DIR}/${ABI}/lib/stripped/lib${name}.so
      )
    endif()
    list(APPEND LIBS ${WNFramework_BINARY_DIR}/${ABI}/lib/stripped/lib${name}.so)
  endforeach()
  accumulate_sources(${name} all_sources)
  foreach(ABI ${WN_SANITIZED_ABIS})
    execute_process(
      COMMAND
        ${CMAKE_COMMAND} -E make_directory ${WNFramework_BINARY_DIR}/apps/${name}/app/src/main/jniLibs/${ABI}
    )
    list(APPEND ALL_LIBS
      COMMAND ${CMAKE_COMMAND} -E
          copy_if_different ${WNFramework_BINARY_DIR}/${ABI}/lib/stripped/lib${name}.so
          ${WNFramework_BINARY_DIR}/apps/${name}/app/src/main/jniLibs/${ABI}/lib${name}.so)
  endforeach()

  set(GRADLE_COMMAND_PREFIX "./")
  set(GRADLE_COMAND_POSTFIX "")
  if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(GRADLE_COMMAND_PREFIX "")
    set(GRADLE_COMMAND_POSTFIX ".bat")
  endif()

  add_custom_command(
    OUTPUT ${WNFramework_BINARY_DIR}/apps/${name}-debug.apk
             ${WNFramework_BINARY_DIR}/apps/${name}-unsigned.apk
      ${ALL_LIBS}
    COMMAND
      ${GRADLE_COMMAND_PREFIX}gradlew${GRADLE_COMMAND_POSTFIX} -g ${WNFramework_BINARY_DIR}/apps/${name}/.gradle_home build
    COMMAND
      ${CMAKE_COMMAND} -E copy
        ${WNFramework_BINARY_DIR}/apps/${name}/app/build/outputs/apk/app-debug.apk
        ${WNFramework_BINARY_DIR}/apps/${name}-debug.apk
    COMMAND
      ${CMAKE_COMMAND} -E copy
        ${WNFramework_BINARY_DIR}/apps/${name}/app/build/outputs/apk/app-release-unsigned.apk
        ${WNFramework_BINARY_DIR}/apps/${name}-unsigned.apk
    WORKING_DIRECTORY ${WNFramework_BINARY_DIR}/apps/${name}
    DEPENDS
      ${all_sources}
      ${LIBS}
  )

  add_custom_target(
    build-${app}-apk
    DEPENDS  ${WNFramework_BINARY_DIR}/apps/${name}-debug.apk
             ${WNFramework_BINARY_DIR}/apps/${name}-unsigned.apk)
endfunction()