# Sets up the visual studio folder structure for gtest.
foreach (target ${PARSED_ARGS_TARGETS})
    get_property(OLD_DIRECTORY TARGET ${target} PROPERTY FOLDER)
    set_property(TARGET ${target} PROPERTY FOLDER Externals/${name}/${OLD_DIRECTORY})
endforeach()
