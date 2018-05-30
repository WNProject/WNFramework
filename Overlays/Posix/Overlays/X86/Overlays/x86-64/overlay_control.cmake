set(ENABLED OFF)

wn_has_architecture(x86-64 HAS)

if (HAS)
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} x86-64)
