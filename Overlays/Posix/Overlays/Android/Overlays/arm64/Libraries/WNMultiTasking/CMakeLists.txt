overlay_header_library_sources(WNMultiTasking
  SOURCES
      asm/Android/WNGetContext_arm64.S
      asm/Android/WNSetContext_arm64.S
      src/Android/WNContext_arm64.h
      src/Android/WNMakeContext_arm64.cpp
)