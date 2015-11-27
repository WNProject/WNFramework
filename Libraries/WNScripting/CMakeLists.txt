add_wn_library(WNScripting SOURCES
  inc/WNASTWalker.h
  inc/WNASTWalker.inl
  inc/WNCGenerator.h
  inc/WNCTranslator.h
  inc/WNEngine.h
  inc/WNFactory.h
  inc/WNEngineFileManager.h
  inc/WNEnums.h
  inc/WNErrors.h
  inc/WNJITEngine.h
  inc/WNJITGenerator.h
  inc/WNNodeTypes.h
  inc/WNScriptHelpers.h
  inc/WNTranslator.h
  inc/WNTypeValidator.h
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
)

set(WNScriptingLLVMComponents codegen instrumentation objcarcopts support mcjit ipo native)
if (WN_SYSTEM STREQUAL "Android")
  add_wn_library(WNScriptingFixes SOURCES src/Android/AndroidFixes.cpp)
  set(WNScriptingAdditionalLibs WNScriptingFixes)
endif()

wn_target_link_libraries(WNScripting ${WNScriptingLLVMLibs}
  ${WNScriptingAdditionalLibs})
add_definitions(${LLVM_DEFINITIONS})

target_include_directories(WNScripting PUBLIC
  ${CMAKE_SOURCE_DIR}/Externals/antlr
  ${CMAKE_SOURCE_DIR}/Externals/llvm/include
  ${CMAKE_BINARY_DIR}/Externals/llvm/include
  ${CMAKE_SOURCE_DIR}/Libraries)

add_subdirectory(test)
