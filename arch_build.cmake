###########################################################
#
# LC App platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the app configuration
set(LC_PLATFORM_CONFIG_FILE_LIST
  lc_internal_cfg_values.h
  lc_platform_cfg.h
  lc_msgid_values.h
  lc_msgids.h
)

generate_configfile_set(${LC_PLATFORM_CONFIG_FILE_LIST})