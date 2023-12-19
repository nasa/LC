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
  lc_internal_cfg.h
  lc_msgids.h
  lc_platform_cfg.h
)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(LC_CFGFILE ${LC_PLATFORM_CONFIG_FILE_LIST})
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
