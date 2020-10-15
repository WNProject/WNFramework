# Sets up the visual studio folder structure for externals.
foreach (TARGET ${EXTERNAL_TARGETS})
    get_property(OLD_DIRECTORY TARGET ${TARGET} PROPERTY FOLDER)
    set_property(TARGET ${TARGET} PROPERTY FOLDER Externals/${name}/${OLD_DIRECTORY})
endforeach()
