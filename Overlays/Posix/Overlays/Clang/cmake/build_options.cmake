set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

add_compile_options(-Wno-gcc-compat)
add_compile_options(-Wno-missing-braces)
add_compile_options(-Wno-unused-private-field)
add_compile_options(-Wno-pessimizing-move)

if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
  add_compile_options("-Wno-unused-lambda-capture")
endif()
