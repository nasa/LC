###########################################################
#
# LC App mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# Add stand alone documentation
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/docs/dox_src ${MISSION_BINARY_DIR}/docs/lc-usersguide)

# The list of header files that control the HK configuration
set(LC_MISSION_CONFIG_FILE_LIST
  lc_fcncodes.h
  lc_interface_cfg.h
  lc_mission_cfg.h
  lc_msgstruct.h
  lc_msgdefs.h
  lc_msg.h
  lc_tblstruct.h
  lc_tbldefs.h
  lc_tbl.h
)

if (CFE_EDS_ENABLED_BUILD)

  # In an EDS-based build, these files come generated from the EDS tool
  set(LC_CFGFILE_SRC_lc_mission_cfg "lc_eds_designparameters.h")
  set(LC_CFGFILE_SRC_lc_tbldefs "lc_eds_typedefs.h")
  set(LC_CFGFILE_SRC_lc_msgdefs "lc_eds_cc.h")
  set(LC_CFGFILE_SRC_lc_msg "lc_eds_typedefs.h")
  set(LC_CFGFILE_SRC_lc_tbl "lc_eds_typedefs.h")

else(CFE_EDS_ENABLED_BUILD)

  # In a non-EDS build, the user supplies these files, plus a couple more
  # There are defaults for all in the "config" dir
  list (APPEND LC_MISSION_CONFIG_FILE_LIST
    lc_msgstruct.h
    lc_tblstruct.h
  )

endif(CFE_EDS_ENABLED_BUILD)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(LC_CFGFILE ${LC_MISSION_CONFIG_FILE_LIST})
  get_filename_component(CFGKEY "${LC_CFGFILE}" NAME_WE)
  if (DEFINED LC_CFGFILE_SRC_${CFGKEY})
    set(DEFAULT_SOURCE GENERATED_FILE "${LC_CFGFILE_SRC_${CFGKEY}}")
  else()
    set(DEFAULT_SOURCE FALLBACK_FILE "${CMAKE_CURRENT_LIST_DIR}/config/default_${LC_CFGFILE}")
  endif()

  generate_config_includefile(
    FILE_NAME           "${LC_CFGFILE}"
    ${DEFAULT_SOURCE}
  )
endforeach()
