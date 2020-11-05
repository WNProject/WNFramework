enable_overlay_directory()

add_wn_application(
  vct
  SOURCES
    src/main.cpp
    src/object.h
    src/scene_demo.cpp
    src/scene_demo.h
    src/scene_file.h
    src/scene_object.h
    src/texture_file.h
    src/texture_manager.h
    src/texture.h
  LIBS
    WNGraphics
	WNFileSystem
)

post_enable_overlay_directory()