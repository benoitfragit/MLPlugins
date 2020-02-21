foreach(SUBPATH ${CMAKE_PREFIX_PATH}) 
    find_path(LIBXML2_INCLUDE_DIR libxml/xpath.h
              HINTS ${SUBPATH} )
 
    find_library(LIBXML2_LIBRARY NAMES xml2 libxml2
                 HINTS ${SUBPATH} )
endforeach(SUBPATH)

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE

find_package_handle_standard_args(LibXml2  DEFAULT_MSG
                                  LIBXML2_LIBRARY LIBXML2_INCLUDE_DIR)

mark_as_advanced(LIBXML2_INCLUDE_DIR LIBXML2_LIBRARY )

set(LIBXML2_LIBRARIES ${LIBXML2_LIBRARY})
set(LIBXML2_INCLUDE_DIRS ${LIBXML2_INCLUDE_DIR} )