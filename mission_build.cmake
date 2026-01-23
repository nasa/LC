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
  lc_fcncode_values.h
  lc_interface_cfg_values.h
  lc_mission_cfg.h
  lc_msgstruct.h
  lc_msgdefs.h
  lc_msg.h
  lc_tblstruct.h
  lc_tbldefs.h
  lc_tbl.h
  lc_topicid_values.h
)

generate_configfile_set(${LC_MISSION_CONFIG_FILE_LIST})
