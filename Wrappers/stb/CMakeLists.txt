enable_overlay_directory()

add_wn_library(
  stb
  INCLUDES
    ${WNFramework_SOURCE_DIR}/Externals/stb
  SOURCES
    stb_image.c
    stb_truetype.c
)

post_enable_overlay_directory()
