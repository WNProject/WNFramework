include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

set(WN_CDECL_BEGIN)
set(WN_CDECL_END)

enable_overlay_directory()

# run specifier tests

wn_check_cxx_source_compiles(
  "
    __forceinline void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___FORCE_INLINE
)

wn_check_cxx_source_compiles(
  "
    inline __attribute__((always_inline)) void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___ATTRIBUTE__ALWAYS_INLINE
)

wn_check_cxx_source_compiles(
  "
    __declspec(noinline) void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___DECLSPEC_NOINLINE
)

wn_check_cxx_source_compiles(
  "
    __attribute__((noinline)) void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___ATTRIBUTE__NOINLINE
)

wn_check_cxx_source_compiles(
  "
    __declspec(selectany) int temp = 0;
    int main() {
      return temp;
    }
  "
  HAS___DECLSPEC_SELECTANY
)

wn_check_cxx_source_compiles(
  "
    __attribute__((weak)) int temp = 0;
    int main() {
      return temp;
    }
  "
  HAS___ATTRIBUTE__WEAK
)


# make specifier selections

set(WN_FORCE_INLINE)
set(WN_NO_INLINE)
set(WN_WEAK_LINK)

if (HAS___ATTRIBUTE__ALWAYS_INLINE)
  set(WN_FORCE_INLINE "inline __attribute__((always_inline))")
elseif (HAS___FORCE_INLINE)
  set(WN_FORCE_INLINE __forceinline)
else()
  set(WN_FORCE_INLINE inline)
endif()

if (HAS___ATTRIBUTE__NOINLINE)
  set(WN_NO_INLINE "__attribute__((noinline))")
elseif (HAS___DECLSPEC_NOINLINE)
  set(WN_NO_INLINE "__declspec(noinline)")
endif()

if (HAS___ATTRIBUTE__WEAK)
  set(WN_WEAK_LINK "__attribute__((weak))")
elseif (HAS___DECLSPEC_SELECTANY)
  set(WN_WEAK_LINK "__declspec(selectany)")
elseif(NOT WN_DISABLE_CXX)
  message(FATAL_ERROR "No weak link style keyword was found")
endif()

# print selections

message(STATUS "WN_FORCE_INLINE Selection: ${WN_FORCE_INLINE}")

if (WN_NO_INLINE)
  message(STATUS "WN_NO_INLINE Selection: ${WN_NO_INLINE}")
else()
  message(STATUS "WN_NO_INLINE Selection: [none]")
endif()

message(STATUS "WN_WEAK_LINK Selection: ${WN_WEAK_LINK}")

if (WN_CDECL_BEGIN)
  message(STATUS "WN_CDECL_BEGIN Selection: ${WN_CDECL_BEGIN}")
else()
  message(STATUS "WN_CDECL_BEGIN Selection: [none]")
endif()

if (WN_CDECL_END)
  message(STATUS "WN_CDECL_END Selection: ${WN_CDECL_END}")
else()
  message(STATUS "WN_CDECL_END Selection: [none]")
endif()

if (WN_STDCALL_BEGIN)
  message(STATUS "WN_STDCALL_BEGIN Selection: ${WN_STDCALL_BEGIN}")
else()
  message(STATUS "WN_STDCALL_BEGIN Selection: [none]")
endif()

if (WN_STDCALL_END)
  message(STATUS "WN_STDCALL_END Selection: ${WN_STDCALL_END}")
else()
  message(STATUS "WN_STDCALL_END Selection: [none]")
endif()

if (WN_FASTCALL_BEGIN)
  message(STATUS "WN_FASTCALL_BEGIN Selection: ${WN_FASTCALL_BEGIN}")
else()
  message(STATUS "WN_FASTCALL_BEGIN Selection: [none]")
endif()

if (WN_FASTCALL_END)
  message(STATUS "WN_FASTCALL_END Selection: ${WN_FASTCALL_END}")
else()
  message(STATUS "WN_FASTCALL_END Selection: [none]")
endif()

if (WN_VECTORCALL_BEGIN)
  message(STATUS "WN_VECTORCALL_BEGIN Selection: ${WN_VECTORCALL_BEGIN}")
else()
  message(STATUS "WN_VECTORCALL_BEGIN Selection: [none]")
endif()

if (WN_VECTORCALL_END)
  message(STATUS "WN_VECTORCALL_END Selection: ${WN_VECTORCALL_END}")
else()
  message(STATUS "WN_VECTORCALL_END Selection: [none]")
endif()

# run include tests

wn_check_include_file_cxx(cstdint HAS_CSTDINT)

if (NOT HAS_CSTDINT AND NOT WN_DISABLE_CXX)
  message(FATAL_ERROR "Unable to locate required integer types header")
endif()

# run stl feature tests

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      const bool b = std::is_trivially_copyable<int>::value;
      return b ? 1 : 0;
    }
  "
  HAS_CPP11_STD_IS_TRIVIALLY_COPYABLE
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      const bool b = std::is_trivially_constructible<int, short>::value;
      return b ? 1 : 0;
    }
  "
  HAS_CPP11_STD_IS_TRIVIALLY_CONSTRUCTIBLE
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      const bool b = std::is_trivially_default_constructible<int>::value;
      return b ? 1 : 0;
    }
  "
  HAS_CPP11_STD_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      const bool b = std::is_trivially_copy_constructible<int>::value;
      return b ? 1 : 0;
    }
  "
  HAS_CPP11_STD_IS_TRIVIALLY_COPY_CONSTRUCTIBLE
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      const bool b = std::is_trivially_move_constructible<int>::value;
      return b ? 1 : 0;
    }
  "
  HAS_CPP11_STD_IS_TRIVIALLY_MOVE_CONSTRUCTIBLE
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      const bool b = std::is_trivially_assignable<int, short>::value;
      return b ? 1 : 0;
    }
  "
  HAS_CPP11_STD_IS_TRIVIALLY_ASSIGNABLE
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      const bool b = std::is_trivially_copy_assignable<int>::value;
      return b ? 1 : 0;
    }
  "
  HAS_CPP11_STD_IS_TRIVIALLY_COPY_ASSIGNABLE
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      const bool b = std::is_trivially_move_assignable<int>::value;
      return b ? 1 : 0;
    }
  "
  HAS_CPP11_STD_IS_TRIVIALLY_MOVE_ASSIGNABLE
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      const bool b = std::is_trivially_destructible<int>::value;
      return b ? 1 : 0;
    }
  "
  HAS_CPP11_STD_IS_TRIVIALLY_DESTRUCTIBLE
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      std::aligned_union<sizeof(int), int, short>::type s;
      return 1;
    }
  "
  HAS_CPP11_STD_ALIGNED_UNION
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      std::decay_t<const int> temp(0);
      return temp;
    }
  "
  HAS_CPP14_STD_DECAY_T
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    struct tester {
      tester() = default;
      template <typename T>
      std::enable_if_t<!std::is_same<T, int>::value, int>
      test(T&& _value) const {
        static_assert(!std::is_same<T, int>::value, \"fail\");
      }
      template <typename T>
      std::enable_if_t<std::is_same<T, int>::value, int>
      test(T&& _value) const {
        return _value;
      }
    };
    int main() {
      return tester().test(int(1));
    }
  "
  HAS_CPP14_STD_ENABLE_IF_T
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      const std::conditional_t<true, int, double> temp(0);
      return static_cast<int>(temp);
    }
  "
  HAS_CPP14_STD_CONDITIONAL_T
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      const std::common_type_t<int, double> temp(0.0);
      return static_cast<int>(temp);
    }
  "
  HAS_CPP14_STD_COMMON_TYPE_T
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int test() {
      return 0;
    }
    int main() {
      std::result_of_t<decltype(&test)()> temp(0);
      return temp;
    }
  "
  HAS_CPP14_STD_RESULT_OF_T
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      return static_cast<int>(std::integer_sequence<int, 1, 2, 3>::size());
    }
  "
  HAS_CPP14_STD_INTEGER_SEQUENCE
)

wn_check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      return std::is_null_pointer<decltype(nullptr)>::value ? 0 : 1;
    }
  "
  HAS_CPP14_STD_IS_NULL_POINTER
)

set(_WN_HAS_CPP11_STD_IS_TRIVIALLY_COPYABLE
  ${HAS_CPP11_STD_IS_TRIVIALLY_COPYABLE})
set(_WN_HAS_CPP11_STD_IS_TRIVIALLY_CONSTRUCTIBLE
  ${HAS_CPP11_STD_IS_TRIVIALLY_CONSTRUCTIBLE})
set(_WN_HAS_CPP11_STD_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE
  ${HAS_CPP11_STD_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE})
set(_WN_HAS_CPP11_STD_IS_TRIVIALLY_COPY_CONSTRUCTIBLE
  ${HAS_CPP11_STD_IS_TRIVIALLY_COPY_CONSTRUCTIBLE})
set(_WN_HAS_CPP11_STD_IS_TRIVIALLY_MOVE_CONSTRUCTIBLE
  ${HAS_CPP11_STD_IS_TRIVIALLY_MOVE_CONSTRUCTIBLE})
set(_WN_HAS_CPP11_STD_IS_TRIVIALLY_ASSIGNABLE
  ${HAS_CPP11_STD_IS_TRIVIALLY_ASSIGNABLE})
set(_WN_HAS_CPP11_STD_IS_TRIVIALLY_COPY_ASSIGNABLE
  ${HAS_CPP11_STD_IS_TRIVIALLY_COPY_ASSIGNABLE})
set(_WN_HAS_CPP11_STD_IS_TRIVIALLY_MOVE_ASSIGNABLE
  ${HAS_CPP11_STD_IS_TRIVIALLY_MOVE_ASSIGNABLE})
set(_WN_HAS_CPP11_STD_IS_TRIVIALLY_DESTRUCTIBLE
  ${HAS_CPP11_STD_IS_TRIVIALLY_DESTRUCTIBLE})
set(_WN_HAS_CPP11_STD_ALIGNED_UNION ${HAS_CPP11_STD_ALIGNED_UNION})
set(_WN_HAS_CPP14_STD_DECAY_T ${HAS_CPP14_STD_DECAY_T})
set(_WN_HAS_CPP14_STD_ENABLE_IF_T ${HAS_CPP14_STD_ENABLE_IF_T})
set(_WN_HAS_CPP14_STD_CONDITIONAL_T ${HAS_CPP14_STD_CONDITIONAL_T})
set(_WN_HAS_CPP14_STD_COMMON_TYPE_T ${HAS_CPP14_STD_COMMON_TYPE_T})
set(_WN_HAS_CPP14_STD_RESULT_OF_T ${HAS_CPP14_STD_RESULT_OF_T})
set(_WN_HAS_CPP14_STD_INTEGER_SEQUENCE ${HAS_CPP14_STD_INTEGER_SEQUENCE})
set(_WN_HAS_CPP14_STD_IS_NULL_POINTER ${HAS_CPP14_STD_IS_NULL_POINTER})

wn_make_preprocessor_symbol(
  _WN_${WN_ARCHITECTURE_GROUP}
  _WN_ARCHITECTURE
)

wn_make_preprocessor_symbol(
  _WN_${WN_ARCHITECTURE_BITNESS}_BIT
  _WN_ARCHITECTURE_BITNESS
)
wn_make_preprocessor_symbol(
  _WN_ENDIAN_${WN_ARCHITECTURE_ENDIANNESS}
  _WN_ARCHITECTURE_ENDIANNESS
)

wn_make_preprocessor_symbol(
  _WN_${WN_PLATFORM}
  _WN_PLATFORM
)

# generate config file
set(CONFIG_FILE_INPUT_PATH_RELATIVE inc/Internal/WNConfig.h.in)
set(CONFIG_FILE_OUTPUT_PATH_RELATIVE inc/Internal/WNConfig.h)

configure_file(
  ${CONFIG_FILE_INPUT_PATH_RELATIVE}
  ${CONFIG_FILE_OUTPUT_PATH_RELATIVE}
)

set(CONFIG_FILE_INPUT_PATH ${CONFIG_FILE_INPUT_PATH_RELATIVE})

set(CONFIG_FILE_OUTPUT_PATH
  ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_OUTPUT_PATH_RELATIVE}
)

# setup target
add_wn_header_library(WNCore
  SOURCES
    inc/Internal/WNConfig.h.in
    inc/Internal/WNErrors.inc
    ${CONFIG_FILE_INPUT_PATH}
    ${CONFIG_FILE_OUTPUT_PATH}
    inc/WNAssert.h
    inc/WNBase.h
    inc/WNEndian.h
    inc/WNMacros.h
    inc/WNPair.h
    inc/WNTuple.h
    inc/WNTypes.h
    inc/WNTypeTraits.h
    inc/WNUtility.h
)

wn_add_subdirectory(test)

post_enable_overlay_directory()