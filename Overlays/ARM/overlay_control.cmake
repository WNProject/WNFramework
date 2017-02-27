set(ENABLED OFF)

wn_has_architecture(ARM64 HAS64)
wn_has_architecture(ARM HAS)

if (HAS OR HAS64)
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} ARM)
