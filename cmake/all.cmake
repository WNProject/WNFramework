#Add overlays should be first.
#The rest are order independent.

include(${WNFramework_SOURCE_DIR}/cmake/determine_architecture.cmake)
include(${WNFramework_SOURCE_DIR}/cmake/find_properties.cmake)
include(${WNFramework_SOURCE_DIR}/cmake/add_overlays.cmake)

include(${WNFramework_SOURCE_DIR}/cmake/build_options.cmake)
include(${WNFramework_SOURCE_DIR}/cmake/externals.cmake)
include(${WNFramework_SOURCE_DIR}/cmake/post_build_options.cmake)
include(${WNFramework_SOURCE_DIR}/cmake/make_preprocessor_symbol.cmake)
include(${WNFramework_SOURCE_DIR}/cmake/targets.cmake)
include(${WNFramework_SOURCE_DIR}/cmake/tests.cmake)
include(${WNFramework_SOURCE_DIR}/cmake/utilities.cmake)