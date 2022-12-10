set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate -u wn_main")

function(overload_add_library name)
  if (NOT WN_ANDROID_WRAPPER)
    cmake_parse_arguments(
      PARSED_ARGS
      "SHARED"
      ""
      "SOURCES;LIBS;INCLUDES;COMPILE_OPTIONS;PUBLIC_COMPILE_DEFINES"
      ${ARGN})
    set(LIB_TYPE STATIC)
    if (PARSED_ARGS_SHARED)
      set(LIB_TYPE SHARED)
    endif()
    add_library(${name} ${LIB_TYPE} ${PARSED_ARGS_SOURCES})
    if (PARSED_ARGS_LIBS)
      target_link_libraries(${name} ${PARSED_ARGS_LIBS})
    endif()
    if (PARSED_ARGS_INCLUDES)
      target_include_directories(${name} PUBLIC ${PARSED_ARGS_INCLUDES})
    endif()
    if (PARSED_ARGS_COMPILE_OPTIONS)
      target_compile_options(${name} PRIVATE ${PARSED_ARGS_COMPILE_OPTIONS})
    endif()
    if (PARSED_ARGS_PUBLIC_COMPILE_DEFINES)
      target_compile_definitions(${name} PUBLIC ${PARSED_ARGS_PUBLIC_COMPILE_DEFINES})
    endif()
    if (WN_ENABLE_TRACY)
      add_custom_command(TARGET ${name}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/stripped
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${name}>
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/stripped/$<TARGET_FILE_NAME:${name}>
      )
    else()
      if(ANDROID_TOOLCHAIN STREQUAL gcc)
        set(strip_exe ${ANDROID_TOOLCHAIN_PREFIX}strip${ANDROID_TOOLCHAIN_SUFFIX})
      else()
        set(strip_exe ${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-strip${ANDROID_TOOLCHAIN_SUFFIX})
      endif()
      add_custom_command(TARGET ${name}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/stripped
        COMMAND ${strip_exe}
          $<TARGET_FILE:${name}> -o
          ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/stripped/$<TARGET_FILE_NAME:${name}>
      )
    endif()
  else()
    cmake_parse_arguments(
        PARSED_ARGS
        "SHARED"
        ""
        "SOURCES;LIBS;INCLUDES;COMPILE_OPTIONS"
        ${ARGN})

    set(TARGETS)
    foreach(BUILD_ABI ${WN_SANITIZED_ABIS})
        add_custom_target(${name}-${BUILD_ABI}
            COMMAND cmake --build . --target ${name}
            WORKING_DIRECTORY ${WNFramework_BINARY_DIR}/${BUILD_ABI}
            SOURCES ${PARSED_ARGS_SOURCES})
        list(APPEND TARGETS ${name}-${BUILD_ABI})
    endforeach()
    add_custom_target(${name} DEPENDS ${TARGETS})
    set(ABSOLUTE_SOURCES)
    foreach(source ${PARSED_ARGS_SOURCES})
      get_filename_component(source ${source} ABSOLUTE)
      list(APPEND ABSOLUTE_SOURCES ${source})
    endforeach()
    set_target_properties(${name} PROPERTIES WN_SOURCES
      "${ABSOLUTE_SOURCES}")
    set_target_properties(${name} PROPERTIES WN_LIBRARIES
        "${PARSED_ARGS_LIBS}")
  endif()
endfunction()

function(overload_add_executable name)
  if (NOT WN_ANDROID_WRAPPER)
      cmake_parse_arguments(
      PARSED_ARGS
      ""
      ""
      "SOURCES;LIBS;INCLUDES"
      ${ARGN})
    add_executable(${name} ${PARSED_ARGS_SOURCES})
    if (PARSED_ARGS_LIBS)
      target_link_libraries(${name} ${PARSED_ARGS_LIBS})
    endif()

    configure_file(
      ${WNFramework_SOURCE_DIR}/Overlays/Posix/Overlays/Android/cmake/android_helpers/standalone/native_runner.py.in
                 ${WNFramework_BINARY_DIR}/bin/${name}.py)

    if (PARSED_ARGS_INCLUDES)
      target_include_directories(${name} PUBLIC ${PARSED_ARGS_INCLUDES})
    endif()
    if (WN_ENABLE_TRACY)
      add_custom_command(TARGET ${name}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/stripped
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${name}>
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/stripped/$<TARGET_FILE_NAME:${name}>
      )
    else()
      if(ANDROID_TOOLCHAIN STREQUAL gcc)
        set(strip_exe ${ANDROID_TOOLCHAIN_PREFIX}strip${ANDROID_TOOLCHAIN_SUFFIX})
      else()
        set(strip_exe ${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-strip${ANDROID_TOOLCHAIN_SUFFIX})
      endif()
      add_custom_command(TARGET ${name}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/stripped
        COMMAND ${strip_exe}
          $<TARGET_FILE:${name}> -o
          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/stripped/$<TARGET_FILE_NAME:${name}>
      )
    endif()
    
  else()
    cmake_parse_arguments(
        PARSED_ARGS
        ""
        ""
        "SOURCES;LIBS"
        ${ARGN})
    add_custom_target(${name})
    set(ABSOLUTE_SOURCES)
     foreach(source ${PARSED_ARGS_SOURCES})
        get_filename_component(source ${source} ABSOLUTE)
        list(APPEND ABSOLUTE_SOURCES ${source})
     endforeach()
     set_target_properties(${name} PROPERTIES WN_SOURCES
        "${ABSOLUTE_SOURCES}")
    set_target_properties(${name} PROPERTIES WN_LIBRARIES
        "${PARSED_ARGS_LIBS}")

    foreach(BUILD_ABI ${WN_FORCED_ABIS})
        add_custom_target(${name}-${BUILD_ABI}
            COMMAND cmake --build . --target ${name}
            WORKING_DIRECTORY ${WNFramework_BINARY_DIR}\${BUILD_ABI})
        add_dependencies(${name}-${BUILD_ABI} ${PARSED_ARGS_SOURCES})
        add_dependencies(${name} ${name}-${BUILD_ABI})
    endforeach()
  endif()
endfunction()

function(overload_add_application name)
  cmake_parse_arguments(
        PARSED_ARGS
        ""
        ""
        "SOURCES;LIBS;INCLUDES"
        ${ARGN})
  if (NOT WN_ANDROID_WRAPPER)
    overload_add_library(${name} SHARED ${ARGN})
     if (WN_ENABLE_TRACY)
      add_custom_command(TARGET ${name}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/stripped
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${name}>
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/stripped/$<TARGET_FILE_NAME:${name}>
      )
    else()
    if(ANDROID_TOOLCHAIN STREQUAL gcc)
      set(strip_exe ${ANDROID_TOOLCHAIN_PREFIX}strip${ANDROID_TOOLCHAIN_SUFFIX})
    else()
      set(strip_exe ${ANDROID_TOOLCHAIN_ROOT}/bin/llvm-strip${ANDROID_TOOLCHAIN_SUFFIX})
    endif()
      add_custom_command(TARGET ${name}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/stripped
        COMMAND ${strip_exe}
          $<TARGET_FILE:${name}> -o
          ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/stripped/$<TARGET_FILE_NAME:${name}>
      )
    endif()
  else()
      add_custom_target(${name})
     set(ABSOLUTE_SOURCES)
     foreach(source ${PARSED_ARGS_SOURCES})
        get_filename_component(s ${source} ABSOLUTE)
        list(APPEND ABSOLUTE_SOURCES ${s})
     endforeach()
     set_target_properties(${name} PROPERTIES WN_SOURCES
        "${ABSOLUTE_SOURCES}")
     set_target_properties(${name} PROPERTIES WN_LIBRARIES
        "${PARSED_ARGS_LIBS}")
  endif()
endfunction()
