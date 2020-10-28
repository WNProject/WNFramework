set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

if(WN_LOW_RESOURCE_MODE)
  set(WN_MSVC_DISABLE_INCREMENTAL_LINKING ON)
  set(WN_MSVC_DISABLE_LINK_PARALLELIZATION ON)
endif()

option(
  WN_MSVC_DISABLE_INCREMENTAL_LINKING
  "Disable incremental linking (reduces build size at the cost of link time)"
  ${WN_MSVC_DISABLE_INCREMENTAL_LINKING}
)
option(
  WN_MSVC_DISABLE_LINK_PARALLELIZATION
  "Disable parallel link operations"
  ${WN_MSVC_DISABLE_LINK_PARALLELIZATION}
)

if(WN_MSVC_DISABLE_LINK_PARALLELIZATION)
  message(STATUS "Disable link parallelization")
  set_property(GLOBAL APPEND PROPERTY JOB_POOLS link_job_pool=1)
  set(CMAKE_JOB_POOL_LINK link_job_pool)
endif()

if(MSVC)
  # Fix for LLVM Visual Studio 2015 RC issue
  if(MSVC_VERSION GREATER 1800)
    list(APPEND WN_LLVM_EXTRA_FLAGS "-DCMAKE_CXX_FLAGS=/Zc:sizedDealloc-")
  endif()

  # Fix for stb issue
  add_compile_options(/wd4204)
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
  if(WN_USE_SCCACHE)
    string(REGEX REPLACE "/Zi" "/Z7" ${lang} "${${lang}}")
    string(REGEX REPLACE "-Zi" "-Z7" ${lang} "${${lang}}")
  endif()
endforeach()
add_compile_options(/W4) # Adjust warnings to level 4
add_compile_options(/WX) # Enable warnings as errors
add_compile_options(/GR-) # Disable run-time type information
add_compile_options(/MP) # Enable multi-processor compilation

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

# HACK remove once googletest stops using tr1 namespace
add_compile_options(/D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)

add_compile_options(/D_WN_WINDOWS)

# Linker options
foreach(type EXE SHARED STATIC)
  set(CMAKE_${type}_LINKER_FLAGS "${CMAKE_${type}_LINKER_FLAGS} /ignore:4221")

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

    # disable incremental linking
    if(WN_MSVC_DISABLE_INCREMENTAL_LINKING)
      message(STATUS "Disable incremental linking: ${type}")

      if("${CMAKE_${type}_LINKER_FLAGS_DEBUG}" MATCHES "/INCREMENTAL")
        string(REGEX REPLACE "/INCREMENTAL" "/INCREMENTAL:NO "
          CMAKE_${type}_LINKER_FLAGS_DEBUG
          "${CMAKE_${type}_LINKER_FLAGS_DEBUG}")
      elseif(NOT "${CMAKE_${type}_LINKER_FLAGS_DEBUG}" MATCHES "/INCREMENTAL:NO")
        set(CMAKE_${type}_LINKER_FLAGS_DEBUG
          "${CMAKE_${type}_LINKER_FLAGS_DEBUG} /INCREMENTAL:NO")
      endif()

      if("${CMAKE_${type}_LINKER_FLAGS_RELWITHDEBINFO}" MATCHES "/INCREMENTAL")
        string(REGEX REPLACE "/INCREMENTAL" "/INCREMENTAL:NO"
          CMAKE_${type}_LINKER_FLAGS_RELWITHDEBINFO
          "${CMAKE_${type}_LINKER_FLAGS_RELWITHDEBINFO}")
      elseif(NOT "${CMAKE_${type}_LINKER_FLAGS_RELWITHDEBINFO}" MATCHES "/INCREMENTAL:NO")
        set(CMAKE_${type}_LINKER_FLAGS_RELWITHDEBINFO
          "${CMAKE_${type}_LINKER_FLAGS_RELWITHDEBINFO} /INCREMENTAL:NO")
      endif()
    endif()
  endif()
endforeach()
