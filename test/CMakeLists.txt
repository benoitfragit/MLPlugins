cmake_minimum_required(VERSION 2.8.9)
project (test_brain)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(LIBXML2)

set(EXTRA_LIBS "")
if (ENABLE_GLIB_EXTRA)
    find_package(glib-2.0)
    include_directories(${GLIB_INCLUDE_DIR})
    include_directories(${GLIB_CONFIG_INCLUDE_DIR})
    set(EXTRA_LIBS ${EXTRA_LIBS} ${GLIB_LIBRARY})
endif(ENABLE_GLIB_EXTRA)

include_directories(${Brain_BINARY_DIR})
include_directories(${Brain_SOURCE_DIR} ${LIBXML2_INCLUDE_DIR})

add_executable(test_brain main.c)
target_link_libraries (test_brain LINK_PUBLIC Brain ${LIBXML2_LIBRARIES} ${EXTRA_LIBS})




