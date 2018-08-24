enable_overlay_directory()

add_wn_header_library(WNFunctional
  SOURCES
    inc/WNDefer.h
    inc/WNFunction.h
    inc/WNInvoke.h
    inc/WNUniqueFunction.h
)

wn_add_subdirectory(test)

post_enable_overlay_directory()