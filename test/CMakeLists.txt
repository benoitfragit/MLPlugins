cmake_minimum_required(VERSION 2.8.9)
project (test_brain)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(LIBXML2)

include_directories(${Brain_SOURCE_DIR} ${LIBXML2_INCLUDE_DIR})

add_executable(test_brain main.c)
target_link_libraries (test_brain LINK_PUBLIC Brain ${LIBXML2_LIBRARIES})




