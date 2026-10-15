project(CFS_LC C)

include(CheckSymbolExists)

# Check for "isnan" and "isfinite" is present in "math.h"
set(CMAKE_REQUIRED_LIBRARIES m)
check_symbol_exists(isnan "math.h" HAVE_ISNAN CMAKE_REQUIRED_LIBRARIES)
check_symbol_exists(isfinite "math.h" HAVE_ISFINITE CMAKE_REQUIRED_LIBRARIES)

if (HAVE_ISNAN OR HAVE_ISFINITE)
  # Add math library dependency to LC App
  message(STATUS "Adding math library dependency to LC App...")
  link_libraries(m)
endif()

set(APP_SRC_FILES
  fsw/src/lc_custom.c
  fsw/src/lc_app.c
  fsw/src/lc_cmds.c
  fsw/src/lc_watch.c
  fsw/src/lc_action.c
  fsw/src/lc_utils.c
)

if (CFE_EDS_ENABLED)
  list(APPEND APP_SRC_FILES
    fsw/src/lc_eds_dispatch.c
  )
else()
  list(APPEND APP_SRC_FILES
    fsw/src/lc_dispatch.c
  )
endif()

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
