foreach(SUBPATH ${CMAKE_PREFIX_PATH})
    find_path(ZLIB_INCLUDE_DIR zlib.h
              HINTS ${SUBPATH} )

    find_library(ZLIB_LIBRARY NAMES zlib zdll
                 HINTS ${SUBPATH} )
endforeach(SUBPATH)

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE

find_package_handle_standard_args(Zlib  DEFAULT_MSG
                                  ZLIB_LIBRARY ZLIB_INCLUDE_DIR)

mark_as_advanced(ZLIB_INCLUDE_DIR ZLIB_LIBRARY )

set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
set(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR} )