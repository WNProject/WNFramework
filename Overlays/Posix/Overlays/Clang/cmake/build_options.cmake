set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")

add_compile_options(-Wno-gcc-compat)
add_compile_options(-Wno-missing-braces)
add_compile_options(-Wno-unused-private-field)
add_compile_options(-Wno-pessimizing-move)