set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# To be called from a pre_add_* file
macro(set_disabled_source_groups)
  if (${name}_OVERLAY_DISABLED_LOGICAL_SOURCES)
    LIST(LENGTH ${name}_OVERLAY_DISABLED_LOGICAL_SOURCES num_local_existing_sources)
    math(EXPR num_local_existing_sources "${num_local_existing_sources} - 1")
    foreach(index RANGE 0 ${num_local_existing_sources})
      list(GET ${name}_OVERLAY_DISABLED_SOURCES ${index} existing_full_source)
      list(GET ${name}_OVERLAY_DISABLED_LOGICAL_SOURCES ${index} existing_logical_source)
      list(GET ${name}_OVERLAY_DISABLED_ROOTED_SOURCES ${index} existing_rooted_overlay)

      get_filename_component(dir_name ${existing_rooted_overlay} DIRECTORY)

      if (IS_ABSOLUTE ${existing_logical_source})
        get_filename_component(binary_abs ${CMAKE_CURRENT_BINARY_DIR} ABSOLUTE)
        if (${existing_logical_source} MATCHES ${binary_abs}  )
          file(RELATIVE_PATH relative_offset
            ${CMAKE_CURRENT_BINARY_DIR}
            ${existing_logical_source})
          get_filename_component(
              relative_offset
              ${relative_offset}
              DIRECTORY)

          string(REPLACE "/" "\\" relative_offset ${relative_offset})
          source_group(
            "_Disabled\\Generated\\${relative_offset}"
            FILES
            "${existing_full_source}")
        else()
          source_group("_Disabled\\Generated" FILES "${existing_full_source}")
        endif()
        continue()
      endif()

      set(overlay_group "_Disabled\\")
      if (${existing_rooted_overlay} STREQUAL ".")
        get_filename_component(dir_name ${existing_logical_source} DIRECTORY)
        set(overlay_group "")
      endif()
      string(REPLACE "/" "\\" dir_name ${dir_name})
      source_group("${overlay_group}${dir_name}" FILES "${existing_full_source}")
      set_source_files_properties(${existing_full_source}
        PROPERTIES HEADER_FILE_ONLY true)
    endforeach()
   endif()
endmacro()

# To be called from a pre_add_* file
macro(set_enabled_source_groups)
  if (${name}_OVERLAY_LOGICAL_SOURCES)
    LIST(LENGTH ${name}_OVERLAY_LOGICAL_SOURCES num_local_existing_sources)
    math(EXPR num_local_existing_sources "${num_local_existing_sources} - 1")
  else()
    set(num_local_existing_sources -1)
  endif()

  foreach(index RANGE 0 ${num_local_existing_sources})
    list(GET ${name}_OVERLAY_SOURCES ${index} existing_full_source)
    list(GET ${name}_OVERLAY_LOGICAL_SOURCES ${index} existing_logical_source)
    list(GET ${name}_OVERLAY_ROOTED_SOURCES ${index} existing_rooted_overlay)

    get_filename_component(dir_name ${existing_rooted_overlay} DIRECTORY)

    if (IS_ABSOLUTE ${existing_logical_source})
      get_filename_component(binary_abs ${CMAKE_CURRENT_BINARY_DIR} ABSOLUTE)
      if (${existing_logical_source} MATCHES ${binary_abs}  )
        file(RELATIVE_PATH relative_offset
          ${CMAKE_CURRENT_BINARY_DIR}
          ${existing_logical_source})
        get_filename_component(
            relative_offset
            ${relative_offset}
            DIRECTORY)

        string(REPLACE "/" "\\" relative_offset ${relative_offset})
        source_group(
          "_Generated\\${relative_offset}"
          FILES
          "${existing_full_source}")
      else()
        source_group("_Generated" FILES "${existing_full_source}")
      endif()
      continue()
    endif()

    set(overlay_group "_Overlays\\")

    if (${existing_rooted_overlay} STREQUAL ".")
      list(GET ${name}_OVERLAY_LOGICAL_SOURCES ${index} dir_name)
      get_filename_component(dir_name ${dir_name} DIRECTORY)
      set(overlay_group "")
    endif()

    string(REPLACE "/" "\\" dir_name ${dir_name})
    source_group("${overlay_group}${dir_name}" FILES "${existing_full_source}")
  endforeach()
endmacro()

macro(set_source_groups)
  set_enabled_source_groups()
  set_disabled_source_groups()
endmacro()
