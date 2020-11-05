enable_overlay_directory()

add_wn_library(
  stb
  INCLUDES
    ${WNFramework_SOURCE_DIR}/externals/stb
  SOURCES
    src/stb_image.c
    src/stb_truetype.c
)

post_enable_overlay_directory()
