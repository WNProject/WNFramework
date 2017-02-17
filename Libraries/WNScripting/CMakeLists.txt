set(WN_LLVM_EXTRA_FLAGS)
set(WN_LLVM_WRAPPER)
set(WN_LLVM_TOOLCHAIN)
set(WN_LLVM_CXX_COMPILER)
set(WN_LLVM_C_COMPILER)
set(WN_LLVM_TRIPLE)
set(WN_LLVM_TARGET)
set(WN_LLVM_ADDITIONAL_BUILD_FLAGS)
set(WN_LLVM_ADDITIONAL_BUILD_FLAGS)
set(WN_LLVM_OVERRIDDEN_TRIPLE "WN_LLVM_NON_OVERRIDDEN_TRIPLE")
enable_overlay_directory()
if(WN_LLVM_TRIPLE)
  set(WN_LLVM_OVERRIDDEN_TRIPLE "WN_LLVM_OVERRIDDEN_TRIPLE")
endif()

set(WN_LLVM_SOURCE_DIR
    ${WNFramework_SOURCE_DIR}/Externals/llvm
    CACHE STRING "Location of the llvm source checkout"
)

file(TO_CMAKE_PATH ${WN_LLVM_SOURCE_DIR} LLVM_SOURCE_DIR_CMAKE_PATH)

if (NOT LLVM_CONFIGURED)
  if(WN_LLVM_CXX_COMPILER)
    list(APPEND WN_LLVM_EXTRA_FLAGS
      "-DCROSS_TOOLCHAIN_FLAGS_NATIVE=-DCMAKE_C_COMPILER=${WN_LLVM_C_COMPILER}\;-DCMAKE_CXX_COMPILER=${WN_LLVM_CXX_COMPILER}")
  endif()
  if(CMAKE_GENERATOR_TOOLSET)
    list(APPEND WN_LLVM_EXTRA_FLAGS "-T\"${CMAKE_GENERATOR_TOOLSET}\"")
  endif()
  if (NOT WN_LLVM_TOOLCHAIN)
    set(WN_LLVM_TOOLCHAIN ${CMAKE_TOOLCHAIN_FILE})
  endif()
  if (WN_LLVM_TOOLCHAIN)
    list(APPEND WN_LLVM_EXTRA_FLAGS "-DCMAKE_TOOLCHAIN_FILE=${WN_LLVM_TOOLCHAIN}")
  endif()

  list(APPEND WN_LLVM_EXTRA_FLAGS "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")

  execute_process(
   COMMAND ${CMAKE_COMMAND} -E make_directory
    ${WNFramework_BINARY_DIR}/Externals
    COMMAND ${CMAKE_COMMAND} -E make_directory
    ${WNFramework_BINARY_DIR}/Externals/llvm
  )

  execute_process(
    COMMAND ${WN_LLVM_WRAPPER} ${CMAKE_COMMAND}
    -DLLVM_BUILD_TOOLS=OFF
    -DLLVM_INCLUDE_TOOLS=OFF
    -DLLVM_BUILD_TOOLS=OFF
    -DLLVM_BUILD_EXAMPLES=OFF
    -DLLVM_INCLUDE_EXAMPLES=OFF
    -DLLVM_BUILD_TESTS=OFF
    -DLLVM_INCLUDE_TESTS=OFF
    -DLLVM_BUILD_UTILS=OFF
    -DLLVM_INCLUDE_UTILS=OFF
    -DLLVM_ENABLE_PLUGINS=OFF
    -DLLVM_ENABLE_ZLIB=OFF
    -DLLVM_HOST_TRIPLE=${WN_LLVM_TRIPLE}
    -DLLVM_DEFAULT_TARGET_TRIPLE=${WN_LLVM_TRIPLE}
    -DLLVM_TARGETS_TO_BUILD=${WN_LLVM_TARGET}
    ${WN_LLVM_EXTRA_FLAGS}
    -G ${CMAKE_GENERATOR} ${WN_LLVM_SOURCE_DIR}
    WORKING_DIRECTORY ${WNFramework_BINARY_DIR}/Externals/llvm
  )
  set(LLVM_CONFIGURED TRUE CACHE INTERNAL "llvm has been configured" FORCE)
endif()

add_custom_target(llvm_target
    COMMAND ${CMAKE_COMMAND} --build . ${WN_LLVM_ADDITIONAL_BUILD_FLAGS}
    WORKING_DIRECTORY ${WNFramework_BINARY_DIR}/Externals/llvm)

include(${WNFramework_BINARY_DIR}/Externals/llvm/lib/cmake/llvm/LLVMConfig.cmake)

set(WNScriptingLLVMComponents codegen mcjit native)
llvm_map_components_to_libnames(WNScriptingLLVMLibs ${WNScriptingLLVMComponents})

option(WN_BUILD_LLVM_MANUALLY
  "Do not automatically build llvm when a dependency is built"
  ${WN_BUILD_LLVM_MANUALLY})

if (NOT ${WN_BUILD_LLVM_MANUALLY})
  foreach (llvm_lib ${WNScriptingLLVMLibs})
    add_dependencies(${llvm_lib} llvm_target)
  endforeach()
endif()
configure_file(
  inc/WNJITConfiguration.h.in
  inc/WNJITConfiguration.h)

add_wn_library(WNScripting
  SOURCES
    inc/WNASTWalker.h
    inc/WNASTWalker.inl
    inc/WNASTCodeGenerator.h
    inc/WNASTCodeGenerator.inl
    inc/WNASTPasses.h
    inc/WNCGenerator.h
    inc/WNCTranslator.h
    inc/WNEngine.h
    inc/WNEngine.inl
    inc/WNFactory.h
    inc/WNEnums.h
    inc/WNErrors.h
    inc/WNJITConfiguration.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inc/WNJITConfiguration.h
    inc/WNJITEngine.h
    inc/WNJITGenerator.h
    inc/WNNodeTypes.h
    inc/WNScriptHelpers.h
    inc/WNTranslator.h
    inc/WNTypeValidator.h
    src/WNASTPasses.cpp
    src/WNCGenerator.cpp
    src/WNCTranslator.cpp
    src/WNFactory.cpp
    src/WNJITEngine.cpp
    src/WNJITGenerator.cpp
    src/WNScriptASTLexer.hpp
    src/WNScriptASTParser.hpp
    src/WNScriptASTLexer.cpp
    src/WNScriptASTParser.cpp
    src/WNScriptHelpers.cpp
    lang/WNScriptAST.g
  LIBS
    WNFileSystem
    ${WNScriptingLLVMLibs}
  INCLUDES
    ${CMAKE_SOURCE_DIR}/Externals/antlr
    ${LLVM_SOURCE_DIR_CMAKE_PATH}/include
    ${CMAKE_BINARY_DIR}/Externals/llvm/include
)

set_source_files_properties(lang/WNScriptAST.g
  PROPERTIES HEADER_FILE_ONLY true)

add_subdirectory(test)
