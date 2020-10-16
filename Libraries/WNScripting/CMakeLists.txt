set(WN_LLVM_EXTRA_FLAGS)
set(WN_LLVM_WRAPPER)
set(WN_LLVM_TOOLCHAIN)
set(WN_LLVM_CXX_COMPILER)
set(WN_LLVM_C_COMPILER)
set(WN_LLVM_TRIPLE)
set(WN_LLVM_TARGET)
set(WN_LLVM_ADDITIONAL_BUILD_FLAGS)
set(WN_LLVM_OVERRIDDEN_TRIPLE "WN_LLVM_NON_OVERRIDDEN_TRIPLE")
enable_overlay_directory()
if (NOT WN_DISABLE_CXX)
  if(WN_LLVM_TRIPLE)
    set(WN_LLVM_OVERRIDDEN_TRIPLE "WN_LLVM_OVERRIDDEN_TRIPLE")
  endif()

  set(WN_LLVM_SOURCE_DIR
      ${WNFramework_SOURCE_DIR}/Externals/llvm-project/llvm
      CACHE STRING "Location of the llvm source checkout"
  )

  file(TO_CMAKE_PATH ${WN_LLVM_SOURCE_DIR} LLVM_SOURCE_DIR_CMAKE_PATH)

  if (NOT LLVM_CONFIGURED)
    if(WN_LLVM_CXX_COMPILER)
      list(APPEND WN_LLVM_EXTRA_FLAGS
        "-DCROSS_TOOLCHAIN_FLAGS_NATIVE=-DCMAKE_C_COMPILER=${WN_LLVM_C_COMPILER}\;-DCMAKE_CXX_COMPILER=${WN_LLVM_CXX_COMPILER}")
    endif()

    if(CMAKE_GENERATOR_TOOLSET)
      list(APPEND WN_LLVM_EXTRA_FLAGS "-T${CMAKE_GENERATOR_TOOLSET}")
    endif()

    if (NOT WN_LLVM_TOOLCHAIN)
      set(WN_LLVM_TOOLCHAIN ${CMAKE_TOOLCHAIN_FILE})
    endif()

    if (WN_LLVM_TOOLCHAIN)
      list(APPEND WN_LLVM_EXTRA_FLAGS "-DCMAKE_TOOLCHAIN_FILE=${WN_LLVM_TOOLCHAIN}")
    endif()

    if (CMAKE_GENERATOR_PLATFORM)
      list(APPEND WN_LLVM_EXTRA_FLAGS "-A${CMAKE_GENERATOR_PLATFORM}")
    endif()

    list(APPEND WN_LLVM_EXTRA_FLAGS "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")

    execute_process(
     COMMAND ${CMAKE_COMMAND} -E make_directory
      ${WNFramework_BINARY_DIR}/Externals/llvm-project/llvm
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${WNFramework_BINARY_DIR}/Externals/llvm-project/llvm
    )

    message(
      "COMMAND ${WN_LLVM_WRAPPER} ${CMAKE_COMMAND} "
      "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} "
      "-DLLVM_BUILD_TOOLS=OFF "
      "-DLLVM_INCLUDE_TOOLS=OFF "
      "-DLLVM_BUILD_RUNTIMES=OFF "
      "-DLLVM_INCLUDE_RUNTIMES=OFF "
      "-DLLVM_BUILD_RUNTIME=OFF "
      "-DLLVM_BUILD_EXAMPLES=OFF "
      "-DLLVM_INCLUDE_EXAMPLES=OFF "
      "-DLLVM_BUILD_TESTS=OFF "
      "-DLLVM_INCLUDE_TESTS=OFF "
      "-DLLVM_INCLUDE_GO_TESTS=OFF "
      "-DLLVM_BUILD_BENCHMARKS=OFF "
      "-DLLVM_INCLUDE_BENCHMARKS=OFF "
      "-DLLVM_BUILD_UTILS=OFF "
      "-DLLVM_INCLUDE_UTILS=OFF "
      "-DLLVM_ENABLE_PLUGINS=OFF "
      "-DLLVM_ENABLE_ZLIB=OFF "
      "-DLLVM_HAVE_LINK_VERSION_SCRIPT=OFF "
      "-DLLVM_HOST_TRIPLE=${WN_LLVM_TRIPLE} "
      "-DLLVM_DEFAULT_TARGET_TRIPLE=${WN_LLVM_TRIPLE} "
      "-DLLVM_TARGETS_TO_BUILD=${WN_LLVM_TARGET} "
      "-DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG} "
      "-DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE} "
      "-DCMAKE_CXX_FLAGS_MINSIZEREL=${CMAKE_CXX_FLAGS_MINSIZEREL} "
      "-DCMAKE_CXX_FLAGS_RELWITHDEBINFO=${CMAKE_CXX_FLAGS_RELWITHDEBINFO} "
      "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} "
      "-DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER} "
      "-G ${CMAKE_GENERATOR} "
      "${WN_LLVM_EXTRA_FLAGS} "
      "${WN_LLVM_SOURCE_DIR} "
      "WORKING_DIRECTORY ${WNFramework_BINARY_DIR}/Externals/llvm-project/llvm"
    )

    execute_process(
      COMMAND ${WN_LLVM_WRAPPER} ${CMAKE_COMMAND}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DLLVM_BUILD_TOOLS=OFF
      -DLLVM_INCLUDE_TOOLS=OFF
      -DLLVM_BUILD_RUNTIMES=OFF
      -DLLVM_INCLUDE_RUNTIMES=OFF
      -DLLVM_BUILD_RUNTIME=OFF
      -DLLVM_BUILD_EXAMPLES=OFF
      -DLLVM_INCLUDE_EXAMPLES=OFF
      -DLLVM_BUILD_TESTS=OFF
      -DLLVM_INCLUDE_TESTS=OFF
      -DLLVM_INCLUDE_GO_TESTS=OFF
      -DLLVM_BUILD_BENCHMARKS=OFF
      -DLLVM_INCLUDE_BENCHMARKS=OFF
      -DLLVM_BUILD_UTILS=OFF
      -DLLVM_INCLUDE_UTILS=OFF
      -DLLVM_ENABLE_PLUGINS=OFF
      -DLLVM_ENABLE_ZLIB=OFF
      -DLLVM_HAVE_LINK_VERSION_SCRIPT=OFF
      -DLLVM_HOST_TRIPLE=${WN_LLVM_TRIPLE}
      -DLLVM_DEFAULT_TARGET_TRIPLE=${WN_LLVM_TRIPLE}
      -DLLVM_TARGETS_TO_BUILD=${WN_LLVM_TARGET}
      -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
      -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
      -DCMAKE_CXX_FLAGS_MINSIZEREL=${CMAKE_CXX_FLAGS_MINSIZEREL}
      -DCMAKE_CXX_FLAGS_RELWITHDEBINFO=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
      -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
      -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
      -G ${CMAKE_GENERATOR}
      ${WN_LLVM_EXTRA_FLAGS}
      ${WN_LLVM_SOURCE_DIR}
      WORKING_DIRECTORY ${WNFramework_BINARY_DIR}/Externals/llvm-project/llvm
    )
    set(LLVM_CONFIGURED TRUE CACHE INTERNAL "llvm has been configured" FORCE)
  endif()

  add_custom_target(llvm_target
      COMMAND ${CMAKE_COMMAND} --build . ${WN_LLVM_ADDITIONAL_BUILD_FLAGS}
      WORKING_DIRECTORY ${WNFramework_BINARY_DIR}/Externals/llvm-project/llvm)

  include(${WNFramework_BINARY_DIR}/Externals/llvm-project/llvm/lib/cmake/llvm/LLVMConfig.cmake)

  set(WNScriptingLLVMComponents codegen mcjit native)
  llvm_map_components_to_libnames(WNScriptingLLVMLibs ${WNScriptingLLVMComponents})

  option(WN_BUILD_LLVM_MANUALLY
    "Do not automatically build llvm when a dependency is built"
    ${WN_BUILD_LLVM_MANUALLY})

  if (NOT ${WN_BUILD_LLVM_MANUALLY})
    llvm_expand_dependencies(l ${WNScriptingLLVMLibs})
    foreach (llvm_lib ${l})
      if("${CMAKE_CFG_INTDIR}" STREQUAL ".")
        string(TOUPPER ${CMAKE_BUILD_TYPE} t)
        get_target_property(loc ${llvm_lib} IMPORTED_LOCATION_${t})
        if(NOT EXISTS ${loc})
          execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${loc})
        endif()
      else()
        foreach(type ${CMAKE_CONFIGURATION_TYPES})
          string(TOUPPER ${type} t)
          get_target_property(loc ${llvm_lib} IMPORTED_LOCATION_${t})
          if (NOT EXISTS ${loc})
            execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${loc})
          endif()
        endforeach()
      endif()
    endforeach()
    foreach (llvm_lib ${WNScriptingLLVMLibs})
      add_dependencies(${llvm_lib} llvm_target)
    endforeach()
  endif()
endif()
configure_file(
  inc/WNJITConfiguration.h.in
  inc/WNJITConfiguration.h)

add_wn_library(WNScripting
  SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/inc/WNJITConfiguration.h
    inc/ast_node_types.h
    inc/c_compiler.h
    inc/externals.h
    inc/forward.h
    inc/internal/ast_convertor_context.h
    inc/internal/c_preamble.h
    inc/jit_compiler.h
    inc/parse_ast_convertor.h
    inc/resource_manager.h
    inc/source_location.h
    inc/type_manager.h
    inc/type_manager.inl
    inc/WNCTranslator.h
    inc/WNEngine.h
    inc/WNEngine.inl
    inc/WNEnums.h
    inc/WNErrors.h
    inc/WNFactory.h
    inc/WNJITConfiguration.h.in
    inc/WNJITEngine.h
    inc/WNNodeTypes.h
    inc/WNScriptHelpers.h
    inc/WNScriptTLS.h
    inc/WNTranslator.h
    lang/WNScriptAST.g
    src/add_builtins.cpp
    src/c_compiler.cpp
    src/conversion_helpers.cpp
    src/convert_expression_to_ast.cpp
    src/convert_function_to_ast.cpp
    src/convert_parse_to_ast.cpp
    src/convert_statement_to_ast.cpp
    src/convert_struct_to_ast.cpp
    src/convert_types_to_ast.cpp
    src/handle_scopes.cpp
    src/jit_compiler.cpp
    src/type_manager.cpp
    src/WNCTranslator.cpp
    src/WNFactory.cpp
    src/WNJITEngine.cpp
    src/WNScriptASTLexer.cpp
    src/WNScriptASTLexer.hpp
    src/WNScriptASTParser.cpp
    src/WNScriptASTParser.hpp
    src/WNScriptHelpers.cpp
  LIBS
    WNFileSystem
    WNLogging
    ${WNScriptingLLVMLibs}
  INCLUDES
    ${CMAKE_SOURCE_DIR}/Externals/antlr
    ${LLVM_SOURCE_DIR_CMAKE_PATH}/include
    ${CMAKE_BINARY_DIR}/Externals/llvm-project/llvm/include
)

set_source_files_properties(lang/WNScriptAST.g
  PROPERTIES HEADER_FILE_ONLY true)
wn_add_subdirectory(test)

post_enable_overlay_directory()
