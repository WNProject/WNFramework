set(ENABLED OFF)

wn_has_architecture(ARM HAS)

if (HAS)
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} arm)
