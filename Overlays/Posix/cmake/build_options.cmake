set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")

add_compile_options(-D_WN_POSIX)
add_compile_options(-D_XOPEN_SOURCE=600)
add_compile_options(-D__STDC_LIMIT_MACROS)
add_compile_options(-D__STDC_CONSTANT_MACROS)
