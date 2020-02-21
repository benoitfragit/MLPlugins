foreach(SUBICONVPATH ${CMAKE_PREFIX_PATH})
    find_path(ICONV_INCLUDE_DIR iconv.h icon
              HINTS ${SUBICONVPATH})

    find_library(ICONV_LIBRARY NAMES iconv
                 HINTS ${SUBICONVPATH} )
endforeach(SUBICONVPATH)

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE

find_package_handle_standard_args(Iconv  DEFAULT_MSG
                                  ICONV_LIBRARY ICONV_INCLUDE_DIR)

mark_as_advanced(ICONV_INCLUDE_DIR ICONV_LIBRARY )

set(ICONV_LIBRARIES ${ICONV_LIBRARY})
set(ICONV_INCLUDE_DIRS ${ICONV_INCLUDE_DIR} )