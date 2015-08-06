add_wn_library(WNScripting SOURCES
  inc/WNNodeTypes.h
  inc/WNScriptingEnums.h
  inc/WNScriptingErrors.h
  inc/WNScriptingJITEngine.h
  inc/WNScriptingEngineFileManager.h
  src/WNScriptingJITEngine.cpp
  src/WNScriptingEngineFactory.cpp
  src/WNScriptASTLexer.hpp
  src/WNScriptASTParser.hpp
  src/WNScriptASTLexer.cpp
  src/WNScriptASTParser.cpp
  inc/WNScriptingEngine.h
  inc/WNScriptingEngineFactory.h
)

set(WNScriptingLLVMComponents codegen instrumentation objcarcopts support mcjit ipo)
if (WN_SYSTEM STREQUAL "Android")
  list(APPEND WNScriptingLLVMComponents "native")
  add_wn_library(WNScriptingFixes SOURCES src/Android/AndroidFixes.cpp)
  set(WNScriptingAdditionalLibs WNScriptingFixes)
elseif (WN_SYSTEM STREQUAL "RPI")
  list(APPEND WNScriptingLLVMComponents "native")
elseif (WN_SYSTEM STREQUAL "Linux")
  list(APPEND WNScriptingLLVMComponents "native")
elseif (WN_SYSTEM STREQUAL "Windows")
  list(APPEND WNScriptingLLVMComponents "native")
endif()

llvm_map_components_to_libnames(llvm_libs ${WNScriptingLLVMComponents})

wn_target_link_libraries(WNScripting ${llvm_libs}
  ${WNScriptingAdditionalLibs})
add_definitions(${LLVM_DEFINITIONS})

target_include_directories(WNScripting PUBLIC
  ${CMAKE_SOURCE_DIR}/Externals/antlr
  ${CMAKE_SOURCE_DIR}/Externals/llvm/include
  ${CMAKE_BINARY_DIR}/Externals/llvm/include
  ${CMAKE_SOURCE_DIR}/Libraries)

add_subdirectory(test)