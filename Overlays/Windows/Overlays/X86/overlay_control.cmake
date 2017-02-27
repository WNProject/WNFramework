set(ENABLED OFF)

wn_has_architecture(x86 HAS)
wn_has_architecture(x86-64 HAS64)

if (HAS OR HAS64)
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} X86)
