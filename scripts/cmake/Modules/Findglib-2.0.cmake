find_package(PkgConfig)
pkg_check_modules(PC_GLIB QUIET glib-2.0)
set(GLIB_DEFINITIONS ${PC_GLIB_CFLAGS_OTHER})

find_path(GLIB_INCLUDE_DIR glib.h
          HINTS ${PC_GLIB_INCLUDEDIR} ${PC_GLIB_INCLUDE_DIRS}
          PATH_SUFFIXES glib )

find_path(GLIB_CONFIG_INCLUDE_DIR
          NAMES glibconfig.h
          PATHS ${PC_GLIB_INCLUDE_DIR} ${PC_GLIB_INCLUDE_DIRS}
          PATH_SUFFIXES lib/glib-2.0/include)

find_library(GLIB_LIBRARY NAMES glib glib-2.0
             HINTS ${PC_GLIB_LIBDIR} ${PC_GLIB_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(glib-2.0  DEFAULT_MSG
                                  GLIB_LIBRARY GLIB_INCLUDE_DIR)

mark_as_advanced(GLIB_INCLUDE_DIR GLIB_LIBRARY )

set(GLIB_LIBRARIES ${GLIB_LIBRARY} )
set(GLIB_INCLUDE_DIRS ${GLIB_INCLUDE_DIR} )
