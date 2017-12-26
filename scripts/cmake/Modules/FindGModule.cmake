SET(GMODULE_FIND_REQUIRED ${GModule_FIND_REQUIRED})

find_path(GMODULE_INCLUDE_DIR glib-2.0/gmodule.h)
mark_as_advanced(GMODULE_INCLUDE_DIR)

set(GMODULE_NAMES ${GMODULE_NAMES} gmodule-2.0 libgmodule-2.0)
find_library(GMODULE_LIBRARY NAMES ${GMODULE_NAMES} )
mark_as_advanced(GMODULE_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set GMODULE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GModule DEFAULT_MSG GMODULE_LIBRARY GMODULE_INCLUDE_DIR)

IF(GMODULE_FOUND)
  SET(GModule_LIBRARIES ${GMODULE_LIBRARY})
  SET(GModule_INCLUDE_DIRS ${GMODULE_INCLUDE_DIR})
ENDIF(GMODULE_FOUND)# - Find the native sqlite3 includes and library

SET(GMODULE_FIND_REQUIRED ${GModule_FIND_REQUIRED})

find_path(GMODULE_INCLUDE_DIR glib-2.0/gmodule.h)
mark_as_advanced(GMODULE_INCLUDE_DIR)

set(GMODULE_NAMES ${GMODULE_NAMES} gmodule-2.0 libgmodule-2.0)
find_library(GMODULE_LIBRARY NAMES ${GMODULE_NAMES} )
mark_as_advanced(GMODULE_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set GMODULE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GModule DEFAULT_MSG GMODULE_LIBRARY GMODULE_INCLUDE_DIR)

IF(GMODULE_FOUND)
  SET(GModule_LIBRARIES ${GMODULE_LIBRARY})
  SET(GModule_INCLUDE_DIRS ${GMODULE_INCLUDE_DIR})
ENDIF(GMODULE_FOUND)
