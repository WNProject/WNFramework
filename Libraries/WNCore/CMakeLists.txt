include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

# run specifier tests

check_cxx_source_compiles(
  "
    inline void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS_INLINE
)

check_cxx_source_compiles(
  "
    _inline void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS__INLINE
)

check_cxx_source_compiles(
  "
    __inline void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___INLINE
)

check_cxx_source_compiles(
  "
    __inline__ void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___INLINE__
)

check_cxx_source_compiles(
  "
    __forceinline void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___FORCE_INLINE
)

check_cxx_source_compiles(
  "
    __attribute__((always_inline)) void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___ATTRIBUTE__ALWAYS_INLINE
)

check_cxx_source_compiles(
  "
    __declspec(noinline) void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___DECLSPEC_NOINLINE
)

check_cxx_source_compiles(
  "
    __attribute__((noinline)) void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___ATTRIBUTE__NOINLINE
)

check_cxx_source_compiles(
  "
    __thread int temp = 0;
    int main() {
      return temp;
    }
  "
  HAS___THREAD
)

check_cxx_source_compiles(
  "
    __declspec(thread) int temp = 0;
    int main() {
      return temp;
    }
  "
  HAS___DECLSPEC_THREAD
)

check_cxx_source_compiles(
  "
    thread_local int temp = 0;
    int main() {
      return temp;
    }
  "
  HAS_THREAD_LOCAL
)

check_cxx_source_compiles(
  "
    __declspec(selectany) int temp = 0;
    int main() {
      return temp;
    }
  "
  HAS___DECLSPEC_SELECTANY
)

check_cxx_source_compiles(
  "
    __attribute__((weak)) int temp = 0;
    int main() {
      return temp;
    }
  "
  HAS___ATTRIBUTE__WEAK
)

check_cxx_source_compiles(
  "
    int temp(int* __restrict a, int* __restrict b) {
      return (*a) + (*b);
    }
    int main() {
      int a = 0;
      int b = 1;
      return temp(&a, &b);
    }
  "
  HAS___RESTRICT
)

check_cxx_source_compiles(
  "
    int temp(int* __restrict__ a, int* __restrict__ b) {
      return (*a) + (*b);
    }
    int main() {
      int a = 0;
      int b = 1;
      return temp(&a, &b);
    }
  "
  HAS___RESTRICT__
)

check_cxx_source_compiles(
  "
    struct temp final {};
    int main() {
      temp tmp;
      return 0;
    }
  "
  HAS_FINAL
)

check_cxx_source_compiles(
  "
    struct temp {
      virtual int test() = 0;
    };
    struct temp2 : temp {
      virtual int test() override {
        return 0;
      }
    };
    int main() {
      temp2* tmp = new temp2();
      return tmp->test();
    }
  "
  HAS_OVERRIDE
)

check_cxx_source_compiles(
  "
    int main() {
      alignas(16) int i = 0;
      return i;
    }
  "
  HAS_ALIGNAS
)

check_cxx_source_compiles(
  "
    int main() {
      __declspec(align(16)) int i = 0;
      return i;
    }
  "
  HAS___DECLSPEC_ALIGN
)

check_cxx_source_compiles(
  "
    int main() {
      __attribute__((aligned(16))) int i = 0;
      return i;
    }
  "
  HAS___ATTRIBUTE__ALIGNED
)

# make specifier selections

set(WN_INLINE)
set(WN_FORCE_INLINE)
set(WN_NO_INLINE)
set(WN_THREAD_LOCAL)
set(WN_WEAK_LINK)
set(WN_RESTRICT)
set(WN_FINAL)
set(WN_OVERRIDE)
set(WN_ALIGN)

if (HAS_INLINE)
  set(WN_INLINE inline)
elseif (HAS___INLINE__)
  set(WN_INLINE __inline__)
elseif (HAS___INLINE)
  set(WN_INLINE __inline)
elseif (HAS__INLINE)
  set(WN_INLINE _inline)
else()
  message(FATAL_ERROR "No inline style keyword was found")
endif()

if (HAS___ATTRIBUTE__ALWAYS_INLINE)
  set(WN_FORCE_INLINE "${WN_INLINE} __attribute__((always_inline))")
elseif (HAS___FORCE_INLINE)
  set(WN_FORCE_INLINE __forceinline)
else()
  set(WN_FORCE_INLINE ${WN_INLINE})
endif()

if (HAS___ATTRIBUTE__NOINLINE)
  set(WN_NO_INLINE "__attribute__((noinline))")
elseif (HAS___DECLSPEC_NOINLINE)
  set(WN_NO_INLINE "__declspec(noinline)")
endif()

if(HAS_THREAD_LOCAL)
  set(WN_THREAD_LOCAL thread_local)
elseif (HAS___DECLSPEC_THREAD)
  set(WN_THREAD_LOCAL "__declspec(thread)")
elseif (HAS___THREAD)
  set(WN_THREAD_LOCAL __thread)
else()
  message(FATAL_ERROR "No thread local style keyword was found")
endif()

if (HAS___ATTRIBUTE__WEAK)
  set(WN_WEAK_LINK "__attribute__((weak))")
elseif (HAS___DECLSPEC_SELECTANY)
  set(WN_WEAK_LINK "__declspec(selectany)")
else()
  message(FATAL_ERROR "No weak link style keyword was found")
endif()

if (HAS___RESTRICT)
  set(WN_RESTRICT __restrict)
elseif (HAS___RESTRICT__)
  set(WN_RESTRICT __restrict__)
endif()

if (HAS_FINAL)
  set(WN_FINAL final)
endif()

if (HAS_OVERRIDE)
  set(WN_OVERRIDE override)
endif()

set(WN_ALIGN_MACRO_VARIABLE_NAME _alignment)

if (HAS_ALIGNAS)
  set(WN_ALIGN "alignas(${WN_ALIGN_MACRO_VARIABLE_NAME})")
elseif (HAS___DECLSPEC_ALIGN)
  set(WN_ALIGN "__declspec(align(${WN_ALIGN_MACRO_VARIABLE_NAME}))")
elseif (HAS___ATTRIBUTE__ALIGNED)
  set(WN_ALIGN "__attribute__((aligned(${WN_ALIGN_MACRO_VARIABLE_NAME})))")
else()
  message(FATAL_ERROR "No align style keyword was found")
endif()

# run calling convention tests

check_cxx_source_compiles(
  "
    void __cdecl temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___CDECL
)

check_cxx_source_compiles(
  "
    void temp() __attribute__((cdecl)) {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___ATTRIBUTE__CDECL
)

check_cxx_source_compiles(
  "
    void __stdcall temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___STDCALL
)

check_cxx_source_compiles(
  "
    void temp() __attribute__((stdcall)) {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___ATTRIBUTE__STDCALL
)

check_cxx_source_compiles(
  "
    void __fastcall temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___FASTCALL
)

check_cxx_source_compiles(
  "
    void temp() __attribute__((fastcall)) {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___ATTRIBUTE__FASTCALL
)

check_cxx_source_compiles(
  "
    void __vectorcall temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___VECTORCALL
)

# make calling convention selections

set(WN_CDECL_BEGIN)
set(WN_CDECL_END)
set(WN_STDCALL_BEGIN)
set(WN_STDCALL_END)
set(WN_FASTCALL_BEGIN)
set(WN_FASTCALL_END)
set(WN_VECTORCALL_BEGIN)
set(WN_VECTORCALL_END)

if (HAS___ATTRIBUTE__CDECL)
  set(WN_CDECL_END "__attribute__((cdecl))")
elseif (HAS___CDECL)
  set(WN_CDECL_BEGIN __cdecl)
endif()

if (HAS___ATTRIBUTE__STDCALL)
  set(WN_STDCALL_END "__attribute__((stdcall))")
elseif (HAS___STDCALL)
  set(WN_STDCALL_BEGIN __stdcall)
endif()

if (HAS___ATTRIBUTE__FASTCALL)
  set(WN_FASTCALL_END "__attribute__((fastcall))")
elseif (HAS___FASTCALL)
  set(WN_FASTCALL_BEGIN __fastcall)
endif()

if (HAS___VECTORCALL)
  set(WN_VECTORCALL_BEGIN __vectorcall)
endif()

# print selections

message(STATUS "WN_INLINE Selection: ${WN_INLINE}")
message(STATUS "WN_FORCE_INLINE Selection: ${WN_FORCE_INLINE}")

if (WN_NO_INLINE)
  message(STATUS "WN_NO_INLINE Selection: ${WN_NO_INLINE}")
else()
  message(STATUS "WN_NO_INLINE Selection: [none]")
endif()

message(STATUS "WN_THREAD_LOCAL Selection: ${WN_THREAD_LOCAL}")
message(STATUS "WN_WEAK_LINK Selection: ${WN_WEAK_LINK}")

if (WN_RESTRICT)
  message(STATUS "WN_RESTRICT Selection: ${WN_RESTRICT}")
else()
  message(STATUS "WN_RESTRICT Selection: [none]")
endif()

if (WN_FINAL)
  message(STATUS "WN_FINAL Selection: ${WN_FINAL}")
else()
  message(STATUS "WN_FINAL Selection: [none]")
endif()

if (WN_OVERRIDE)
  message(STATUS "WN_OVERRIDE Selection: ${WN_OVERRIDE}")
else()
  message(STATUS "WN_OVERRIDE Selection: [none]")
endif()

message(STATUS "WN_ALIGN Selection: ${WN_ALIGN}")

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

check_include_file_cxx(cstdint HAS_CSTDINT)

if (NOT HAS_CSTDINT)
  message(FATAL_ERROR "Unable to locate required integer types header")
endif()

# run stl feature tests

check_cxx_source_compiles(
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

check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      std::decay_t<const int> temp(0);

      return temp;
    }
  "
  HAS_CPP14_STD_DECAY_T
)

check_cxx_source_compiles(
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

check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      const std::common_type_t<int, double> temp(0.0);

      return static_cast<int>(temp);
    }
  "
  HAS_CPP14_STD_COMMON_TYPE_T
)

check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      return static_cast<int>(std::integer_sequence<int, 1, 2, 3>::size());
    }
  "
  HAS_CPP14_STD_INTEGER_SEQUENCE
)

check_cxx_source_compiles(
  "
    #include <type_traits>
    int main() {
      return std::is_null_pointer<decltype(nullptr)>::value ? 0 : 1;
    }
  "
  HAS_CPP14_STD_IS_NULL_POINTER
)

set(_WN_HAS_CPP14_STD_ENABLE_IF_T ${HAS_CPP14_STD_ENABLE_IF_T})
set(_WN_HAS_CPP14_STD_DECAY_T ${HAS_CPP14_STD_DECAY_T})
set(_WN_HAS_CPP14_STD_RESULT_OF_T ${HAS_CPP14_STD_RESULT_OF_T})
set(_WN_HAS_CPP14_STD_COMMON_TYPE_T ${HAS_CPP14_STD_COMMON_TYPE_T})
set(_WN_HAS_CPP14_STD_INTEGER_SEQUENCE ${HAS_CPP14_STD_INTEGER_SEQUENCE})
set(_WN_HAS_CPP14_STD_IS_NULL_POINTER ${HAS_CPP14_STD_IS_NULL_POINTER})

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
set(INC_INTERNAL
  inc/Internal/WNBase_Clang.h
  inc/Internal/WNBase_Clang_GCC.h
  inc/Internal/WNBase_GCC.h
  inc/Internal/WNBase_MSVC.h
  inc/Internal/WNConfigOld.h
  inc/Internal/WNConfig.h.in
  inc/Internal/WNErrors.inc
  inc/Internal/WNExtendedTypes.h
  ${CONFIG_FILE_INPUT_PATH}
  ${CONFIG_FILE_OUTPUT_PATH}
)

set(INC_INTERNAL_ARM
  inc/Internal/ARM/WNBase.h
  inc/Internal/ARM/WNBase_Clang.h
  inc/Internal/ARM/WNBase_GCC.h
  inc/Internal/ARM/WNBase_MSVC.h
  inc/Internal/ARM/WNConfig.h
)

set(INC_INTERNAL_X86
  inc/Internal/X86/WNBase.h
  inc/Internal/X86/WNBase_Clang.h
  inc/Internal/X86/WNBase_GCC.h
  inc/Internal/X86/WNBase_MSVC.h
  inc/Internal/X86/WNConfig.h
)

source_group("inc\\Internal" FILES ${INC_INTERNAL})
source_group("inc\\Internal\\ARM" FILES ${INC_INTERNAL_ARM})
source_group("inc\\Internal\\x86" FILES ${INC_INTERNAL_X86})

add_wn_header_library(WNCore
  inc/WNAlgorithm.h
  inc/WNAssert.h
  inc/WNBase.h
  inc/WNConfigOld.h
  inc/WNEndian.h
  inc/WNExtendedTypes.h
  inc/WNTypes.h
  inc/WNTypeTraits.h
  inc/WNUtility.h
  ${INC_INTERNAL}
  ${INC_INTERNAL_ARM}
  ${INC_INTERNAL_X86}
)

add_subdirectory(test)