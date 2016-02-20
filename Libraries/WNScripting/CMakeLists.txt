set(WN_SCRIPT_SOURCES
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
  src/WNScriptHelpers.cpp)


if (WN_SYSTEM STREQUAL "Windows")
  list(APPEND WN_SCRIPT_SOURCES lang/WNScriptAST.g)
endif()
add_wn_library(WNScripting SOURCES
  ${WN_SCRIPT_SOURCES}
)

if (WN_SYSTEM STREQUAL "Windows")
  set_source_files_properties(lang/WNScriptAST.g
    PROPERTIES HEADER_FILE_ONLY true)
  source_group("lang" FILES lang/WNScriptAST.g)
endif()

set(WNScriptingLLVMComponents codegen instrumentation objcarcopts support mcjit ipo native)
if (WN_SYSTEM STREQUAL "Android")
  add_wn_library(WNScriptingFixes SOURCES src/Android/AndroidFixes.cpp)
  set(WNScriptingAdditionalLibs WNScriptingFixes)
endif()

wn_target_link_libraries(WNScripting ${WNScriptingLLVMLibs}
  ${WNScriptingAdditionalLibs}
  WNFileSystem)
add_definitions(${LLVM_DEFINITIONS})

target_include_directories(WNScripting PUBLIC
  ${CMAKE_SOURCE_DIR}/Externals/antlr
  ${CMAKE_SOURCE_DIR}/Externals/llvm/include
  ${CMAKE_BINARY_DIR}/Externals/llvm/include
)

add_subdirectory(test)
