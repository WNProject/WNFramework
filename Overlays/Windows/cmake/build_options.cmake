set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Fix for LLVM Visual Studio 2015 RC issue
if(MSVC)
  if(MSVC_VERSION GREATER 1800)
    list(APPEND WN_LLVM_EXTRA_FLAGS "-DCMAKE_CXX_FLAGS=/Zc:sizedDealloc-")
  endif()
endif()
# Preprocessor definitions
add_compile_options(/D_CRT_SECURE_NO_WARNINGS) # Disable CRT secure warnings
add_compile_options(/D_SCL_SECURE_NO_WARNINGS) # Disable SCL secure warnings
add_compile_options(/D_HAS_EXCEPTIONS=0) # Disable STL exceptions
# Compiler options

foreach(lang CMAKE_CXX_FLAGS CMAKE_C_FLAGS
             CMAKE_CXX_FLAGS_RELEASE CMAKE_C_FLAGS_RELEASE
             CMAKE_CXX_FLAGS_DEBUG CMAKE_C_FLAGS_DEBUG
             CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS_RELWITHDEBINFO
             CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_C_FLAGS_MINSIZEREL)
  # Remove any flags related to exceptions
  if(${lang} MATCHES "/EHsc")
    string(REGEX REPLACE "/EHsc" "" ${lang} "${${lang}}")
  endif()
  # Remove any flags related to rtti
  if(${lang} MATCHES "/GR")
    string(REGEX REPLACE "/GR" "" ${lang} "${${lang}}")
  endif()
  # Remove any flags related to warning level
  if(${lang} MATCHES "/W3")
    string(REGEX REPLACE "/W3" "" ${lang} "${${lang}}")
  endif()
  # Remove any flags related to dynamic crt
  if(${lang} MATCHES "/MDd")
    string(REGEX REPLACE "/MDd" "" ${lang} "${${lang}}")
  endif()
  # Remove any flags related to dynamic crt
  if(${lang} MATCHES "/MD")
    string(REGEX REPLACE "/MD" "" ${lang} "${${lang}}")
  endif()
endforeach()
add_compile_options(/W4) # Adjust warnings to level 4
add_compile_options(/WX) # Enable warnings as errors
add_compile_options(/GR-) # Disable run-time type information
add_compile_options(/MP) # Enable multi-processor compilation

# Adjust CRT usage to static for all configs
add_compile_options($<$<CONFIG:Debug>:/MTd> 
                    $<$<NOT:$<CONFIG:Debug>>:/MT>)
# Adjust optimization to full optimization
add_compile_options($<$<CONFIG:Release>:/Ox>)
# Adjust inlining to any suitable
add_compile_options($<$<CONFIG:Release>:/Ob2>)
 # Adjust to favour size (tend to still be better than /Ot which favours speed)
add_compile_options($<$<CONFIG:Release>:/Os>)
add_compile_options($<$<CONFIG:Release>:/Oy>) # Adjust to remove frame pointers
add_compile_options($<$<CONFIG:Release>:/Oi>) # Enable intrinsic functions
add_compile_options($<$<CONFIG:Release>:/GT>) # Enable fiber-safe optimizations
# Enable whole program optimization
add_compile_options($<$<CONFIG:Release>:/GL>) 
add_compile_options($<$<CONFIG:Release>:/Gm->) # Disable minimal rebuild

add_compile_options(/D_WN_WINDOWS)

# Linker options
foreach(type EXE SHARED STATIC)
  # Enable link time code generation
  if(NOT "${CMAKE_${type}_LINKER_FLAGS_RELEASE}" MATCHES "/LTCG")
    set(CMAKE_${type}_LINKER_FLAGS_RELEASE
      "${CMAKE_${type}_LINKER_FLAGS_RELEASE} /LTCG")
  endif()
  if(NOT "${type}" STREQUAL "STATIC")
    # Adjust to remove unreferenced code
    if("${CMAKE_${type}_LINKER_FLAGS_RELEASE}" MATCHES "/OPT:NOREF")
      string(REGEX REPLACE "/OPT:NOREF" "/OPT:REF"
        CMAKE_${type}_LINKER_FLAGS_RELEASE
        "${CMAKE_${type}_LINKER_FLAGS_RELEASE}")
    elseif(NOT "${CMAKE_${type}_LINKER_FLAGS_RELEASE}" MATCHES "/OPT:REF")
      set(CMAKE_${type}_LINKER_FLAGS_RELEASE
        "${CMAKE_${type}_LINKER_FLAGS_RELEASE} /OPT:REF")
    endif()
    # Adjust to enable COMDAT folding
    if("${CMAKE_${type}_LINKER_FLAGS_RELEASE}" MATCHES "/OPT:NOICF")
      string(REGEX REPLACE "/OPT:NOICF" "/OPT:ICF"
        CMAKE_${type}_LINKER_FLAGS_RELEASE
        "${CMAKE_${type}_LINKER_FLAGS_RELEASE}")
    elseif(NOT "${CMAKE_${type}_LINKER_FLAGS_RELEASE}" MATCHES "/OPT:ICF")
      set(CMAKE_${type}_LINKER_FLAGS_RELEASE
        "${CMAKE_${type}_LINKER_FLAGS_RELEASE} /OPT:ICF")
    endif()
  endif()
endforeach()