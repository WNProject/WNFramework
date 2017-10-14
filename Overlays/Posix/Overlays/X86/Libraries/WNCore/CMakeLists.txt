

wn_check_cxx_source_compiles(
  "
    void temp() __attribute__((cdecl)) {}
    int main() {
      temp();
      return 0;
    }
  "
  HAS___ATTRIBUTE__CDECL
)

if (HAS___ATTRIBUTE__CDECL)
  set_overlay_var(WN_CDECL_END "__attribute__((cdecl))")
endif()


overlay_header_library_sources(WNCore
  SOURCES
    inc/Internal/X86/WNBase_GCC_Clang.h)
