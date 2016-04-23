overlay_library_sources(WNEntryPoint
  SOURCES
  inc/Internal/Extension/x86/3DNOW/WNBasicTraits.h)

overlay_set_variable(WN_MEMORY_EXTENSION_FILE
  "WNMemory/inc/Internal/Extension/x86/3DNOW/WNBasicTraits.h")

overlay_set_variable(WN_MEMORY_TRAITS_NAME
  "basic_traits_3dnow")
