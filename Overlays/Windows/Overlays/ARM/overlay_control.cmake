set(ENABLED OFF)

wn_has_architecture(ARM HAS)
wn_has_architecture(ARM64 HAS64)

if (HAS OR HAS64)
  set(ENABLED ON)
endif()

register_overlay(${ENABLED} ARM)
