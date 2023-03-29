project(CFS_LC C)

set(APP_SRC_FILES
  fsw/src/lc_custom.c
  fsw/src/lc_app.c
  fsw/src/lc_cmds.c
  fsw/src/lc_dispatch.c
  fsw/src/lc_watch.c
  fsw/src/lc_action.c
  fsw/src/lc_utils.c
)

# Create the app module
add_cfe_app(lc ${APP_SRC_FILES})

# This permits direct access to public headers in the fsw/inc directory
target_include_directories(lc PUBLIC fsw/inc)

set(APP_TABLE_FILES
  fsw/tables/lc_def_wdt.c
  fsw/tables/lc_def_adt.c
)

add_cfe_tables(lc ${APP_TABLE_FILES})

# If UT is enabled, then add the tests from the subdirectory
# Note that this is an app, and therefore does not provide
# stub functions, as other entities would not typically make
# direct function calls into this application.
if(ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif()
