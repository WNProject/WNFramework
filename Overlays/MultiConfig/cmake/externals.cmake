# Sets up the visual studio folder structure for externals.
foreach (EXTERNAL_TARGET ${EXTERNAL_TARGETS})
    get_property(OLD_DIRECTORY TARGET ${EXTERNAL_TARGET} PROPERTY FOLDER)
    set_property(
        TARGET ${EXTERNAL_TARGET}
        PROPERTY FOLDER Externals/${name}/${OLD_DIRECTORY}
    )
endforeach()
