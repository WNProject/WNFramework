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
    gradlew
    gradle/wrapper/gradle-wrapper.jar
    gradle/wrapper/gradle-wrapper.properties
  )

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

  add_custom_command(
    OUTPUT ${WNFramework_BINARY_DIR}/apps/${name}-debug.apk
             ${WNFramework_BINARY_DIR}/apps/${name}-unsigned.apk
      ${ALL_LIBS}
    COMMAND
      gradlew -g ${WNFramework_BINARY_DIR}/apps/${name}/.gradle_home build
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