
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}")
 
# -DDEBUG conflicts with some libraries, remove it here
string(REPLACE "-DDEBUG" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

add_compile_options(-D_WN_ANDROID)
add_compile_options(-D_XOPEN_SOURCE=600)

if (NOT WN_ANDROID_SDK)
  message(STATUS "Warning WN_ANDROID_SDK not defined, applications will not"
    " be built")
endif()

find_program(WN_PYTHON Python)
