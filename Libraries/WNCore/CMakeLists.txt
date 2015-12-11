include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

# run keyword tests
check_cxx_source_compiles(
  "
    _inline void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS_INLINE
)

check_cxx_source_compiles(
  "
    __inline void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS__INLINE
)

check_cxx_source_compiles(
  "
    __inline__ void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS__INLINE__
)

check_cxx_source_compiles(
  "
    __forceinline void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS__FORCE_INLINE
)

check_cxx_source_compiles(
  "
    __attribute__((always_inline)) void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS__ATTRIBUTE__ALWAYS_INLINE
)

check_cxx_source_compiles(
  "
    __declspec(noinline) void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS__DECLSPEC_NOINLINE
)

check_cxx_source_compiles(
  "
    __attribute__((noinline)) void temp() {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS__ATTRIBUTE__NOINLINE
)

check_cxx_source_compiles(
  "
    __thread int temp = 0;
    int main() {
      return temp;
    }
  "
  HAS__THREAD
)

check_cxx_source_compiles(
  "
    __declspec(thread) int temp = 0;
    int main() {
      return temp;
    }
  "
  HAS__DECLSPEC_THREAD
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
  HAS__DECLSPEC_SELECTANY
)

check_cxx_source_compiles(
  "
    __attribute__((weak)) int temp = 0;
    int main() {
      return temp;
    }
  "
  HAS__ATTRIBUTE__WEAK
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
  HAS__RESTRICT
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
  HAS__RESTRICT__
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

# make keyword selections

set(WN_INLINE)
set(WN_FORCE_INLINE)
set(WN_NO_INLINE)
set(WN_THREAD_LOCAL)
set(WN_WEAK_LINK)
set(WN_RESTRICT)
set(WN_FINAL)
set(WN_OVERRIDE)

if (HAS__INLINE__)
  set(WN_INLINE __inline__)
elseif (HAS__INLINE)
  set(WN_INLINE __inline)
elseif (HAS_INLINE)
  set(WN_INLINE _inline)
else()
  set(WN_INLINE inline)
endif()

if (HAS__FORCE_INLINE)
  set(WN_FORCE_INLINE __forceinline)
elseif (HAS__ATTRIBUTE__ALWAYS_INLINE)
  set(WN_FORCE_INLINE "${WN_INLINE} __attribute__((always_inline))")
else()
  set(WN_FORCE_INLINE ${WN_INLINE})
endif()

if (HAS__DECLSPEC_NOINLINE)
  set(WN_NO_INLINE "__declspec(noinline)")
elseif (HAS__ATTRIBUTE__NOINLINE)
  set(WN_NO_INLINE "__attribute__((noinline))")
endif()

if (HAS__DECLSPEC_THREAD)
  set(WN_THREAD_LOCAL "__declspec(thread)")
elseif (HAS__THREAD)
  set(WN_THREAD_LOCAL "__thread")
elseif(HAS_THREAD_LOCAL)
  set(WN_THREAD_LOCAL "thread_local")
else()
  message(FATAL_ERROR "No thread local style keyword was found")
endif()

if (HAS__DECLSPEC_SELECTANY)
  set(WN_WEAK_LINK "__declspec(selectany)")
elseif (HAS__ATTRIBUTE__WEAK)
  set(WN_WEAK_LINK "__attribute__((weak))")
else()
  message(FATAL_ERROR "No weak link style keyword was found")
endif()

if (HAS__RESTRICT)
  set(WN_RESTRICT "__restrict")
elseif (HAS__RESTRICT__)
  set(WN_RESTRICT "__restrict__")
endif()

if (HAS_FINAL)
  set(WN_FINAL final)
endif()

if (HAS_OVERRIDE)
  set(WN_OVERRIDE override)
endif()

# run specifier tests

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
  HAS__DECLSPEC_ALIGN
)

check_cxx_source_compiles(
  "
    int main() {
      __attribute__((aligned(16))) int i = 0;
      return i;
    }
  "
  HAS__ATTRIBUTE__ALIGNED
)

# make specifier selections

set(WN_ALIGN)

if (HAS_ALIGNAS)
  set(WN_ALIGN "alignas(_x)")
elseif (HAS__DECLSPEC_ALIGN)
  set(WN_ALIGN "__declspec(align(_x))")
elseif (HAS__ATTRIBUTE__ALIGNED)
  set(WN_ALIGN "__attribute__((aligned(_x)))")
else()
  message(FATAL_ERROR "No align style keyword was found")
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

configure_file(inc/Internal/WNConfig.h.in inc/Internal/WNConfig.h)

set(INC_INTERNAL
  inc/Internal/WNBase_Clang.h
  inc/Internal/WNBase_Clang_GCC.h
  inc/Internal/WNBase_GCC.h
  inc/Internal/WNBase_MSVC.h
  inc/Internal/WNConfigOld.h
  inc/Internal/WNConfig.h.in
  inc/Internal/WNErrors.inc
  inc/Internal/WNExtendedTypes.h
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